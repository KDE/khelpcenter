/*
    SPDX-FileCopyrightText: 2002 Frerich Raabe <raabe@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "toc.h"

#include "docentry.h"
#include "khc_debug.h"

#include <QApplication>
#include <QFileInfo>
#include <QIcon>
#include <QStandardPaths>
#include <QTextStream>

#include <KProcess>
#include <KXmlGuiWindow>
#include <docbookxslt.h>

#include <sys/stat.h>

using namespace KHC;

class TOCItem : public NavigatorItem
{
    public:
        TOCItem( TOC *parent, QTreeWidgetItem *parentItem, QTreeWidgetItem *after, const QString &text );

        const TOC *toc() const { return m_toc; }

    private:
        TOC *m_toc = nullptr;
};

class TOCChapterItem : public TOCItem
{
    public:
        TOCChapterItem( TOC *toc, NavigatorItem *parent, QTreeWidgetItem *after, const QString &title,
                const QString &name );

        virtual QString url();

    private:
        QString m_name;
};

class TOCSectionItem : public TOCItem
{
    public:
        TOCSectionItem( TOC *toc, TOCChapterItem *parent, QTreeWidgetItem *after, const QString &title,
                const QString &name );

        virtual QString url();

    private:
        QString m_name;
};

bool TOC::m_alreadyWarned = false;

TOC::TOC( NavigatorItem *parentItem )
    : m_parentItem(parentItem)
{
}

void TOC::build( const QString &file )
{
    QFileInfo fileInfo( file );
    QString fileName = fileInfo.absoluteFilePath();
    const QStringList resourceDirs = KDocTools::documentationDirs();
    QStringList::ConstIterator it = resourceDirs.begin();
    QStringList::ConstIterator end = resourceDirs.end();
    for ( ; it != end; ++it ) {
        if ( fileName.startsWith( *it ) ) {
            fileName.remove( 0, ( *it ).length() );
            break;
        }
    }

    QString cacheFile = fileName.replace( QLatin1Char('/'), QStringLiteral("__") );
#ifdef Q_WS_WIN
    cacheFile.replace( QLatin1Char(':'), QStringLiteral("_") );
#endif
    m_cacheFile = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + QStringLiteral("/help/") + cacheFile ;
    m_sourceFile = file;

    if ( cacheStatus() == NeedRebuild )
        buildCache();
    else
        fillTree();
}

TOC::CacheStatus TOC::cacheStatus() const
{
    if ( !QFile::exists( m_cacheFile ) ||
         sourceFileCTime() != cachedCTime() )
        return NeedRebuild;

    return CacheOk;
}

int TOC::sourceFileCTime() const
{
    struct stat stat_buf;
    stat( QFile::encodeName( m_sourceFile ).data(), &stat_buf );

    return stat_buf.st_ctime;
}

int TOC::cachedCTime() const
{
    QFile f( m_cacheFile );
    if ( !f.open( QIODevice::ReadOnly ) )
        return 0;

    QDomDocument doc;
    if ( !doc.setContent( &f ) )
        return 0;

    QDomComment timestamp = doc.documentElement().lastChild().toComment();

    return timestamp.data().trimmed().toInt();
}

void TOC::buildCache()
{
    KXmlGuiWindow *mainWindow = dynamic_cast<KXmlGuiWindow *>( qobject_cast<QApplication*>(qApp)->activeWindow() );

    KProcess *meinproc = new KProcess;
    connect(meinproc, QOverload<int, QProcess::ExitStatus>::of(&KProcess::finished), this, &TOC::meinprocExited);

    *meinproc << QStandardPaths::findExecutable(QStringLiteral("meinproc5"));
    *meinproc << QStringLiteral("--stylesheet") << QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("khelpcenter/table-of-contents.xslt") );
    *meinproc << QStringLiteral("--output") << m_cacheFile;
    *meinproc << m_sourceFile;

    meinproc->setOutputChannelMode(KProcess::OnlyStderrChannel);
    meinproc->start();
    if (!meinproc->waitForStarted()) {
        qCWarning(KHC_LOG) << "could not start process" << meinproc->program();
        if (mainWindow && !m_alreadyWarned) {
            ; // add warning message box with don't display again option 
            m_alreadyWarned = true;
        }
        delete meinproc;
    }
}

void TOC::meinprocExited( int exitCode, QProcess::ExitStatus exitStatus)
{
    KProcess *meinproc = static_cast<KProcess *>(sender());
    KXmlGuiWindow *mainWindow = dynamic_cast<KXmlGuiWindow *>( qobject_cast<QApplication*>(qApp)->activeWindow() );

    if ( exitStatus == QProcess::CrashExit || exitCode != 0 ) {
        qCWarning(KHC_LOG) << "running" << meinproc->program() << "failed with exitCode" << exitCode;
        qCWarning(KHC_LOG) << "stderr output:" << meinproc->readAllStandardError();
        if (mainWindow && !m_alreadyWarned) {
            ; // add warning message box with don't display again option 
            m_alreadyWarned = true;
        }
        delete meinproc;
        return;
    }

    delete meinproc;

    // add a timestamp to the meinproc4 created xml file
    QFile f( m_cacheFile );
    if ( !f.open( QIODevice::ReadWrite ) )
        return;

    QDomDocument doc;
    if ( !doc.setContent( &f ) )
        return;

    QDomComment timestamp = doc.createComment( QString::number( sourceFileCTime() ) );
    doc.documentElement().appendChild( timestamp );

    // write back updated xml content 
    f.seek( 0 );
    QTextStream stream( &f );
    stream.setCodec( "UTF-8" );
#ifdef Q_WS_WIN
    /*
      the problem that on german systems umlauts are displayed as '?' for unknown (Qt'r related ?) reasons
      is caused by wrong encoding type conversations and has been fixed in kdelibs/kdoctools
      To have propper encoding tags in the xml file, QXmlDocument::save() is used. 
    */
    doc.save(stream, 1, QDomNode::EncodingFromTextStream);

#else
    stream << doc.toString();
#endif
    f.close();
    fillTree();
}

void TOC::fillTree()
{
    QFile f( m_cacheFile );
    if ( !f.open( QIODevice::ReadOnly ) )
        return;

    QDomDocument doc;
    if ( !doc.setContent( &f ) )
        return;

    TOCChapterItem *chapItem = nullptr;
    QDomNodeList chapters = doc.documentElement().elementsByTagName( QStringLiteral("chapter") );
    for ( int chapterCount = 0; chapterCount < chapters.count(); chapterCount++ ) {
        QDomElement chapElem = chapters.item( chapterCount ).toElement();
        QDomElement chapTitleElem = childElement( chapElem, QStringLiteral( "title" ) );
        QString chapTitle = chapTitleElem.text().simplified();
        QDomElement chapRefElem = childElement( chapElem, QStringLiteral( "anchor" ) );
        QString chapRef = chapRefElem.text().trimmed();

        chapItem = new TOCChapterItem( this, m_parentItem, chapItem, chapTitle, chapRef );

        TOCSectionItem *sectItem = nullptr;
        QDomNodeList sections = chapElem.elementsByTagName( QStringLiteral("section") );
        for ( int sectCount = 0; sectCount < sections.count(); sectCount++ ) {
            QDomElement sectElem = sections.item( sectCount ).toElement();
            QDomElement sectTitleElem = childElement( sectElem, QStringLiteral( "title" ) );
            const QString sectTitle = sectTitleElem.text().simplified();
            QDomElement sectRefElem = childElement( sectElem, QStringLiteral( "anchor" ) );
            const QString sectRef = sectRefElem.text().trimmed();

            sectItem = new TOCSectionItem( this, chapItem, sectItem, sectTitle, sectRef );
        }
    }
}

QDomElement TOC::childElement( const QDomElement &element, const QString &name )
{
    QDomElement e;
    for ( e = element.firstChild().toElement(); !e.isNull(); e = e.nextSibling().toElement() )
        if ( e.tagName() == name )
            break;
    return e;
}

void TOC::slotItemSelected( QTreeWidgetItem *item )
{
    TOCItem *tocItem;
    if ( ( tocItem = dynamic_cast<TOCItem *>( item ) ) )
        Q_EMIT itemSelected( tocItem->entry()->url() );

    item->setExpanded( !item->isExpanded() );
}

TOCItem::TOCItem( TOC *toc, QTreeWidgetItem *parentItem, QTreeWidgetItem *after, const QString &text )
    : NavigatorItem( new DocEntry( text ), parentItem, after )
{
        setAutoDeleteDocEntry( true );
    m_toc = toc;
}

TOCChapterItem::TOCChapterItem( TOC *toc, NavigatorItem *parent, QTreeWidgetItem *after,
                const QString &title, const QString &name )
    : TOCItem( toc, parent, after, title ),
    m_name( name )
{
    setExpanded( false );
    entry()->setUrl(url());
}

QString TOCChapterItem::url()
{
    return QLatin1String("help:") + toc()->application() + QLatin1Char('/') + m_name 
               + QLatin1String(".html");
}

TOCSectionItem::TOCSectionItem( TOC *toc, TOCChapterItem *parent, QTreeWidgetItem *after,
                const QString &title, const QString &name )
    : TOCItem( toc, parent, after, title ),
    m_name( name )
{
    setIcon( 0, QIcon::fromTheme( QStringLiteral("text-plain") ) );
    entry()->setUrl(url());
}

QString TOCSectionItem::url()
{
    if ( static_cast<TOCSectionItem *>( parent()->child(0) ) == this )
        return static_cast<TOCChapterItem *>( parent() )->url() + QLatin1Char('#') + m_name;

    return QStringLiteral("help:") + toc()->application() + QLatin1Char('/') + m_name + QStringLiteral(".html");
}


// vim:ts=2:sw=2:et
