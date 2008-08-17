/*
 *  This file is part of the KDE Help Center
 *
 *  Copyright (C) 1999 Matthias Elter (me@kde.org)
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
#include "plugintraverser.h"
#include "docmetainfo.h"
#include "navigator.h"
#include "navigatorappitem.h"
#include "navigatoritem.h"

#include <kapplication.h>
#include <kconfig.h>
#include <kconfiggroup.h>
#include <kdebug.h>
#include <kiconloader.h>

#include <Qt3Support/Q3CheckListItem>
#include <kglobal.h>

using namespace KHC;

PluginTraverser::PluginTraverser( Navigator *navigator, Q3ListView *parent )
  : DocEntryTraverser(),
  mListView( parent ),
  mParentItem( 0 ),
  mCurrentItem( 0 ),
  mNavigator( navigator )
{
}

PluginTraverser::PluginTraverser( Navigator *navigator, NavigatorItem *parent )
  : DocEntryTraverser(),
  mListView( 0 ),
  mParentItem( parent ),
  mCurrentItem( 0 ),
  mNavigator( navigator )
{
}

void PluginTraverser::process( DocEntry *entry )
{
  if ( !mListView && !mParentItem ) {
    kDebug( 1400 ) << "ERROR! Neither mListView nor mParentItem is set.";
    return;
  }

  if ( !entry->docExists() && !mNavigator->showMissingDocs() )
    return;

#if 0
  kDebug() << "PluginTraverser::process(): " << entry->name()
    << " (weight: " << entry->weight() << " parent: "
    << ( mParentItem ? mParentItem->name() : "0" ) << ")" << endl;
#endif

  if ( entry->khelpcenterSpecial() == QLatin1String("apps") ) {
    NavigatorAppItem *appItem;
    entry->setIcon( "kde" );
    if ( mListView )
      appItem = new NavigatorAppItem( entry, mListView, mCurrentItem );
    else
      appItem = new NavigatorAppItem( entry, mParentItem, mCurrentItem );
    KConfigGroup cfg(KGlobal::config(), "General");
    appItem->setRelpath( cfg.readPathEntry( "AppsRoot", QString() ) );
    mCurrentItem = appItem;
  } else if ( entry->khelpcenterSpecial() == QLatin1String("scrollkeeper" )) {
    if ( mParentItem ) {
      mCurrentItem = mNavigator->insertScrollKeeperDocs( mParentItem, mCurrentItem );
    }
    return;
  } else {
    if ( mListView )
      mCurrentItem = new NavigatorItem( entry, mListView, mCurrentItem );
    else
      mCurrentItem = new NavigatorItem( entry, mParentItem, mCurrentItem );

    if ( entry->khelpcenterSpecial() == QLatin1String("konqueror") ) {
      mNavigator->insertParentAppDocs( entry->khelpcenterSpecial(), mCurrentItem );
    } else if ( entry->khelpcenterSpecial() == QLatin1String("kcontrol") ) {
      mNavigator->insertKCMDocs( entry->khelpcenterSpecial(), mCurrentItem, QString("kcontrol") );
    } else if ( entry->khelpcenterSpecial() == QLatin1String("kinfocenter") ) {
      mNavigator->insertKCMDocs( entry->khelpcenterSpecial(), mCurrentItem, QString("kinfocenter") );
    } else if ( entry->khelpcenterSpecial() == QLatin1String("kioslave") ) {
      mNavigator->insertIOSlaveDocs( entry->khelpcenterSpecial(), mCurrentItem );
    } else if ( entry->khelpcenterSpecial() == QLatin1String("info") ) {
      mNavigator->insertInfoDocs( mCurrentItem );
    } else {
      return;
    }
// TODO: was contents2 -> needs to be changed to help-contents-alternate or similar
    mCurrentItem->setPixmap( 0, SmallIcon( QLatin1String("help-contents") ) );
  }
}

DocEntryTraverser *PluginTraverser::createChild( DocEntry * /*entry*/ )
{
  if ( mCurrentItem ) {
    return new PluginTraverser( mNavigator, mCurrentItem );
  }
  kDebug( 1400 ) << "ERROR! mCurrentItem is not set.";
  return 0;
}

// vim:ts=2:sw=2:et
