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
#include "infotree.h"
#include "navigatoritem.h"

#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <klistview.h>
#include <kstandarddirs.h>
#include <kurl.h>

#include <qfile.h>
#include <qtextstream.h>

#include <stdlib.h>  // for getenv()

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
    InfoNodeItem( InfoCategoryItem *parent, const QString &text );
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

InfoNodeItem::InfoNodeItem( InfoCategoryItem *parent, const QString &text )
  : NavigatorItem( parent, text )
{
  kdDebug( 1440 ) << "Created info node item: " << text << endl;
}

InfoTree::InfoTree( QObject *parent, const char *name )
  : TreeBuilder( parent, name ),
  m_parentItem( 0 )
{
}

void InfoTree::build( NavigatorItem *parent )
{
  kdDebug( 1400 ) << "Populating info tree." << endl;

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

  QString infoPath = getenv( "INFOPATH" );
  if ( !infoPath.isEmpty() )
    infoDirFiles += QStringList::split( ':', infoPath );

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
  kdDebug( 1400 ) << "Parsing info dir file " << infoDirFileName << endl;

  QFile infoDirFile( infoDirFileName );
  if ( !infoDirFile.open( IO_ReadOnly ) )
    return;

  QTextStream stream( &infoDirFile );
  // Skip introduction blurb.
  while ( !stream.eof() && !stream.readLine().startsWith( "* Menu:" ) );

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

      InfoNodeItem *item = new InfoNodeItem( catItem, appName );
      item->setUrl( url );

      s = stream.readLine();
    }
  }
  infoDirFile.close();
} 

void InfoTree::slotItemSelected( QListViewItem *item )
{
  InfoNodeItem *nodeItem;
  if ( ( nodeItem = dynamic_cast<InfoNodeItem *>( item ) ) )
    emit urlSelected( KURL( nodeItem->url() ) );
}

#include "infotree.moc"
// vim:ts=2:sw=2:et
