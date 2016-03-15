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

#include "navigatoritem.h"

#include "docentry.h"
#include "khc_debug.h"

#include <QIcon>

using namespace KHC;

NavigatorItem::NavigatorItem( DocEntry *entry, QTreeWidget *parent,
                              QTreeWidgetItem *after )
  : QTreeWidgetItem( parent, after )
{
  init( entry );
}

NavigatorItem::NavigatorItem( DocEntry *entry, QTreeWidgetItem *parent,
                              QTreeWidgetItem *after )
  : QTreeWidgetItem( parent, after )
{
  init( entry );
}

NavigatorItem::NavigatorItem( DocEntry *entry, QTreeWidget *parent )
  : QTreeWidgetItem( parent )
{
    init( entry );
}

NavigatorItem::NavigatorItem( DocEntry *entry, QTreeWidgetItem *parent )
  : QTreeWidgetItem( parent )
{
  init( entry );
}

NavigatorItem::~NavigatorItem()
{
  if ( mAutoDeleteDocEntry ) delete mEntry;
}

void NavigatorItem::init( DocEntry *entry )
{
  mEntry = entry;
  mAutoDeleteDocEntry = false;

  updateItem();
}

DocEntry *NavigatorItem::entry() const
{
  return mEntry;
}

void NavigatorItem::setAutoDeleteDocEntry( bool enabled )
{
  mAutoDeleteDocEntry = enabled;
}

void NavigatorItem::updateItem()
{
  setText( 0, entry()->name() );
  setIcon( 0, QIcon::fromTheme( entry()->icon() ) );
}

void NavigatorItem::itemExpanded( bool open )
{
  Q_UNUSED( open );
}

// vim:ts=2:sw=2:et
