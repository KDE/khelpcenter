/*
    SPDX-FileCopyrightText: 2002 Frerich Raabe <raabe@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
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
      explicit InfoTree( QObject *parent = nullptr);

      void build( NavigatorItem  *parentItem ) override;

    private:
      void parseInfoDirFile( const QString &fileName );

      NavigatorItem *m_parentItem = nullptr;
      NavigatorItem *m_alphabItem = nullptr;
      NavigatorItem *m_categoryItem = nullptr;
  };
}

#endif // KHC_INFOTREE_H
// vim:ts=2:sw=2:et
