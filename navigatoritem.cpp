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

#include "toc.h"
#include "docentry.h"

#include <kdebug.h>
#include <kiconloader.h>
#include <QPixmap>
using namespace KHC;

NavigatorItem::NavigatorItem( DocEntry *entry, Q3ListView *parent,
                              Q3ListViewItem *after )
  : Q3ListViewItem( parent, after )
{
  init( entry );
}

NavigatorItem::NavigatorItem( DocEntry *entry, Q3ListViewItem *parent,
                              Q3ListViewItem *after )
  : Q3ListViewItem( parent, after )
{
  init( entry );
}

NavigatorItem::NavigatorItem( DocEntry *entry, Q3ListView *parent )
  : Q3ListViewItem( parent )
{
    init( entry );
}

NavigatorItem::NavigatorItem( DocEntry *entry, Q3ListViewItem *parent )
  : Q3ListViewItem( parent )
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
  Q3ListViewItem::setOpen( open );

// TODO: was contents2 -> needs to be changed to help-contents-alternate or similar
  if ( entry()->icon().isEmpty() || entry()->icon() == "help-contents" ) {
    if ( open && childCount() > 0 ) setPixmap( 0, SmallIcon( "help-contents" ) );
// TODO: was contents2 -> needs to be changed to help-contents-alternate or similar
    else setPixmap( 0, SmallIcon( "help-contents" ) );
  }
}

// vim:ts=2:sw=2:et
