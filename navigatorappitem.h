/*
    SPDX-FileCopyrightText: 1999 Matthias Elter <me@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only
*/

#ifndef KHC_NAVIGATORAPPITEM_H
#define KHC_NAVIGATORAPPITEM_H

#include "navigatoritem.h"

namespace KHC {

class TOC;

class NavigatorAppItem : public NavigatorItem
{
  public:
    NavigatorAppItem( DocEntry *entry, QTreeWidgetItem *parent );

    NavigatorAppItem( DocEntry *entry, QTreeWidget *parent,
                      QTreeWidgetItem *after );
    NavigatorAppItem( DocEntry *entry, QTreeWidgetItem *parent,
                      QTreeWidgetItem *after );

    ~NavigatorAppItem();

    void itemExpanded( bool open ) override;

  private:
    void scheduleTOCBuild();

    TOC *mToc = nullptr;
    
    static QMap< QString,NavigatorAppItem* > s_menuItemsMap;
    
};

}

#endif //KHC_NAVIGATORAPPITEM_H
// vim:ts=2:sw=2:et
