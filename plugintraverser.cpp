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
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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

#include <qlistview.h>

using namespace KHC;

PluginTraverser::PluginTraverser( Navigator *navigator, QListView *parent )
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

  if (entry->khelpcenterSpecial() == "apps") {
    NavigatorAppItem *appItem;
    if ( mListView )
      appItem = new NavigatorAppItem( mListView, mCurrentItem );
    else
      appItem = new NavigatorAppItem( mParentItem, mCurrentItem );
    KConfig *cfg = kapp->config();
    cfg->setGroup( "General" );
    appItem->setRelpath( cfg->readEntry( "AppsRoot" ) );
    appItem->setIcon( "kmenu" );
    mCurrentItem = appItem;
  } else if ( entry->khelpcenterSpecial() == "scrollkeeper" ) {
    if ( mParentItem ) {
      mCurrentItem = mNavigator->insertScrollKeeperDocs( mParentItem, mCurrentItem );
    }
    return;
  } else {
    if ( mListView )
      mCurrentItem = new NavigatorItem( mListView, mCurrentItem );
    else
      mCurrentItem = new NavigatorItem( mParentItem, mCurrentItem );

    if (entry->khelpcenterSpecial() == "applets" ) {
      mNavigator->insertAppletDocs( mCurrentItem );
    } else if ( entry->khelpcenterSpecial() == "kinfocenter" ||
                entry->khelpcenterSpecial() == "kcontrol" ||
                entry->khelpcenterSpecial() == "konqueror" ) {
      mNavigator->insertParentAppDocs( entry->khelpcenterSpecial(), mCurrentItem );
    } else if ( entry->khelpcenterSpecial() == "kioslave" ) {
      mNavigator->insertIOSlaveDocs( entry->khelpcenterSpecial(), mCurrentItem );
    } else if ( entry->khelpcenterSpecial() == "info" ) {
      mCurrentItem->setPixmap( 0, SmallIcon( "contents2" ) );
      mNavigator->insertInfoDocs( mCurrentItem );
    }
  }

  mCurrentItem->setName( entry->name() );
  mCurrentItem->setUrl( entry->docPath() );

  if ( mCurrentItem->icon().isNull() ) {
    if ( !entry->docExists() ) {
      mCurrentItem->setIcon( "unknown" );
    } else {
      if ( entry->icon().isEmpty() ) {
        if ( entry->isDirectory() ) {
          mCurrentItem->setIcon( "contents2" );
        } else {
          mCurrentItem->setIcon( "document2" );
        }
      } else {
        mCurrentItem->setIcon( entry->icon() );
      }
    }
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