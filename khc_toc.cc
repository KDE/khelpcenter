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

#include <kiconloader.h>
#include <klistview.h>
#include <klocale.h>
#include <kprocess.h>
#include <kstandarddirs.h>

#include <qdom.h>
#include <qheader.h>

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
	clear();

	m_meinprocBuffer.open( IO_WriteOnly );

	KProcess *meinproc = new KProcess;
	connect( meinproc, SIGNAL( receivedStdout( KProcess *, char *, int ) ),
	         this, SLOT( gotMeinprocOutput( KProcess *, char *, int ) ) );
	connect( meinproc, SIGNAL( processExited( KProcess * ) ),
	         this, SLOT( meinprocExited( KProcess * ) ) );

	*meinproc << locate( "exe", "meinproc" );
	*meinproc << "--stylesheet" << locate( "data", "khelpcenter/table-of-contents.xslt" );
	*meinproc << "--stdout";
	*meinproc << file;

	meinproc->start( KProcess::NotifyOnExit, KProcess::Stdout );
}

void khcTOC::gotMeinprocOutput( KProcess * /*meinproc*/, char *data, int len )
{
	m_meinprocBuffer.writeBlock( data, len );
}

void khcTOC::meinprocExited( KProcess *meinproc )
{
	if ( !meinproc->normalExit() || meinproc->exitStatus() != 0 ) {
		delete meinproc;
		return;
	}

	delete meinproc;
	
	QDomDocument doc;
	if ( !doc.setContent( m_meinprocBuffer.buffer() ) )
		return;

	m_meinprocBuffer.flush();
	m_meinprocBuffer.close();

	fill( doc );
}

void khcTOC::fill( const QDomDocument &doc )
{	
	clear();

	QDomNodeList chapters = doc.documentElement().elementsByTagName( "chapter" );
	for ( unsigned int chapterCount = 0; chapterCount < chapters.count(); chapterCount++ ) {
		QDomElement chapElem = chapters.item( chapterCount ).toElement();
		QDomElement chapTitleElem = childElement( chapElem, QString::fromLatin1( "title" ) );
		QString chapTitle = chapTitleElem.text().simplifyWhiteSpace();
		QDomElement chapRefElem = childElement( chapElem, QString::fromLatin1( "anchor" ) );
		QString chapRef = chapRefElem.text().stripWhiteSpace();

		khcTOCChapterItem *chapItem;
		if ( childCount() == 0 )
			chapItem = new khcTOCChapterItem( this, chapTitle, chapRef );
		else
			chapItem = new khcTOCChapterItem( this, lastChild(), chapTitle, chapRef );

		QDomNodeList sections = chapElem.elementsByTagName( "section" );
		for ( unsigned int sectCount = 0; sectCount < sections.count(); sectCount++ ) {
			QDomElement sectElem = sections.item( sectCount ).toElement();
			QDomElement sectTitleElem = childElement( sectElem, QString::fromLatin1( "title" ) );
			QString sectTitle = sectTitleElem.text().simplifyWhiteSpace();
			QDomElement sectRefElem = childElement( sectElem, QString::fromLatin1( "anchor" ) );
			QString sectRef = sectRefElem.text().stripWhiteSpace();

			if ( chapItem->childCount() == 0 )
				new khcTOCSectionItem( chapItem, sectTitle, sectRef );
			else {
				QListViewItem *lastChild = chapItem->firstChild();
				while ( lastChild->nextSibling() )
					lastChild = lastChild->nextSibling();
				new khcTOCSectionItem( chapItem, lastChild, sectTitle, sectRef );
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
	khcTOCItem *tocItem;
	if ( ( tocItem = dynamic_cast<khcTOCItem *>( item ) ) )
		emit itemSelected( tocItem->link() );
}

khcTOCItem::khcTOCItem( khcTOCItem *parent, const QString &text )
	: KListViewItem( parent, text )
{
}

khcTOCItem::khcTOCItem( khcTOCItem *parent, QListViewItem *after, const QString &text )
	: KListViewItem( parent, after, text )
{
}

khcTOCItem::khcTOCItem( khcTOC *parent, const QString &text )
	: KListViewItem( parent, text )
{
}

khcTOCItem::khcTOCItem( khcTOC *parent, QListViewItem *after, const QString &text )
	: KListViewItem( parent, after, text )
{
}

khcTOC *khcTOCItem::toc() const
{
	return static_cast<khcTOC *>( listView() );
}

khcTOCChapterItem::khcTOCChapterItem( khcTOC *parent, const QString &title, const QString &name )
	: khcTOCItem( parent, title ),
	m_name( name )
{
	setOpen( false );
}

khcTOCChapterItem::khcTOCChapterItem( khcTOC *parent, QListViewItem *after, const QString &title, const QString &name )
	: khcTOCItem( parent, after, title ),
	m_name( name )
{
	setOpen( false );
}

void khcTOCChapterItem::setOpen( bool open )
{
	khcTOCItem::setOpen( open );
	
	setPixmap( 0, SmallIcon( open ? "contents" : "contents2" ) );
}

QString khcTOCChapterItem::link() const
{
	return "help:" + toc()->application() + "/" + m_name + ".html";
}

khcTOCSectionItem::khcTOCSectionItem( khcTOCChapterItem *parent, const QString &title, const QString &name )
	: khcTOCItem( parent, title ),
	m_name( name )
{
	setPixmap( 0, SmallIcon( "document" ) );
}

khcTOCSectionItem::khcTOCSectionItem( khcTOCChapterItem *parent, QListViewItem *after, const QString &title, const QString &name )
	: khcTOCItem( parent, after, title ),
	m_name( name )
{
	setPixmap( 0, SmallIcon( "document" ) );
}

QString khcTOCSectionItem::link() const
{
	if ( static_cast<khcTOCSectionItem *>( parent()->firstChild() ) == this )
		return static_cast<khcTOCChapterItem *>( parent() )->link() + "#" + m_name;
	
	return "help:" + toc()->application() + "/" + m_name + ".html";
}

#include "khc_toc.moc"
// vim:ts=4:sw=4:noet
