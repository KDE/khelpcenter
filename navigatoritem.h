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
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#ifndef KHC_NAVIGATORITEM_H
#define KHC_NAVIGATORITEM_H

#include <qlistview.h>

namespace KHC {

class TOC;
class DocEntry;

class NavigatorItem : public QListViewItem
{
  public:
    NavigatorItem( DocEntry *entry, QListView *parent );
    NavigatorItem( DocEntry *entry, QListViewItem *parent );

    NavigatorItem( DocEntry *entry, QListView *parent,
                   QListViewItem *after );
    NavigatorItem( DocEntry *entry, QListViewItem *parent,
                   QListViewItem *after );

    ~NavigatorItem();

    DocEntry *entry() const;

    void setAutoDeleteDocEntry( bool );

    void updateItem();

    TOC *toc() const { return mToc; }

    TOC *createTOC();

  private:
    void init( DocEntry * );
    
    TOC *mToc;

    DocEntry *mEntry;
    bool mAutoDeleteDocEntry;
};

}

#endif

// vim:ts=2:sw=2:et
