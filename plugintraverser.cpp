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
#include "navigatorappgroupitem.h"
#include "navigatoritem.h"
#include "khc_debug.h"

#include <KConfig>

#include <QIcon>

#include <prefs.h>

using namespace KHC;

PluginTraverser::PluginTraverser( Navigator *navigator, QTreeWidget *parent )
  : DocEntryTraverser(),
  mListView( parent ),
  mParentItem( nullptr ),
  mCurrentItem( nullptr ),
  mNavigator( navigator )
{
}

PluginTraverser::PluginTraverser( Navigator *navigator, NavigatorItem *parent )
  : DocEntryTraverser(),
  mListView( nullptr ),
  mParentItem( parent ),
  mCurrentItem( nullptr ),
  mNavigator( navigator )
{
}

void PluginTraverser::process( DocEntry *entry )
{
  if ( !mListView && !mParentItem ) {
    qCWarning(KHC_LOG) << "ERROR! Neither mListView nor mParentItem is set.";
    return;
  }

  if ( !entry->docExists() && !Prefs::showMissingDocs() )
    return;

  if ( entry->khelpcenterSpecial().isEmpty() ) {
    if ( mListView )
      mCurrentItem = new NavigatorAppItem( entry, mListView, mCurrentItem );
    else
      mCurrentItem = new NavigatorAppItem( entry, mParentItem, mCurrentItem );
  } else if ( entry->khelpcenterSpecial() == QLatin1String("apps") ) {
    NavigatorAppGroupItem *appItem;
    entry->setIcon( QStringLiteral("kde") );
    if ( mListView )
      appItem = new NavigatorAppGroupItem( entry, mListView, mCurrentItem );
    else
      appItem = new NavigatorAppGroupItem( entry, mParentItem, mCurrentItem );
    appItem->setRelpath( Prefs::appsRoot() );
    mCurrentItem = appItem;
  } else {
    if ( mListView )
      mCurrentItem = new NavigatorItem( entry, mListView, mCurrentItem );
    else
      mCurrentItem = new NavigatorItem( entry, mParentItem, mCurrentItem );

    if ( entry->khelpcenterSpecial() == QLatin1String("konqueror") ) {
      mNavigator->insertParentAppDocs( entry->khelpcenterSpecial(), mCurrentItem );
    } else if ( entry->khelpcenterSpecial() == QLatin1String("kcontrol") ) {
      mNavigator->insertKCMDocs( entry->khelpcenterSpecial(), mCurrentItem, QStringLiteral("kcontrol") );
    } else if ( entry->khelpcenterSpecial() == QLatin1String("konquerorcontrol") ) {
      mNavigator->insertKCMDocs( entry->khelpcenterSpecial(), mCurrentItem, QStringLiteral("konquerorcontrol") );
    } else if ( entry->khelpcenterSpecial() == QLatin1String("filemanagercontrol") ) {
      mNavigator->insertKCMDocs( entry->khelpcenterSpecial(), mCurrentItem, QStringLiteral("filemanagercontrol") );
    } else if ( entry->khelpcenterSpecial() == QLatin1String("browsercontrol") ) {
      mNavigator->insertKCMDocs( entry->khelpcenterSpecial(), mCurrentItem, QStringLiteral("browsercontrol") );
    } else if ( entry->khelpcenterSpecial() == QLatin1String("othercontrol") ) {
      mNavigator->insertKCMDocs( entry->khelpcenterSpecial(), mCurrentItem, QStringLiteral("othercontrol") );
    } else if ( entry->khelpcenterSpecial() == QLatin1String("kinfocenter") ) {
      mNavigator->insertKCMDocs( entry->khelpcenterSpecial(), mCurrentItem, QStringLiteral("kinfocenter") );
    } else if ( entry->khelpcenterSpecial() == QLatin1String("kioslave") ) {
      mNavigator->insertIOSlaveDocs( entry->khelpcenterSpecial(), mCurrentItem );
    } else if ( entry->khelpcenterSpecial() == QLatin1String("info") ) {
      mNavigator->insertInfoDocs( mCurrentItem );
    } else if ( entry->khelpcenterSpecial() == QLatin1String("scrollkeeper") ) {
      mNavigator->insertScrollKeeperDocs( mCurrentItem );
    } else {
      return;
    }
// TODO: was contents2 -> needs to be changed to help-contents-alternate or similar
    mCurrentItem->setIcon( 0, QIcon::fromTheme( QStringLiteral("help-contents") ) );
  }
}

DocEntryTraverser *PluginTraverser::createChild( DocEntry * /*entry*/ )
{
  if ( mCurrentItem ) {
    return new PluginTraverser( mNavigator, mCurrentItem );
  }
  qCWarning(KHC_LOG) << "ERROR! mCurrentItem is not set.";
  return nullptr;
}

// vim:ts=2:sw=2:et
