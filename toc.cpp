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
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "toc.h"

#include "docentry.h"

#include <kiconloader.h>
#include <kprocess.h>
#include <kstandarddirs.h>

#include <qdir.h>
#include <qfileinfo.h>

#include <sys/stat.h>

using namespace KHC;

class TOCItem : public NavigatorItem
{
	public:
		TOCItem( TOC *parent, QListViewItem *parentItem, QListViewItem *after, const QString &text );

		const TOC *toc() const { return m_toc; }
	
	private:
		TOC *m_toc;
};

class TOCChapterItem : public TOCItem
{
	public:
		TOCChapterItem( TOC *toc, NavigatorItem *parent, QListViewItem *after, const QString &title, 
				const QString &name );

		virtual QString url();
		
		virtual void setOpen( bool open );
	
	private:
		QString m_name;
};

class TOCSectionItem : public TOCItem
{
	public:
		TOCSectionItem( TOC *toc, TOCChapterItem *parent, QListViewItem *after, const QString &title, 
				const QString &name );

		virtual QString url();
	
	private:
		QString m_name;
};

TOC::TOC( NavigatorItem *parentItem )
{
	m_parentItem = parentItem;
}

void TOC::build( const QString &file )
{
	QFileInfo fileInfo( file );
	QString fileName = fileInfo.absFilePath();
	const QStringList resourceDirs = KGlobal::dirs()->resourceDirs( "html" );
	QStringList::ConstIterator it = resourceDirs.begin();
	QStringList::ConstIterator end = resourceDirs.end();
	for ( ; it != end; ++it ) {
		if ( fileName.startsWith( *it ) ) {
			fileName.remove( 0, ( *it ).length() );
			break;
		}
	}

	QString cacheFile = fileName.replace( QDir::separator(), "__" );
	m_cacheFile = locateLocal( "cache", "help/" + cacheFile );
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
	if ( !f.open( IO_ReadOnly ) )
		return 0;
	
	QDomDocument doc;
	if ( !doc.setContent( &f ) )
		return 0;

	QDomComment timestamp = doc.documentElement().lastChild().toComment();

	return timestamp.data().stripWhiteSpace().toInt();
}

void TOC::buildCache()
{
	KProcess *meinproc = new KProcess;
	connect( meinproc, SIGNAL( processExited( KProcess * ) ),
	         this, SLOT( meinprocExited( KProcess * ) ) );

	*meinproc << locate( "exe", "meinproc" );
	*meinproc << "--stylesheet" << locate( "data", "khelpcenter/table-of-contents.xslt" );
	*meinproc << "--output" << m_cacheFile;
	*meinproc << m_sourceFile;

	meinproc->start( KProcess::NotifyOnExit );
}

void TOC::meinprocExited( KProcess *meinproc )
{
	if ( !meinproc->normalExit() || meinproc->exitStatus() != 0 ) {
		delete meinproc;
		return;
	}

	delete meinproc;

	QFile f( m_cacheFile );
	if ( !f.open( IO_ReadWrite ) )
		return;

	QDomDocument doc;
	if ( !doc.setContent( &f ) )
		return;

	QDomComment timestamp = doc.createComment( QString::number( sourceFileCTime() ) );
	doc.documentElement().appendChild( timestamp );

	f.at( 0 );
	QTextStream stream( &f );
	stream.setEncoding( QTextStream::UnicodeUTF8 );
	stream << doc.toString();

	f.close();

	fillTree();
}

void TOC::fillTree()
{
	QFile f( m_cacheFile );
	if ( !f.open( IO_ReadOnly ) )
		return;

	QDomDocument doc;
	if ( !doc.setContent( &f ) )
		return;
	
	TOCChapterItem *chapItem = 0;
	QDomNodeList chapters = doc.documentElement().elementsByTagName( "chapter" );
	for ( unsigned int chapterCount = 0; chapterCount < chapters.count(); chapterCount++ ) {
		QDomElement chapElem = chapters.item( chapterCount ).toElement();
		QDomElement chapTitleElem = childElement( chapElem, QString::fromLatin1( "title" ) );
		QString chapTitle = chapTitleElem.text().simplifyWhiteSpace();
		QDomElement chapRefElem = childElement( chapElem, QString::fromLatin1( "anchor" ) );
		QString chapRef = chapRefElem.text().stripWhiteSpace();

		chapItem = new TOCChapterItem( this, m_parentItem, chapItem, chapTitle, chapRef );

		TOCSectionItem *sectItem = 0;
		QDomNodeList sections = chapElem.elementsByTagName( "section" );
		for ( unsigned int sectCount = 0; sectCount < sections.count(); sectCount++ ) {
			QDomElement sectElem = sections.item( sectCount ).toElement();
			QDomElement sectTitleElem = childElement( sectElem, QString::fromLatin1( "title" ) );
			QString sectTitle = sectTitleElem.text().simplifyWhiteSpace();
			QDomElement sectRefElem = childElement( sectElem, QString::fromLatin1( "anchor" ) );
			QString sectRef = sectRefElem.text().stripWhiteSpace();

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

void TOC::slotItemSelected( QListViewItem *item )
{
	TOCItem *tocItem;
	if ( ( tocItem = dynamic_cast<TOCItem *>( item ) ) )
		emit itemSelected( tocItem->entry()->url() );

	item->setOpen( !item->isOpen() );
}

TOCItem::TOCItem( TOC *toc, QListViewItem *parentItem, QListViewItem *after, const QString &text )
	: NavigatorItem( new DocEntry( text ), parentItem, after )
{
        setAutoDeleteDocEntry( true );
	m_toc = toc;
}

TOCChapterItem::TOCChapterItem( TOC *toc, NavigatorItem *parent, QListViewItem *after,
				const QString &title, const QString &name )
	: TOCItem( toc, parent, after, title ),
	m_name( name )
{
	setOpen( false );
}

void TOCChapterItem::setOpen( bool open )
{
	TOCItem::setOpen( open );

	if ( open && childCount() > 0 )	
		setPixmap( 0, SmallIcon( "contents" ) );
	else
		setPixmap( 0, SmallIcon( "contents2" ) );
}

QString TOCChapterItem::url()
{
	return "help:" + toc()->application() + "/" + m_name + ".html";
}

TOCSectionItem::TOCSectionItem( TOC *toc, TOCChapterItem *parent, QListViewItem *after,
				const QString &title, const QString &name )
	: TOCItem( toc, parent, after, title ),
	m_name( name )
{
	setPixmap( 0, SmallIcon( "document" ) );
}

QString TOCSectionItem::url()
{
	if ( static_cast<TOCSectionItem *>( parent()->firstChild() ) == this )
		return static_cast<TOCChapterItem *>( parent() )->url() + "#" + m_name;
	
	return "help:" + toc()->application() + "/" + m_name + ".html";
}

#include "toc.moc"
// vim:ts=2:sw=2:et
