/*
 *  khc_toc.cc - part of the KDE Help Center
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
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include "khc_toc.h"

#include <kdebug.h>
#include <kiconloader.h>
#include <klistview.h>
#include <klocale.h>
#include <kprocess.h>
#include <kstandarddirs.h>

#include <qdom.h>
#include <qfileinfo.h>
#include <qheader.h>

#include <sys/stat.h>

class TOCItem : public KListViewItem
{
	public:
		TOCItem( TOCItem *parent, const QString &text );
		TOCItem( TOCItem *parent, QListViewItem *after, const QString &text );
		TOCItem( khcTOC *parent, const QString &text );
		TOCItem( khcTOC *parent, QListViewItem *after, const QString &text );

		virtual QString link() const = 0;

		khcTOC *toc() const;
};

class TOCChapterItem : public TOCItem
{
	public:
		TOCChapterItem( khcTOC *parent, const QString &title, const QString &name );
		TOCChapterItem( khcTOC *parent, QListViewItem *after, const QString &title, const QString &name );

		virtual QString link() const;
		
		virtual void setOpen( bool open );
	
	private:
		QString m_name;
};

class TOCSectionItem : public TOCItem
{
	public:
		TOCSectionItem( TOCChapterItem *parent, const QString &title, const QString &name );
		TOCSectionItem( TOCChapterItem *parent, QListViewItem *after, const QString &title, const QString &name );

		virtual QString link() const;
	
	private:
		QString m_name;
};

khcTOC::khcTOC( QWidget *parent ) : KListView( parent, "khcTOC" )
{
	connect( this, SIGNAL( executed( QListViewItem * ) ),
	         this, SLOT( slotItemSelected( QListViewItem * ) ) );
	connect( this, SIGNAL( returnPressed( QListViewItem * ) ),
	         this, SLOT( slotItemSelected( QListViewItem * ) ) );
	
	setFrameStyle( QFrame::Panel | QFrame::Sunken );
	addColumn( QString::null );
	header()->hide();
	setSorting( -1, true );
	setRootIsDecorated( true );

	reset();
}

void khcTOC::reset()
{
	clear();

	insertItem( new KListViewItem( this, i18n( "No manual selected" ) ) );
}

void khcTOC::build( const QString &file )
{
	QFileInfo fileInfo( file );
	QString cacheFile = QStringList::split( "/", fileInfo.dirPath() ).last() + ".toc.xml";
	m_cacheFile = locateLocal( "cache", "help/" + cacheFile );
	m_sourceFile = file;

	if ( cacheStatus() == NeedRebuild )
		buildCache();
	else
		fillTree();
}

khcTOC::CacheStatus khcTOC::cacheStatus() const
{
	if ( !QFile::exists( m_cacheFile ) ||
	     sourceFileCTime() != cachedCTime() )
		return NeedRebuild;

	return CacheOk;
}

int khcTOC::sourceFileCTime() const
{
	struct stat stat_buf;
	stat( QFile::encodeName( m_sourceFile ).data(), &stat_buf );

	return stat_buf.st_ctime;
}

int khcTOC::cachedCTime() const
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

void khcTOC::buildCache()
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

void khcTOC::meinprocExited( KProcess *meinproc )
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
	stream << doc.toString();

	f.close();

	fillTree();
}

void khcTOC::fillTree()
{
	QFile f( m_cacheFile );
	if ( !f.open( IO_ReadOnly ) )
		return;

	QDomDocument doc;
	if ( !doc.setContent( &f ) )
		return;
	
	clear();

	QDomNodeList chapters = doc.documentElement().elementsByTagName( "chapter" );
	for ( unsigned int chapterCount = 0; chapterCount < chapters.count(); chapterCount++ ) {
		QDomElement chapElem = chapters.item( chapterCount ).toElement();
		QDomElement chapTitleElem = childElement( chapElem, QString::fromLatin1( "title" ) );
		QString chapTitle = chapTitleElem.text().simplifyWhiteSpace();
		QDomElement chapRefElem = childElement( chapElem, QString::fromLatin1( "anchor" ) );
		QString chapRef = chapRefElem.text().stripWhiteSpace();

		TOCChapterItem *chapItem;
		if ( childCount() == 0 )
			chapItem = new TOCChapterItem( this, chapTitle, chapRef );
		else
			chapItem = new TOCChapterItem( this, lastChild(), chapTitle, chapRef );

		QDomNodeList sections = chapElem.elementsByTagName( "section" );
		for ( unsigned int sectCount = 0; sectCount < sections.count(); sectCount++ ) {
			QDomElement sectElem = sections.item( sectCount ).toElement();
			QDomElement sectTitleElem = childElement( sectElem, QString::fromLatin1( "title" ) );
			QString sectTitle = sectTitleElem.text().simplifyWhiteSpace();
			QDomElement sectRefElem = childElement( sectElem, QString::fromLatin1( "anchor" ) );
			QString sectRef = sectRefElem.text().stripWhiteSpace();

			if ( chapItem->childCount() == 0 )
				new TOCSectionItem( chapItem, sectTitle, sectRef );
			else {
				QListViewItem *lastChild = chapItem->firstChild();
				while ( lastChild->nextSibling() )
					lastChild = lastChild->nextSibling();
				new TOCSectionItem( chapItem, lastChild, sectTitle, sectRef );
			}
		}
	}
}

QDomElement khcTOC::childElement( const QDomElement &element, const QString &name )
{
	QDomElement e;
	for ( e = element.firstChild().toElement(); !e.isNull(); e = e.nextSibling().toElement() )
		if ( e.tagName() == name )
			break;
	return e;
}

void khcTOC::slotItemSelected( QListViewItem *item )
{
	TOCItem *tocItem;
	if ( ( tocItem = dynamic_cast<TOCItem *>( item ) ) )
		emit itemSelected( tocItem->link() );

	item->setOpen( !item->isOpen() );
}

TOCItem::TOCItem( TOCItem *parent, const QString &text )
	: KListViewItem( parent, text )
{
}

TOCItem::TOCItem( TOCItem *parent, QListViewItem *after, const QString &text )
	: KListViewItem( parent, after, text )
{
}

TOCItem::TOCItem( khcTOC *parent, const QString &text )
	: KListViewItem( parent, text )
{
}

TOCItem::TOCItem( khcTOC *parent, QListViewItem *after, const QString &text )
	: KListViewItem( parent, after, text )
{
}

khcTOC *TOCItem::toc() const
{
	return static_cast<khcTOC *>( listView() );
}

TOCChapterItem::TOCChapterItem( khcTOC *parent, const QString &title, const QString &name )
	: TOCItem( parent, title ),
	m_name( name )
{
	setOpen( false );
}

TOCChapterItem::TOCChapterItem( khcTOC *parent, QListViewItem *after, const QString &title, const QString &name )
	: TOCItem( parent, after, title ),
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

QString TOCChapterItem::link() const
{
	return "help:" + toc()->application() + "/" + m_name + ".html";
}

TOCSectionItem::TOCSectionItem( TOCChapterItem *parent, const QString &title, const QString &name )
	: TOCItem( parent, title ),
	m_name( name )
{
	setPixmap( 0, SmallIcon( "document" ) );
}

TOCSectionItem::TOCSectionItem( TOCChapterItem *parent, QListViewItem *after, const QString &title, const QString &name )
	: TOCItem( parent, after, title ),
	m_name( name )
{
	setPixmap( 0, SmallIcon( "document" ) );
}

QString TOCSectionItem::link() const
{
	if ( static_cast<TOCSectionItem *>( parent()->firstChild() ) == this )
		return static_cast<TOCChapterItem *>( parent() )->link() + "#" + m_name;
	
	return "help:" + toc()->application() + "/" + m_name + ".html";
}

#include "khc_toc.moc"
// vim:ts=4:sw=4:noet
