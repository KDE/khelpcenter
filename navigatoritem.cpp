/*
    SPDX-FileCopyrightText: 1999 Matthias Elter <me@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
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
