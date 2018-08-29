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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#ifndef KHC_PLUGINTRAVERSER_H
#define KHC_PLUGINTRAVERSER_H

#include "docentrytraverser.h"
#include <QtGlobal>
class QTreeWidget;

namespace KHC {
  
  class Navigator;
  class NavigatorItem;

  class PluginTraverser : public DocEntryTraverser
  {
    public:
      PluginTraverser( Navigator *navigator, QTreeWidget *parent );
      PluginTraverser( Navigator *navigator, NavigatorItem *parent );

      void process( DocEntry *entry ) override;

      DocEntryTraverser *createChild( DocEntry *entry ) override;

  private:
    QTreeWidget *mListView;
    NavigatorItem *mParentItem;
    NavigatorItem *mCurrentItem;

    Navigator *mNavigator;
  };
}

#endif // KHC_PLUGINTRAVERSER_H
// vim:ts=2:sw=2:et
