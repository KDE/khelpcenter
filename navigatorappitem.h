/*
 *  navigatorappitem.h - part of the KDE Help Center
 *
 *  Copyright (C) 2001 Waldo Bastian <bastian@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
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

#ifndef __navigatorappitem_h___
#define __navigatorappitem_h___

#include "navigatoritem.h"

class KService;

namespace KHC {

class NavigatorAppItem : public NavigatorItem
{
 public:
    NavigatorAppItem (QListView *parent, QListViewItem *after);
    NavigatorAppItem (QListViewItem *parent, QListViewItem *after);
    NavigatorAppItem (QListView* parent, const QString& text, const QString& miniicon, const QString& relpath);
    NavigatorAppItem (QListViewItem* parent, const QString& text, const QString& miniicon, const QString& relpath);

    virtual void setOpen(bool);

 protected:
    QString documentationURL(KService *s);

    QString relpath;
};

}

#endif
// vim:ts=2:sw=2:et
