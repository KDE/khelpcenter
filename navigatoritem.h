/*
    SPDX-FileCopyrightText: 1999 Matthias Elter <me@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KHC_NAVIGATORITEM_H
#define KHC_NAVIGATORITEM_H

#include <QTreeWidgetItem>

namespace KHC {

class DocEntry;

class NavigatorItem : public QTreeWidgetItem
{
  public:
    NavigatorItem( DocEntry *entry, QTreeWidget *parent );
    NavigatorItem( DocEntry *entry, QTreeWidgetItem *parent );

    NavigatorItem( DocEntry *entry, QTreeWidget *parent,
                   QTreeWidgetItem *after );
    NavigatorItem( DocEntry *entry, QTreeWidgetItem *parent,
                   QTreeWidgetItem *after );

    ~NavigatorItem();

    DocEntry *entry() const;

    void setAutoDeleteDocEntry( bool );

    void updateItem();

    virtual void itemExpanded( bool open );

  private:
    void init( DocEntry * );

    DocEntry *mEntry = nullptr;
    bool mAutoDeleteDocEntry;
};

}

#endif // KHC_NAVIGATORITEM_H

// vim:ts=2:sw=2:et
