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

#ifndef __navigatoritem_h___
#define __navigatoritem_h___

#include <qlistview.h>

namespace KHC {

class TOC;

class NavigatorItem : public QListViewItem
{
  public:
    NavigatorItem (QListView *parent, QListViewItem *after);
    NavigatorItem (QListViewItem *parent, QListViewItem *after);
    NavigatorItem (QListView* parent, const QString& text,
                   const QString& icon = QString::null);
    NavigatorItem (QListViewItem* parent, const QString& text,
                   const QString& icon = QString::null);
    NavigatorItem (QListView* parent, QListViewItem* after,
                   const QString& text, const QString& icon = QString::null);
    NavigatorItem (QListViewItem* parent, QListViewItem* after,
                   const QString& text, const QString& icon = QString::null);

    ~NavigatorItem();

    void setName( const QString & );
    void setUrl( const QString & );
    void setInfo( const QString & );
    void setIcon( const QString & );

    QString name() { return mName; }
    virtual QString url() { return mUrl; }
    QString info() { return mInfo; }
    QString icon() { return mIcon; }

    TOC *toc() { return mToc; }

    TOC *createTOC();

 private:
    void init();
    void init(const QString &text, const QString &icon);
    
    QString mName;
    QString mUrl;
    QString mInfo;
    QString mIcon;
    
    TOC *mToc;
};

}

#endif
// vim:ts=2:sw=2:et
