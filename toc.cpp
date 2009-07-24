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

#include "toc.h"

#include "docentry.h"

#include <kiconloader.h>
#include <kprocess.h>
#include <kstandarddirs.h>
#include <kdebug.h>
#include <kxmlguiwindow.h>
#include <KApplication>
#include <KStatusBar>
#include <klocale.h>

#include <QDir>
#include <QFileInfo>
//Added by qt3to4:
#include <QTextStream>
#include <QPixmap>
#include <sys/stat.h>

using namespace KHC;

class TOCItem : public NavigatorItem
{
    public:
        TOCItem( TOC *parent, Q3ListViewItem *parentItem, Q3ListViewItem *after, const QString &text );

        const TOC *toc() const { return m_toc; }

    private:
        TOC *m_toc;
};

class TOCChapterItem : public TOCItem
{
    public:
        TOCChapterItem( TOC *toc, NavigatorItem *parent, Q3ListViewItem *after, const QString &title,
                const QString &name );

        virtual QString url();

    private:
        QString m_name;
};

class TOCSectionItem : public TOCItem
{
    public:
        TOCSectionItem( TOC *toc, TOCChapterItem *parent, Q3ListViewItem *after, const QString &title,
                const QString &name );

        virtual QString url();

    private:
        QString m_name;
};

bool TOC::m_alreadyWarned = false;

TOC::TOC( NavigatorItem *parentItem )
{
    m_parentItem = parentItem;
}

void TOC::build( const QString &file )
{
    QFileInfo fileInfo( file );
    QString fileName = fileInfo.absoluteFilePath();
    const QStringList resourceDirs = KGlobal::dirs()->resourceDirs( "html" );
    QStringList::ConstIterator it = resourceDirs.begin();
    QStringList::ConstIterator end = resourceDirs.end();
    for ( ; it != end; ++it ) {
        if ( fileName.startsWith( *it ) ) {
            fileName.remove( 0, ( *it ).length() );
            break;
        }
    }

    QString cacheFile = fileName.replace( '/', "__" );
#ifdef Q_WS_WIN
    cacheFile = cacheFile.replace( ':', "_" );
#endif
    m_cacheFile = KStandardDirs::locateLocal( "cache", "help/" + cacheFile );
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
    KXmlGuiWindow *mainWindow = dynamic_cast<KXmlGuiWindow *>( kapp->activeWindow() );

    KProcess *meinproc = new KProcess;
    connect( meinproc, SIGNAL( finished( int, QProcess::ExitStatus) ),
             this, SLOT( meinprocExited( int, QProcess::ExitStatus) ) );

    *meinproc << KStandardDirs::locate("exe", "meinproc4");
    *meinproc << "--stylesheet" << KStandardDirs::locate( "data", "khelpcenter/table-of-contents.xslt" );
    *meinproc << "--output" << m_cacheFile;
    *meinproc << m_sourceFile;

    meinproc->setOutputChannelMode(KProcess::OnlyStderrChannel);
    meinproc->start();
    if (!meinproc->waitForStarted()) {
        kError() << "could not start process" << meinproc->program();
        if (mainWindow && !m_alreadyWarned) {
            ; // add warning message box with don't display again option 
              // http://api.kde.org/4.0-api/kdelibs-apidocs/kdeui/html/classKDialog.html
            m_alreadyWarned = true;
        }
        delete meinproc;
    }
}

void TOC::meinprocExited( int exitCode, QProcess::ExitStatus exitStatus)
{
    KProcess *meinproc = static_cast<KProcess *>(sender());
    KXmlGuiWindow *mainWindow = dynamic_cast<KXmlGuiWindow *>( kapp->activeWindow() );

    if ( exitStatus == QProcess::CrashExit || exitCode != 0 ) {
        kError() << "running" << meinproc->program() << "failed with exitCode" << exitCode;
        kError() << "stderr output:" << meinproc->readAllStandardError(); 
        if (mainWindow && !m_alreadyWarned) {
            ; // add warning message box with don't display again option 
              // http://api.kde.org/4.0-api/kdelibs-apidocs/kdeui/html/classKDialog.html
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

    TOCChapterItem *chapItem = 0;
    QDomNodeList chapters = doc.documentElement().elementsByTagName( "chapter" );
    for ( int chapterCount = 0; chapterCount < chapters.count(); chapterCount++ ) {
        QDomElement chapElem = chapters.item( chapterCount ).toElement();
        QDomElement chapTitleElem = childElement( chapElem, QLatin1String( "title" ) );
        QString chapTitle = chapTitleElem.text().simplified();
        QDomElement chapRefElem = childElement( chapElem, QLatin1String( "anchor" ) );
        QString chapRef = chapRefElem.text().trimmed();

        chapItem = new TOCChapterItem( this, m_parentItem, chapItem, chapTitle, chapRef );

        TOCSectionItem *sectItem = 0;
        QDomNodeList sections = chapElem.elementsByTagName( "section" );
        for ( int sectCount = 0; sectCount < sections.count(); sectCount++ ) {
            QDomElement sectElem = sections.item( sectCount ).toElement();
            QDomElement sectTitleElem = childElement( sectElem, QLatin1String( "title" ) );
            QString sectTitle = sectTitleElem.text().simplified();
            QDomElement sectRefElem = childElement( sectElem, QLatin1String( "anchor" ) );
            QString sectRef = sectRefElem.text().trimmed();

            sectItem = new TOCSectionItem( this, chapItem, sectItem, sectTitle, sectRef );
        }
    }

  m_parentItem->setOpen( true );
}

QDomElement TOC::childElement( const QDomElement &element, const QString &name )
{
    QDomElement e;
    for ( e = element.firstChild().toElement(); !e.isNull(); e = e.nextSibling().toElement() )
        if ( e.tagName() == name )
            break;
    return e;
}

void TOC::slotItemSelected( Q3ListViewItem *item )
{
    TOCItem *tocItem;
    if ( ( tocItem = dynamic_cast<TOCItem *>( item ) ) )
        emit itemSelected( tocItem->entry()->url() );

    item->setOpen( !item->isOpen() );
}

TOCItem::TOCItem( TOC *toc, Q3ListViewItem *parentItem, Q3ListViewItem *after, const QString &text )
    : NavigatorItem( new DocEntry( text ), parentItem, after )
{
        setAutoDeleteDocEntry( true );
    m_toc = toc;
}

TOCChapterItem::TOCChapterItem( TOC *toc, NavigatorItem *parent, Q3ListViewItem *after,
                const QString &title, const QString &name )
    : TOCItem( toc, parent, after, title ),
    m_name( name )
{
    setOpen( false );
    entry()->setUrl(url());
}

QString TOCChapterItem::url()
{
    return QLatin1String("help:") + toc()->application() + QLatin1Char('/') + m_name 
               + QLatin1String(".html");
}

TOCSectionItem::TOCSectionItem( TOC *toc, TOCChapterItem *parent, Q3ListViewItem *after,
                const QString &title, const QString &name )
    : TOCItem( toc, parent, after, title ),
    m_name( name )
{
    setPixmap( 0, SmallIcon( "text-plain" ) );
    entry()->setUrl(url());
}

QString TOCSectionItem::url()
{
    if ( static_cast<TOCSectionItem *>( parent()->firstChild() ) == this )
        return static_cast<TOCChapterItem *>( parent() )->url() + '#' + m_name;

    return "help:" + toc()->application() + '/' + m_name + ".html";
}

#include "toc.moc"
// vim:ts=2:sw=2:et
