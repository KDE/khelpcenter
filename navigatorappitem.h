/*
 *  navigatorappitem.h - part of the KDE Help Center
 *
 *  Copyright (C) 2001 Waldo Bastian <bastian@kde.org>
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

#ifndef __navigatorappitem_h___
#define __navigatorappitem_h___

#include "navigatoritem.h"

class KService;

namespace KHC {

class NavigatorAppItem : public NavigatorItem
{
  public:
    NavigatorAppItem( DocEntry *entry, Q3ListView *parent,
                      const QString &relPath );
    NavigatorAppItem( DocEntry *entry, Q3ListViewItem *parent,
                      const QString &relPath );

    NavigatorAppItem( DocEntry *entry, Q3ListView *parent,
                      Q3ListViewItem *after );
    NavigatorAppItem( DocEntry *entry, Q3ListViewItem *parent,
                      Q3ListViewItem *after );

    void setRelpath( const QString & );

    virtual void setOpen(bool);
    void populate( bool recursive = false );

  protected:
    QString documentationURL( const KService *s );

  private:
    QString mRelpath;
    bool mPopulated;
};

}

#endif
// vim:ts=2:sw=2:et
