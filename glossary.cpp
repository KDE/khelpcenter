
/*
 *  This file is part of the KDE Help Center
 *
 *  Copyright (C) 2002 Frerich Raabe (raabe@kde.org)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "glossary.h"
#include "view.h"

#include <KConfig>
#include <KIconLoader>
#include <KConfigGroup>
#include <KXmlGuiWindow>
#include <KProcess>
#include <QStatusBar>

#include <QTreeWidgetItem>

#include <QFrame>
#include <QListView>
#include <qapplication.h>
#include <QTextStream>

#include <sys/stat.h>
#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>

using namespace KHC;

class SectionItem : public QTreeWidgetItem
{
    public:
        SectionItem( QTreeWidgetItem *parent, const QString &text )
	: QTreeWidgetItem( parent )
        {
	  setText(0,text);
	  setIcon(0,SmallIcon( "help-contents" ));
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

    connect( this, SIGNAL( itemActivated(QTreeWidgetItem *, int) ),
	     this, SLOT( treeItemSelected( QTreeWidgetItem * ) ) );

    setHeaderHidden(true);
    setAllColumnsShowFocus( true );
    setRootIsDecorated( true );

    m_byTopicItem = new QTreeWidgetItem( this );
    m_byTopicItem->setText( 0, i18n( "By Topic" ) );
    m_byTopicItem->setIcon( 0, SmallIcon( "help-contents" ) );

    m_alphabItem = new QTreeWidgetItem( this );
    m_alphabItem->setText( 0, i18n( "Alphabetically" ) );
    m_alphabItem->setIcon( 0, SmallIcon( "character-set" ) );

    m_cacheFile = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + QLatin1Char('/') + "help/glossary.xml" ;
    QDir().mkpath( QFileInfo( m_cacheFile ).absolutePath() );
    
    m_sourceFile = View::langLookup( QLatin1String( "khelpcenter/glossary/index.docbook" ) );
    m_config = KSharedConfig::openConfig();

}

void Glossary::showEvent(QShowEvent *event)
{
    if ( !m_initialized )
    {
        if ( cacheStatus() == NeedRebuild )
	  rebuildGlossaryCache();
        else
	  buildGlossaryTree();
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
	m_config->group("Glossary").readPathEntry( "CachedGlossary", QString() ) != m_sourceFile ||
	m_config->group("Glossary").readEntry( "CachedGlossaryTimestamp" ).toInt() != glossaryCTime() )
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
    connect( meinproc, SIGNAL( finished(int,QProcess::ExitStatus) ),
             this, SLOT( meinprocFinished(int,QProcess::ExitStatus) ) );
    
    *meinproc << QStandardPaths::findExecutable(QStringLiteral( "meinproc5" ) );
    *meinproc << QLatin1String( "--output" ) << m_cacheFile;
    *meinproc << QLatin1String( "--stylesheet" )
              << QStandardPaths::locate(QStandardPaths::GenericDataLocation, QLatin1String( "khelpcenter/glossary.xslt" ) );
    *meinproc << m_sourceFile;

    meinproc->setOutputChannelMode(KProcess::OnlyStderrChannel);
    meinproc->start();
    if (!meinproc->waitForStarted()) 
    {
        qWarning() << "could not start process" << meinproc->program();
        if (mainWindow && !m_alreadyWarned)
	{
            ; // add warning message box with don't display again option 
              // http://api.kde.org/4.0-api/kdelibs-apidocs/kdeui/html/classKDialog.html
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
        qWarning() << "running" << meinproc->program() << "failed with exitCode" << exitCode;
        qWarning() << "stderr output:" << meinproc->readAllStandardError();
        if (mainWindow && !m_alreadyWarned) 
	{
            ; // add warning message box with don't display again option 
              // http://api.kde.org/4.0-api/kdelibs-apidocs/kdeui/html/classKDialog.html
            m_alreadyWarned = true;
        }
        delete meinproc;
        return; 
    }        
    delete meinproc;

    if ( !QFile::exists( m_cacheFile ) )
        return;

    m_config->group("Glossary").writePathEntry( "CachedGlossary", m_sourceFile );
    m_config->group("Glossary").writeEntry( "CachedGlossaryTimestamp", glossaryCTime() );
    m_config->sync();

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

    QDomNodeList sectionNodes = doc.documentElement().elementsByTagName( QLatin1String( "section" ) );
    for ( int i = 0; i < sectionNodes.count(); i++ ) 
    {
        QDomElement sectionElement = sectionNodes.item( i ).toElement();
        QString title = sectionElement.attribute( QLatin1String( "title" ) );
        SectionItem *topicSection = new SectionItem( m_byTopicItem, title );

        QDomNodeList entryNodes = sectionElement.elementsByTagName( QLatin1String( "entry" ) );
        for ( int j = 0; j < entryNodes.count(); j++ ) 
	{
            QDomElement entryElement = entryNodes.item( j ).toElement();

            QString entryId = entryElement.attribute( QLatin1String( "id" ) );
            if ( entryId.isNull() )
                continue;

            QDomElement termElement = childElement( entryElement, QLatin1String( "term" ) );
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

            QDomElement definitionElement = childElement( entryElement, QLatin1String( "definition" ) );
            QString definition = definitionElement.text().simplified();

            GlossaryEntryXRef::List seeAlso;

            QDomElement referencesElement = childElement( entryElement, QLatin1String( "references" ) );
            QDomNodeList referenceNodes = referencesElement.elementsByTagName( QLatin1String( "reference" ) );
            if ( referenceNodes.count() > 0 )
                for ( int k = 0; k < referenceNodes.count(); k++ ) 
		{
                    QDomElement referenceElement = referenceNodes.item( k ).toElement();

                    QString term = referenceElement.attribute( QLatin1String( "term" ) );
                    QString id = referenceElement.attribute( QLatin1String( "id" ) );

                    seeAlso += GlossaryEntryXRef( term, id );
                }

            m_glossEntries.insert( entryId, new GlossaryEntry( term, definition, seeAlso ) );
        }
    }
    sortItems(0, Qt::AscendingOrder);
}

void Glossary::treeItemSelected( QTreeWidgetItem *item )
{
    if ( !item )
        return;

    if ( EntryItem *i = dynamic_cast<EntryItem *>( item ) )
        emit entrySelected( entry( i->id() ) );

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

QString Glossary::entryToHtml( const GlossaryEntry &entry )
{
    QFile htmlFile( QStandardPaths::locate(QStandardPaths::GenericDataLocation, "khelpcenter/glossary.html.in" ) );
    if (!htmlFile.open(QIODevice::ReadOnly))
      return QString( "<html><head></head><body><h3>%1</h3>%2</body></html>" )
             .arg( i18n( "Error" ) )
             .arg( i18n( "Unable to show selected glossary entry: unable to open "
                          "file 'glossary.html.in'!" ) );

    QString seeAlso;
    if (!entry.seeAlso().isEmpty()) 
    {
        seeAlso = i18n("See also: ");
        GlossaryEntryXRef::List seeAlsos = entry.seeAlso();
        GlossaryEntryXRef::List::ConstIterator it = seeAlsos.constBegin();
        GlossaryEntryXRef::List::ConstIterator end = seeAlsos.constEnd();
        for (; it != end; ++it)
	{
            seeAlso += QLatin1String("<a href=\"glossentry:");
            seeAlso += (*it).id();
            seeAlso += QLatin1String("\">") + (*it).term();
            seeAlso += QLatin1String("</a>, ");
        }
        seeAlso = seeAlso.left(seeAlso.length() - 2);
    }
    
    QTextStream htmlStream(&htmlFile);
    return htmlStream.readAll()
           .arg( i18n( "KDE Glossary" ) )
           .arg( entry.term() )
           .arg( entry.definition() )
           .arg( seeAlso );
}

void Glossary::slotSelectGlossEntry( const QString &id )
{
    if ( !m_idDict.contains( id ) )
        return;

    EntryItem *newItem = m_idDict.value( id );
    EntryItem *curItem = dynamic_cast<EntryItem *>( currentItem() );
    if ( curItem != 0 ) 
    {
        if ( curItem->id() == id )
	  return;
	  curItem->parent()->setExpanded( false );
    }

    setCurrentItem( newItem );
}


// vim:ts=4:sw=4:et
