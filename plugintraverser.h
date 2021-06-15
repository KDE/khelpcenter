/*
    SPDX-FileCopyrightText: 1999 Matthias Elter <me@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KHC_PLUGINTRAVERSER_H
#define KHC_PLUGINTRAVERSER_H

#include "docentrytraverser.h"
#include <QTreeWidget>

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
    QTreeWidget *mListView = nullptr;
    NavigatorItem *mParentItem = nullptr;
    NavigatorItem *mCurrentItem = nullptr;

    Navigator *mNavigator = nullptr;
  };
}

#endif // KHC_PLUGINTRAVERSER_H
// vim:ts=2:sw=2:et
