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

#include <QTreeWidgetItem>

#include "docmetainfo.h"

namespace KHC {

class ScopeItem : public QTreeWidgetItem
{
  public:
    ScopeItem( QTreeWidget *parent, DocEntry *entry )
        : QTreeWidgetItem( parent, QStringList() << entry->name(), rttiId() ),
          mEntry( entry ), mObserver( 0 ) { init(); }

    ScopeItem( QTreeWidgetItem *parent, DocEntry *entry )
      : QTreeWidgetItem( parent, QStringList() << entry->name(), rttiId() ),
        mEntry( entry ), mObserver( 0 ) { init(); }

    DocEntry *entry()const { return mEntry; }

    static int rttiId() { return 734678; }

    class Observer
    {
      public:
        virtual ~Observer() {}
        virtual void scopeItemChanged( ScopeItem * ) = 0;
    };

    void setObserver( Observer *o ) { mObserver = o; }

    void setOn( bool on ) { setCheckState( 0, on ? Qt::Checked : Qt::Unchecked ); }
    bool isOn() const { return checkState( 0 ) == Qt::Checked; }

  protected:
    void stateChange ( bool )
    {
      if ( mObserver ) mObserver->scopeItemChanged( this );
    }

  private:
    void init() { setCheckState(0, Qt::Checked); }

    DocEntry *mEntry;

    Observer *mObserver;
};

}

#endif
// vim:ts=2:sw=2:et
