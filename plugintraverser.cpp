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
 *  Foundation, Inc., 51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include "plugintraverser.h"
#include "docmetainfo.h"
#include "navigator.h"
#include "navigatorappitem.h"
#include "navigatoritem.h"

#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kiconloader.h>

#include <q3listview.h>

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
    kdDebug( 1400 ) << "ERROR! Neither mListView nor mParentItem is set." << endl;
    return;
  }

  if ( !entry->docExists() & !mNavigator->showMissingDocs() )
    return;

#if 0
  kdDebug() << "PluginTraverser::process(): " << entry->name()
    << " (weight: " << entry->weight() << " parent: "
    << ( mParentItem ? mParentItem->name() : "0" ) << ")" << endl;
#endif

  if ( entry->khelpcenterSpecial() == "apps" ) {
    NavigatorAppItem *appItem;
    entry->setIcon( "kmenu" );
    if ( mListView )
      appItem = new NavigatorAppItem( entry, mListView, mCurrentItem );
    else
      appItem = new NavigatorAppItem( entry, mParentItem, mCurrentItem );
    KConfig *cfg = kapp->config();
    cfg->setGroup( "General" );
    appItem->setRelpath( cfg->readPathEntry( "AppsRoot" ) );
    mCurrentItem = appItem;
  } else if ( entry->khelpcenterSpecial() == "scrollkeeper" ) {
    if ( mParentItem ) {
      mCurrentItem = mNavigator->insertScrollKeeperDocs( mParentItem, mCurrentItem );
    }
    return;
  } else {
    if ( mListView )
      mCurrentItem = new NavigatorItem( entry, mListView, mCurrentItem );
    else
      mCurrentItem = new NavigatorItem( entry, mParentItem, mCurrentItem );

    if (entry->khelpcenterSpecial() == "applets" ) {
      mNavigator->insertAppletDocs( mCurrentItem );
    } else if ( entry->khelpcenterSpecial() == "kinfocenter" ||
                entry->khelpcenterSpecial() == "kcontrol" ||
                entry->khelpcenterSpecial() == "konqueror" ) {
      mNavigator->insertParentAppDocs( entry->khelpcenterSpecial(), mCurrentItem );
    } else if ( entry->khelpcenterSpecial() == "kioslave" ) {
      mNavigator->insertIOSlaveDocs( entry->khelpcenterSpecial(), mCurrentItem );
    } else if ( entry->khelpcenterSpecial() == "info" ) {
      mNavigator->insertInfoDocs( mCurrentItem );
    } else {
      return;
    }
    mCurrentItem->setPixmap( 0, SmallIcon( "contents2" ) );
  }
}

DocEntryTraverser *PluginTraverser::createChild( DocEntry * /*entry*/ )
{
  if ( mCurrentItem ) {
    return new PluginTraverser( mNavigator, mCurrentItem );
  }
  kdDebug( 1400 ) << "ERROR! mCurrentItem is not set." << endl;
  return 0;
}

// vim:ts=2:sw=2:et
