/*
    SPDX-FileCopyrightText: 2002 Frerich Raabe <raabe@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "infotree.h"

#include "navigatoritem.h"
#include "docentry.h"
#include "khc_debug.h"

#include <KLocalizedString>

#include <QFile>
#include <QIcon>
#include <QTextStream>

#include <prefs.h>

using namespace KHC;

class InfoCategoryItem : public NavigatorItem
{
  public:
    InfoCategoryItem( NavigatorItem *parent, const QString &text );
	
    void itemExpanded( bool open ) override;
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
  setIcon( 0, QIcon::fromTheme( QStringLiteral("help-contents") ) );
//  qCDebug(KHC_LOG) << "Got category: " << text;
}

void InfoCategoryItem::itemExpanded( bool open )
{
  NavigatorItem::itemExpanded( open );

  if ( open && childCount() > 0 ) setIcon( 0, QIcon::fromTheme( QStringLiteral("help-contents") ) );
// TODO: was contents2 -> needs to be changed to help-contents-alternate or similar
  else setIcon( 0, QIcon::fromTheme( QStringLiteral("help-contents") ) );
}

InfoNodeItem::InfoNodeItem( InfoCategoryItem *parent, const QString &text )
  : NavigatorItem( new DocEntry( text ), parent )
{
  setAutoDeleteDocEntry( true );
//  qCDebug(KHC_LOG) << "Created info node item: " << text;
}

InfoTree::InfoTree( QObject *parent )
  : TreeBuilder( parent ),
    m_parentItem( nullptr )
{
}

void InfoTree::build( NavigatorItem *parent )
{
  qCDebug(KHC_LOG) << "Populating info tree.";

  m_parentItem = parent;

  DocEntry *entry = new DocEntry( i18n( "Alphabetically" ) );
  m_alphabItem = new NavigatorItem( entry, parent );
  m_alphabItem->setAutoDeleteDocEntry( true );
  entry = new DocEntry( i18n( "By Category" ) );
  m_categoryItem = new NavigatorItem( entry, parent );
  m_categoryItem->setAutoDeleteDocEntry( true );

  QStringList infoDirFiles = Prefs::searchpaths();

  const QString infoPath = QFile::decodeName( qgetenv( "INFOPATH" ) );
  if ( !infoPath.isEmpty() )
    infoDirFiles += infoPath.split( QLatin1Char(':'));

  QStringList::ConstIterator it = infoDirFiles.constBegin();
  QStringList::ConstIterator end = infoDirFiles.constEnd();
  for ( ; it != end; ++it ) {
    QString infoDirFileName = *it + QStringLiteral("/dir");
    if ( QFile::exists( infoDirFileName ) )
      parseInfoDirFile( infoDirFileName );
  }

  m_alphabItem->sortChildren( 0, Qt::AscendingOrder /* ascending */ );
}

void InfoTree::parseInfoDirFile( const QString &infoDirFileName )
{
  qCDebug(KHC_LOG) << "Parsing info dir file " << infoDirFileName;

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
        QString url = QStringLiteral("info:/") + s.mid( openBrace + 1, closeBrace - openBrace - 1 );
        if ( dot - closeBrace > 1 )
          url += QLatin1Char('/') + s.mid( closeBrace + 1, dot - closeBrace - 1 );
        else
          url += QLatin1String("/Top");

        InfoNodeItem *item = new InfoNodeItem( catItem, appName );
        item->entry()->setUrl( url );

        const QChar first = appName.at( 0 ).toUpper();
        InfoCategoryItem *alphabSection = alphabSections.value( first );

        if ( alphabSection == nullptr ) {
          alphabSection = new InfoCategoryItem( m_alphabItem, QString( first ) );
          alphabSections.insert( first, alphabSection );
        }

        item = new InfoNodeItem( alphabSection, appName );
        item->entry()->setUrl( url );
      }
    }
    catItem->sortChildren( 0, Qt::AscendingOrder );
  }
  infoDirFile.close();

  for ( InfoCategoryItem *item : qAsConst(alphabSections) ) {
    item->sortChildren( 0, Qt::AscendingOrder );
  }
  m_alphabItem->sortChildren( 0, Qt::AscendingOrder );
  m_categoryItem->sortChildren( 0, Qt::AscendingOrder );
}


// vim:ts=2:sw=2:et
