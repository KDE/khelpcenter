/*
 *  infotree.h - part of the KDE Help Center
 *
 *  Copyright (C) 2002 Frerich Raabe (raabe@kde.org)
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
#ifndef KHC_INFOTREE_H
#define KHC_INFOTREE_H

#include "treebuilder.h"

namespace KHC
{
  class NavigatorItem;
  class InfoTree : public TreeBuilder
  {
    Q_OBJECT
    public:
      InfoTree( QObject *parent );

      virtual void build( NavigatorItem  *parentItem );

    private:
      void parseInfoDirFile( const QString &fileName );

      NavigatorItem *m_parentItem;
      NavigatorItem *m_alphabItem;
      NavigatorItem *m_categoryItem;
  };
}

#endif // KHC_INFOTREE_H
// vim:ts=2:sw=2:et
