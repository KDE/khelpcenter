/*
    SPDX-FileCopyrightText: 2002 Cornelius Schumacher <schumacher@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only
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

    DocEntry *mEntry = nullptr;
};

}

#endif //KHC_SCOPEITEM_H
// vim:ts=2:sw=2:et
