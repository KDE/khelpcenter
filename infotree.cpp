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

#include "infotree.h"

#include "navigatoritem.h"
#include "docentry.h"

#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <k3listview.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kurl.h>

#include <QFile>
#include <QTextStream>
#include <QPixmap>
#include <stdlib.h>  // for getenv()
#include <kglobal.h>

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
  : NavigatorItem( new DocEntry( text ), parent )
{
  setAutoDeleteDocEntry( true );
  setOpen( false );
//  kDebug(1400) << "Got category: " << text;
}

void InfoCategoryItem::setOpen( bool open )
{
  NavigatorItem::setOpen( open );

  if ( open && childCount() > 0 ) setPixmap( 0, SmallIcon( "help-contents" ) );
  else setPixmap( 0, SmallIcon( "contents2" ) );
}

InfoNodeItem::InfoNodeItem( InfoCategoryItem *parent, const QString &text )
  : NavigatorItem( new DocEntry( text ), parent )
{
  setAutoDeleteDocEntry( true );
//  kDebug( 1400 ) << "Created info node item: " << text;
}

InfoTree::InfoTree( QObject *parent )
  : TreeBuilder( parent ),
    m_parentItem( 0 )
{
}

void InfoTree::build( NavigatorItem *parent )
{
  kDebug( 1400 ) << "Populating info tree.";

  m_parentItem = parent;

  DocEntry *entry = new DocEntry( i18n( "Alphabetically" ) );
  m_alphabItem = new NavigatorItem( entry, parent );
  m_alphabItem->setAutoDeleteDocEntry( true );
  entry = new DocEntry( i18n( "By Category" ) );
  m_categoryItem = new NavigatorItem( entry, parent );
  m_categoryItem->setAutoDeleteDocEntry( true );

  KConfigGroup cfg(KGlobal::config(), "Info pages");
  QStringList infoDirFiles = cfg.readEntry( "Search paths" , QStringList() );
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

  QString infoPath = ::getenv( "INFOPATH" );
  if ( !infoPath.isEmpty() )
    infoDirFiles += infoPath.split( ':');

  QStringList::ConstIterator it = infoDirFiles.begin();
  QStringList::ConstIterator end = infoDirFiles.end();
  for ( ; it != end; ++it ) {
    QString infoDirFileName = *it + "/dir";
    if ( QFile::exists( infoDirFileName ) )
      parseInfoDirFile( infoDirFileName );
  }

  m_alphabItem->sortChildItems( 0, true /* ascending */ );
}

void InfoTree::parseInfoDirFile( const QString &infoDirFileName )
{
  kDebug( 1400 ) << "Parsing info dir file " << infoDirFileName;

  QFile infoDirFile( infoDirFileName );
  if ( !infoDirFile.open( QIODevice::ReadOnly ) )
    return;

  QTextStream stream( &infoDirFile );
  // Skip introduction blurb.
  while ( !stream.atEnd() && !stream.readLine().startsWith( "* Menu:" ) );

  while ( !stream.atEnd() ) {
    QString s = stream.readLine();
    if ( s.trimmed().isEmpty() )
      continue;

    InfoCategoryItem *catItem = new InfoCategoryItem( m_categoryItem, s );
    while ( !stream.atEnd() && !s.trimmed().isEmpty() ) {
      s = stream.readLine();
      if ( s[ 0 ] == '*' ) {
        const int colon = s.indexOf( ":" );
        const int openBrace = s.indexOf( "(", colon );
        const int closeBrace = s.indexOf( ")", openBrace );
        const int dot = s.indexOf( ".", closeBrace );

        QString appName = s.mid( 2, colon - 2 );
        QString url = "info:/" + s.mid( openBrace + 1, closeBrace - openBrace - 1 );
        if ( dot - closeBrace > 1 )
          url += QLatin1Char('/') + s.mid( closeBrace + 1, dot - closeBrace - 1 );
        else
          url += QLatin1String("/Top");

        InfoNodeItem *item = new InfoNodeItem( catItem, appName );
        item->entry()->setUrl( url );

        InfoCategoryItem *alphabSection = 0;
        for ( Q3ListViewItem* it=m_alphabItem->firstChild(); it; it=it->nextSibling() ) {
          if ( it->text( 0 ) == QString( appName[ 0 ].toUpper() ) ) {
            alphabSection = static_cast<InfoCategoryItem *>( it );
            break;
          }
        }

        if ( alphabSection == 0 )
          alphabSection = new InfoCategoryItem( m_alphabItem, QString( appName[ 0 ].toUpper() ) );

        item = new InfoNodeItem( alphabSection, appName );
        item->entry()->setUrl( url );
      }
    }
  }
  infoDirFile.close();
}

#include "infotree.moc"
// vim:ts=2:sw=2:et
