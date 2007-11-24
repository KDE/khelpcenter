/* This file is part of the KDE project
   Copyright 2002 Cornelius Schumacher <schumacher@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License version 2 or at your option version 3 as published
   by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef SCOPEITEM_H
#define SCOPEITEM_H

#include <Qt3Support/Q3CheckListItem>

#include "docmetainfo.h"

namespace KHC {

class ScopeItem : public Q3CheckListItem
{
  public:
    ScopeItem( Q3ListView *parent, DocEntry *entry )
      : Q3CheckListItem( parent, entry->name(), Q3CheckListItem::CheckBox ),
        mEntry( entry ), mObserver( 0 ) {}

    ScopeItem( Q3ListViewItem *parent, DocEntry *entry )
      : Q3CheckListItem( parent, entry->name(), Q3CheckListItem::CheckBox ),
        mEntry( entry ), mObserver( 0 ) {}

    DocEntry *entry()const { return mEntry; }

    int rtti() const { return rttiId(); }

    static int rttiId() { return 734678; }

    class Observer
    {
      public:
        virtual ~Observer() {}
        virtual void scopeItemChanged( ScopeItem * ) = 0;
    };

    void setObserver( Observer *o ) { mObserver = o; }

  protected:
    void stateChange ( bool )
    {
      if ( mObserver ) mObserver->scopeItemChanged( this );
    }

  private:
    DocEntry *mEntry;

    Observer *mObserver;
};

}

#endif
// vim:ts=2:sw=2:et
