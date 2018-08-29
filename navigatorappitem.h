/*
 *  navigatorappitem.h - part of the KDE Help Center
 *
 *  Copyright (C) 1999 Matthias Elter (me@kde.org)
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License version 2 or at your option version 3 as published
 *  by the Free Software Foundation.
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

    TOC *mToc;
};

}

#endif //KHC_NAVIGATORAPPITEM_H
// vim:ts=2:sw=2:et
