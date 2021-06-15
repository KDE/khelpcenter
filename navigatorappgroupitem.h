/*
    SPDX-FileCopyrightText: 2001 Waldo Bastian <bastian@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only
*/

#ifndef KHC_NAVIGATORAPPGROUPITEM_H
#define KHC_NAVIGATORAPPGROUPITEM_H

#include "navigatoritem.h"

class KService;

namespace KHC {

class NavigatorAppGroupItem : public NavigatorItem
{
  public:
    NavigatorAppGroupItem( DocEntry *entry, QTreeWidget *parent,
                           const QString &relPath );
    NavigatorAppGroupItem( DocEntry *entry, QTreeWidgetItem *parent,
                           const QString &relPath );

    NavigatorAppGroupItem( DocEntry *entry, QTreeWidget *parent,
                           QTreeWidgetItem *after );
    NavigatorAppGroupItem( DocEntry *entry, QTreeWidgetItem *parent,
                           QTreeWidgetItem *after );

    void setRelpath( const QString & );

    void itemExpanded(bool) override;
    void populate( bool recursive = false );

  protected:
    static QString documentationURL( const KService *s );

  private:
    QString mRelpath;
    bool mPopulated = false;
};

}

#endif //KHC_NAVIGATORAPPGROUPITEM_H
// vim:ts=2:sw=2:et
