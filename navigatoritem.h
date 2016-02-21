/*
 *  navigatoritem.h - part of the KDE Help Center
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
#ifndef KHC_NAVIGATORITEM_H
#define KHC_NAVIGATORITEM_H

#include <QTreeWidgetItem>

namespace KHC {

class TOC;
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

    TOC *toc() const { return mToc; }
  
    void setExpanded( bool open );

  private:
    void init( DocEntry * );
    void scheduleTOCBuild();
    
    TOC *mToc;

    DocEntry *mEntry;
    bool mAutoDeleteDocEntry;
};

}

#endif // KHC_NAVIGATORITEM_H

// vim:ts=2:sw=2:et
