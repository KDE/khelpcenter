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
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include "infotree.h"
#include "navigatoritem.h"

#include <kapplication.h>
#include <kconfig.h>
#include <kiconloader.h>
#include <klistview.h>
#include <kstandarddirs.h>
#include <kurl.h>

#include <qfile.h>
#include <qtextstream.h>

using namespace KHC;

class InfoCategoryItem : public NavigatorItem
{
	public:
    InfoCategoryItem( NavigatorItem *parent, const QString &text );
		
		virtual void setOpen( bool open );
};

class InfoNodeItem : public NavigatorItem
{
  public:
    InfoNodeItem( InfoCategoryItem *parent, const QString &url, const QString &text );

    QString url() const { return m_url; }

  private:
    QString m_url;
};

InfoCategoryItem::InfoCategoryItem( NavigatorItem *parent, const QString &text )
  : NavigatorItem( parent, text )
{
  setOpen( false );
}

void InfoCategoryItem::setOpen( bool open )
{
  NavigatorItem::setOpen( open );

	if ( open && childCount() > 0 )	
		setPixmap( 0, SmallIcon( "contents" ) );
	else
		setPixmap( 0, SmallIcon( "contents2" ) );
}

InfoNodeItem::InfoNodeItem( InfoCategoryItem *parent, const QString &url, const QString &text )
  : NavigatorItem( parent, text ),
  m_url( url )
{
}

InfoTree::InfoTree( QObject *parent, const char *name )
  : QObject( parent, name ),
  m_parentItem( 0 )
{
}

void InfoTree::build( NavigatorItem *parent )
{
  m_parentItem = parent;

  connect( parent->listView(), SIGNAL( currentChanged( QListViewItem * ) ),
           this, SLOT( slotItemSelected( QListViewItem * ) ) );

  KConfig *cfg = kapp->config();
  cfg->setGroup( "Info pages" );
  QStringList infoDirFiles = cfg->readListEntry( "Search paths" );
  // Default paths taken fron kdebase/kioslave/info/kde-info2html.conf
  if ( infoDirFiles.isEmpty() ) { 
    infoDirFiles << "/usr/share/info";
    infoDirFiles << "/usr/info";
    infoDirFiles << "/usr/lib/info";
    infoDirFiles << "/usr/local/info";
    infoDirFiles << "/usr/local/lib/info";
    infoDirFiles << "/usr/X11R6/info";
    infoDirFiles << "/usr/X11R6/lib/info";
    infoDirFiles << "/usr/X11R6/lib/xemacs/info";
  }

  QStringList::ConstIterator it = infoDirFiles.begin();
  QStringList::ConstIterator end = infoDirFiles.end();
  for ( ; it != end; ++it ) {
    QString infoDirFileName = *it + "/dir";
    if ( QFile::exists( infoDirFileName ) )
      parseInfoDirFile( infoDirFileName );
  }
}

void InfoTree::parseInfoDirFile( const QString &infoDirFileName )
{ 
  QFile infoDirFile( infoDirFileName );
  if ( !infoDirFile.open( IO_ReadOnly ) )
    return;

  QTextStream stream( &infoDirFile );
  // Skip introduction blurb.
  while ( !stream.eof() && stream.readLine() != "* Menu:" );

  while ( !stream.eof() ) {
    QString s = stream.readLine();
    if ( s.stripWhiteSpace().isEmpty() )
      continue;

    InfoCategoryItem *catItem = new InfoCategoryItem( m_parentItem, s );
    s = stream.readLine();
    while ( !stream.eof() && s.startsWith( "*" ) ) {
      const int colon = s.find( ":" );
      const int openBrace = s.find( "(", colon );
      const int closeBrace = s.find( ")", openBrace );
      const int dot = s.find( ".", closeBrace );

      QString appName = s.mid( 2, colon - 2 );
      QString url = "info:/" + s.mid( openBrace + 1, closeBrace - openBrace - 1 );
      if ( dot - closeBrace > 1 )
        url += "/" + s.mid( closeBrace + 1, dot - closeBrace - 1 );
      else
        url += "/Top";

      new InfoNodeItem( catItem, url, appName );

      s = stream.readLine();
    }
  }
  infoDirFile.close();
} 

void InfoTree::slotItemSelected( QListViewItem *item )
{
  InfoNodeItem *nodeItem;
  if ( ( nodeItem = dynamic_cast<InfoNodeItem *>( item ) ) )
    emit infoPageSelected( KURL( nodeItem->url() ) );
}

#include "infotree.moc"
// vim:ts=2:sw=2:et
