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
#include "khc_debug.h"
#include "prefs.h"

#include <KLocalizedString>

#include <QFile>
#include <QTextStream>
#include <QIcon>

#include <stdlib.h>  // for getenv()

using namespace KHC;

class InfoCategoryItem : public NavigatorItem
{
  public:
    InfoCategoryItem( NavigatorItem *parent, const QString &text );
	
    virtual void itemExpanded( bool open );
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
  setExpanded( false );
  setIcon( 0, QIcon::fromTheme( "help-contents" ) );
//  khcDebug() << "Got category: " << text;
}

void InfoCategoryItem::itemExpanded( bool open )
{
  NavigatorItem::itemExpanded( open );

  if ( open && childCount() > 0 ) setIcon( 0, QIcon::fromTheme( "help-contents" ) );
// TODO: was contents2 -> needs to be changed to help-contents-alternate or similar
  else setIcon( 0, QIcon::fromTheme( "help-contents" ) );
}

InfoNodeItem::InfoNodeItem( InfoCategoryItem *parent, const QString &text )
  : NavigatorItem( new DocEntry( text ), parent )
{
  setAutoDeleteDocEntry( true );
//  khcDebug() << "Created info node item: " << text;
}

InfoTree::InfoTree( QObject *parent )
  : TreeBuilder( parent ),
    m_parentItem( 0 )
{
}

void InfoTree::build( NavigatorItem *parent )
{
  khcDebug() << "Populating info tree.";

  m_parentItem = parent;

  DocEntry *entry = new DocEntry( i18n( "Alphabetically" ) );
  m_alphabItem = new NavigatorItem( entry, parent );
  m_alphabItem->setAutoDeleteDocEntry( true );
  entry = new DocEntry( i18n( "By Category" ) );
  m_categoryItem = new NavigatorItem( entry, parent );
  m_categoryItem->setAutoDeleteDocEntry( true );

  QStringList infoDirFiles = Prefs::searchpaths();

  QString infoPath = ::getenv( "INFOPATH" );
  if ( !infoPath.isEmpty() )
    infoDirFiles += infoPath.split( ':');

  QStringList::ConstIterator it = infoDirFiles.constBegin();
  QStringList::ConstIterator end = infoDirFiles.constEnd();
  for ( ; it != end; ++it ) {
    QString infoDirFileName = *it + "/dir";
    if ( QFile::exists( infoDirFileName ) )
      parseInfoDirFile( infoDirFileName );
  }

  m_alphabItem->sortChildren( 0, Qt::AscendingOrder /* ascending */ );
}

void InfoTree::parseInfoDirFile( const QString &infoDirFileName )
{
  khcDebug() << "Parsing info dir file " << infoDirFileName;

  QFile infoDirFile( infoDirFileName );
  if ( !infoDirFile.open( QIODevice::ReadOnly ) )
    return;

  QTextStream stream( &infoDirFile );
  // Skip introduction blurb.
  while ( !stream.atEnd() && !stream.readLine().startsWith( QLatin1String("* Menu:") ) ) {
      ;
  }

  QHash< QChar, InfoCategoryItem * > alphabSections;

  while ( !stream.atEnd() ) {
    QString s = stream.readLine();
    if ( s.trimmed().isEmpty() )
      continue;

    InfoCategoryItem *catItem = new InfoCategoryItem( m_categoryItem, s );
    while ( !stream.atEnd() ) {
      s = stream.readLine();
      if ( s.isEmpty() )
        break;
      if ( s.at( 0 ) == QLatin1Char( '*' ) ) {
        const int colon = s.indexOf( QLatin1Char( ':' ) );
        const int openBrace = s.indexOf( QLatin1Char( '(' ), colon );
        const int closeBrace = s.indexOf( QLatin1Char( ')' ), openBrace );
        const int dot = s.indexOf( QLatin1Char( '.' ), closeBrace );

        QString appName = s.mid( 2, colon - 2 );
        QString url = "info:/" + s.mid( openBrace + 1, closeBrace - openBrace - 1 );
        if ( dot - closeBrace > 1 )
          url += QLatin1Char('/') + s.mid( closeBrace + 1, dot - closeBrace - 1 );
        else
          url += QLatin1String("/Top");

        InfoNodeItem *item = new InfoNodeItem( catItem, appName );
        item->entry()->setUrl( url );

        const QChar first = appName.at( 0 ).toUpper();
        InfoCategoryItem *alphabSection = alphabSections.value( first );

        if ( alphabSection == 0 ) {
          alphabSection = new InfoCategoryItem( m_alphabItem, QString( first ) );
          alphabSections.insert( first, alphabSection );
        }

        item = new InfoNodeItem( alphabSection, appName );
        item->entry()->setUrl( url );
      }
    }
  }
  infoDirFile.close();
}


// vim:ts=2:sw=2:et
