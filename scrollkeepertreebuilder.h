/*
 *  This file is part of the KDE Help Center
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
 *  Foundation, Inc., 51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#ifndef KHC_SCROLLKEEPERTREEBUILDER_H
#define KHC_SCROLLKEEPERTREEBUILDER_H

#include <qobject.h>
#include <q3ptrlist.h>

#include "navigatoritem.h"

class KProcIO;

class QDomNode;

namespace KHC {

class ScrollKeeperTreeBuilder : public QObject
{
  Q_OBJECT
  public:
    ScrollKeeperTreeBuilder( QObject *parent, const char *name = 0 );

    NavigatorItem *build( NavigatorItem *parent, NavigatorItem *after );

  private slots:
    void getContentsList( KProcIO *proc );

  private:
    void loadConfig();
    int insertSection( NavigatorItem *parent, NavigatorItem *after,
                       const QDomNode &sectNode, NavigatorItem *&created );
    void insertDoc( NavigatorItem *parent, const QDomNode &docNode );

    bool mShowEmptyDirs;
    QString mContentsList;
    Q3PtrList<NavigatorItem> mItems;
};

}

#endif // KHC_SCROLLKEEPERTREEBUILDER_H
// vim:ts=2:sw=2:et
