/*
    SPDX-FileCopyrightText: 2002 Frerich Raabe <raabe@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "glossary.h"
#include "view.h"
#include "khc_debug.h"

#include <KProcess>
#include <KXmlGuiWindow>

#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QIcon>
#include <QStandardPaths>
#include <QStatusBar>
#include <QTreeWidgetItem>

#include <sys/stat.h>

#include <prefs.h>

using namespace KHC;

class SectionItem : public QTreeWidgetItem
{
    public:
        SectionItem( QTreeWidgetItem *parent, const QString &text )
        : QTreeWidgetItem( parent )
        {
          setText(0,text);
          setIcon(0,QIcon::fromTheme( QStringLiteral("help-contents") ));
        }
};

class EntryItem : public QTreeWidgetItem
{
    public:
        EntryItem( SectionItem *parent, const QString &term, const QString &id )
        : QTreeWidgetItem( parent ), m_id( id )
        {
          setText(0,term);
        }

        QString id() const { return m_id; }

    private:
        QString m_id;
};

bool Glossary::m_alreadyWarned = false;

Glossary::Glossary( QWidget *parent ) : QTreeWidget( parent )
{
    m_initialized = false;
    setFrameStyle( QFrame::NoFrame );

    connect(this, &Glossary::itemActivated, this, &Glossary::treeItemSelected);

    setHeaderHidden(true);
    setAllColumnsShowFocus( true );
    setRootIsDecorated( true );

    m_byTopicItem = new QTreeWidgetItem( this );
    m_byTopicItem->setText( 0, i18n( "By Topic" ) );
    m_byTopicItem->setIcon( 0, QIcon::fromTheme( QStringLiteral("help-contents") ) );

    m_alphabItem = new QTreeWidgetItem( this );
    m_alphabItem->setText( 0, i18n( "Alphabetically" ) );
    m_alphabItem->setIcon( 0, QIcon::fromTheme( QStringLiteral("character-set") ) );

    m_cacheFile = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + QLatin1Char('/') + QStringLiteral("help/glossary.xml");
    QDir().mkpath( QFileInfo( m_cacheFile ).absolutePath() );

    m_sourceFile = View::langLookup( QStringLiteral( "khelpcenter/glossary/index.docbook" ) );

}

void Glossary::showEvent(QShowEvent *event)
{
    if ( !m_initialized )
    {
        if ( cacheStatus() == NeedRebuild ) {
          rebuildGlossaryCache();
        } else {
          buildGlossaryTree();
        }
        m_initialized = true;
    }
    QTreeWidget::showEvent(event);
}

Glossary::~Glossary()
{
    qDeleteAll( m_glossEntries );
}

const GlossaryEntry &Glossary::entry( const QString &id ) const
{
    return *m_glossEntries[ id ];
}

Glossary::CacheStatus Glossary::cacheStatus() const
{
    if ( !QFile::exists( m_cacheFile ) ||
         Prefs::cachedGlossary() != m_sourceFile ||
         Prefs::cachedGlossaryTimestamp() != glossaryCTime() )
        return NeedRebuild;

    return CacheOk;
}

int Glossary::glossaryCTime() const
{
    struct stat stat_buf;
    stat( QFile::encodeName( m_sourceFile ).data(), &stat_buf );

    return stat_buf.st_ctime;
}

void Glossary::rebuildGlossaryCache()
{
    KXmlGuiWindow *mainWindow = dynamic_cast<KXmlGuiWindow *>( qobject_cast<QApplication*>(qApp)->activeWindow() );
    if (mainWindow)
        mainWindow->statusBar()->showMessage( i18n( "Rebuilding glossary cache..." ) );

    KProcess *meinproc = new KProcess;
    connect(meinproc, QOverload<int, QProcess::ExitStatus>::of(&KProcess::finished), this, &Glossary::meinprocFinished);

    *meinproc << QStandardPaths::findExecutable(QStringLiteral( "meinproc5" ) );
    *meinproc << QStringLiteral( "--output" ) << m_cacheFile;
    *meinproc << QStringLiteral( "--stylesheet" )
              << QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral( "khelpcenter/glossary.xslt" ) );
    *meinproc << m_sourceFile;

    meinproc->setOutputChannelMode(KProcess::OnlyStderrChannel);
    meinproc->start();
    if (!meinproc->waitForStarted())
    {
        qCWarning(KHC_LOG) << "could not start process" << meinproc->program();
        if (mainWindow && !m_alreadyWarned)
        {
            ; // add warning message box with don't display again option
            m_alreadyWarned = true;
        }
        delete meinproc;
    }
}

void Glossary::meinprocFinished( int exitCode, QProcess::ExitStatus exitStatus )
{
    KProcess *meinproc = static_cast<KProcess *>(sender());
    KXmlGuiWindow *mainWindow = dynamic_cast<KXmlGuiWindow *>( qobject_cast<QApplication*>(qApp)->activeWindow() );

    if (exitStatus != QProcess::NormalExit || exitCode != 0)
    {
        qCWarning(KHC_LOG) << "running" << meinproc->program() << "failed with exitCode" << exitCode;
        qCWarning(KHC_LOG) << "stderr output:" << meinproc->readAllStandardError();
        if (mainWindow && !m_alreadyWarned)
        {
            ; // add warning message box with don't display again option
            m_alreadyWarned = true;
        }
        delete meinproc;
        return;
    }
    delete meinproc;

    if ( !QFile::exists( m_cacheFile ) )
        return;

    Prefs::setCachedGlossary( m_sourceFile );
    Prefs::setCachedGlossaryTimestamp( glossaryCTime() );
    Prefs::self()->save();

    m_status = CacheOk;

    if (mainWindow)
        mainWindow->statusBar()->showMessage( i18n( "Rebuilding cache... done." ), 2000 );

    buildGlossaryTree();
}

void Glossary::buildGlossaryTree()
{
    QFile cacheFile(m_cacheFile);
    if ( !cacheFile.open( QIODevice::ReadOnly ) )
        return;

    QDomDocument doc;
    if ( !doc.setContent( &cacheFile ) )
        return;

    QHash< QChar, SectionItem * > alphabSections;

    QDomNodeList sectionNodes = doc.documentElement().elementsByTagName( QStringLiteral( "section" ) );
    for ( int i = 0; i < sectionNodes.count(); i++ )
    {
        QDomElement sectionElement = sectionNodes.item( i ).toElement();
        QString title = sectionElement.attribute( QStringLiteral( "title" ) );
        SectionItem *topicSection = new SectionItem( m_byTopicItem, title );

        QDomNodeList entryNodes = sectionElement.elementsByTagName( QStringLiteral( "entry" ) );
        for ( int j = 0; j < entryNodes.count(); j++ )
        {
            QDomElement entryElement = entryNodes.item( j ).toElement();

            QString entryId = entryElement.attribute( QStringLiteral( "id" ) );
            if ( entryId.isNull() )
                continue;

            QDomElement termElement = childElement( entryElement, QStringLiteral( "term" ) );
            QString term = termElement.text().simplified();

            EntryItem *entry = new EntryItem(topicSection, term, entryId );
            m_idDict.insert( entryId, entry );

            const QChar first = term.at( 0 ).toUpper();
            SectionItem *alphabSection = alphabSections.value( first );

            if ( !alphabSection )
            {
                alphabSection = new SectionItem( m_alphabItem, QString( first ) );
                alphabSections.insert( first, alphabSection );
            }

            new EntryItem( alphabSection, term, entryId );

            QDomElement definitionElement = childElement( entryElement, QStringLiteral( "definition" ) );
            QString definition = definitionElement.text().simplified();

            GlossaryEntryXRef::List seeAlso;

            QDomElement referencesElement = childElement( entryElement, QStringLiteral( "references" ) );
            QDomNodeList referenceNodes = referencesElement.elementsByTagName( QStringLiteral( "reference" ) );
            if ( referenceNodes.count() > 0 )
                for ( int k = 0; k < referenceNodes.count(); k++ )
                {
                    QDomElement referenceElement = referenceNodes.item( k ).toElement();

                    QString term = referenceElement.attribute( QStringLiteral( "term" ) );
                    QString id = referenceElement.attribute( QStringLiteral( "id" ) );

                    seeAlso += GlossaryEntryXRef( term, id );
                }

            m_glossEntries.insert( entryId, new GlossaryEntry( entryId, term, definition, seeAlso ) );
        }
    }
    sortItems(0, Qt::AscendingOrder);
}

void Glossary::treeItemSelected( QTreeWidgetItem *item )
{
    if ( !item )
        return;

    if ( EntryItem *i = dynamic_cast<EntryItem *>( item ) )
        Q_EMIT entrySelected( entry( i->id() ) );

    item->setExpanded( !item->isExpanded() );
}

QDomElement Glossary::childElement( const QDomElement &element, const QString &name )
{
    QDomElement e;
    for ( e = element.firstChild().toElement(); !e.isNull(); e = e.nextSibling().toElement() )
        if ( e.tagName() == name )
            break;
    return e;
}

void Glossary::slotSelectGlossEntry( const QString &id )
{
    if ( !m_idDict.contains( id ) )
        return;

    EntryItem *newItem = m_idDict.value( id );
    EntryItem *curItem = dynamic_cast<EntryItem *>( currentItem() );
    if ( curItem != nullptr )
    {
        if ( curItem->id() == id ) {
          return;
        }
        curItem->parent()->setExpanded( false );
    }

    setCurrentItem( newItem );
}


// vim:ts=4:sw=4:et
