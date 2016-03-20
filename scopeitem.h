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

#ifndef KHC_SCOPEITEM_H
#define KHC_SCOPEITEM_H

#include <QTreeWidgetItem>

#include "docmetainfo.h"

namespace KHC {

class ScopeItem : public QTreeWidgetItem
{
  public:
    ScopeItem( QTreeWidgetItem *parent, DocEntry *entry )
      : QTreeWidgetItem( parent, QStringList() << entry->name(), ScopeItemType ),
        mEntry( entry ) { init(); }

    DocEntry *entry()const { return mEntry; }

    enum { ScopeItemType = 734678 };

    void setOn( bool on ) { setCheckState( 0, on ? Qt::Checked : Qt::Unchecked ); }
    bool isOn() const { return checkState( 0 ) == Qt::Checked; }

  private:
    void init() { setCheckState(0, Qt::Checked); }

    DocEntry *mEntry;
};

}

#endif //KHC_SCOPEITEM_H
// vim:ts=2:sw=2:et
