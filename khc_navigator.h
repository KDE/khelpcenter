/*
 *  khc_navigator.h - part of the KDE Help Center
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
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __khc_navigator_h__
#define __khc_navigator_h__

#include <qlist.h>

#include <ktreelist.h>

class IndexWidget;
class SearchWidget;
class khcNavigatorItem;
class QTabBar;

class khcNavigator : public QWidget
{
    Q_OBJECT
  
 public:
    khcNavigator(QWidget *parent=0, const char *name=0);
    virtual ~khcNavigator();

 protected:
    void resizeEvent (QResizeEvent *);

 public slots:
    void slotURLSelected(QString url);
    void slotItemSelected(int index);
    void slotReloadTree();
    void slotTabSelected(int);

 signals:
    void itemSelected(QString itemURL);
    void setBussy(bool bussy);
	
 private:
    void setupContentsTab();
    void setupIndexTab();
    void setupSearchTab();
    void buildTree();
    void clearTree();

    void buildManSubTree(khcNavigatorItem *parent);
    void buildManualSubTree(khcNavigatorItem *parent);
    void insertPlugins();

    bool appendEntries (const char *dirName,  khcNavigatorItem *parent, QList<khcNavigatorItem> *appendList);
    bool processDir(const char *dirName, khcNavigatorItem *parent,  QList<khcNavigatorItem> *appendList);
    bool containsDocuments(QString dir);

    QTabBar *tabBar;
    KTreeList *tree;
    SearchWidget *search;
    IndexWidget *index;

    QList<khcNavigatorItem> staticItems, manualItems, pluginItems;
};

#endif
