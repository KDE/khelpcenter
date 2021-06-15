/*
    SPDX-FileCopyrightText: 1999 Matthias Elter <me@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KHC_SCROLLKEEPERTREEBUILDER_H
#define KHC_SCROLLKEEPERTREEBUILDER_H

#include <QObject>

#include "navigatoritem.h"

class QDomNode;

namespace KHC {

class ScrollKeeperTreeBuilder : public QObject
{
  Q_OBJECT
  public:
    explicit ScrollKeeperTreeBuilder( QObject *parent );

    void build( NavigatorItem *parent );

    void buildOrHide( NavigatorItem *parent );

  private:
    void loadConfig();
    int insertSection( NavigatorItem *parent,
                       const QDomNode &sectNode, NavigatorItem *&created );
    void insertDoc( NavigatorItem *parent, const QDomNode &docNode );

    bool mShowEmptyDirs;
    QString mContentsList;
    QList<NavigatorItem*> mItems;
};

}

#endif // KHC_SCROLLKEEPERTREEBUILDER_H
// vim:ts=2:sw=2:et
