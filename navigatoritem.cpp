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

#include "navigatoritem.h"

#include "toc.h"
#include "docentry.h"

#include <kdebug.h>
#include <kiconloader.h>

using namespace KHC;

NavigatorItem::NavigatorItem( DocEntry *entry, QListView *parent,
                              QListViewItem *after )
  : QListViewItem( parent, after )
{
  init( entry );
}

NavigatorItem::NavigatorItem( DocEntry *entry, QListViewItem *parent,
                              QListViewItem *after )
  : QListViewItem( parent, after )
{
  init( entry );
}

NavigatorItem::NavigatorItem( DocEntry *entry, QListView *parent )
  : QListViewItem( parent )
{
    init( entry );
}

NavigatorItem::NavigatorItem( DocEntry *entry, QListViewItem *parent )
  : QListViewItem( parent )
{
  init( entry );
}

NavigatorItem::~NavigatorItem()
{
  delete mToc;

  if ( mAutoDeleteDocEntry ) delete mEntry;
}

void NavigatorItem::init( DocEntry *entry )
{
  mEntry = entry;
  mAutoDeleteDocEntry = false;
  mToc = 0;

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
  setPixmap( 0, SmallIcon( entry()->icon() ) );
}

TOC *NavigatorItem::createTOC()
{
    mToc = new TOC( this );
    return mToc;
}

void NavigatorItem::setOpen( bool open )
{
  QListViewItem::setOpen( open );

  if ( entry()->icon().isEmpty() || entry()->icon() == "contents2" ) {
    if ( open && childCount() > 0 ) setPixmap( 0, SmallIcon( "contents" ) );
    else setPixmap( 0, SmallIcon( "contents2" ) );
  }
}

// vim:ts=2:sw=2:et
