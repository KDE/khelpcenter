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
#include <kparts/browserextension.h>
#include <kparts/part.h>
#include <qtabwidget.h>
#include <qlistview.h>

class SearchWidget;
class khcNavigatorItem;
class khcNavigator;
class KListView;
class KService;

class SectionItem : public QListViewItem
{
	public:
		SectionItem(QListViewItem *, const QString &);

		virtual void setOpen(const bool);
};

class khcNavigatorExtension : public KParts::BrowserExtension
{
    Q_OBJECT
 public:
    khcNavigatorExtension(KParts::ReadOnlyPart *part, const char *name=0) :
      KParts::BrowserExtension( part, name ) {}
    virtual ~khcNavigatorExtension() {}

 public slots:
    void slotItemSelected(const QString&);
};

class khcNavigator : public KParts::ReadOnlyPart
{
    Q_OBJECT

 public:
    khcNavigator(QWidget *parentWidget, QObject *widget, const char *name=0);
    virtual ~khcNavigator();

    virtual bool openURL( const KURL &url );

 protected:
    bool openFile();
    khcNavigatorExtension * m_extension;

};

class khcNavigatorWidget : public QTabWidget
{
    Q_OBJECT

 public:
    khcNavigatorWidget(QWidget *parent=0, const char *name=0);
    virtual ~khcNavigatorWidget();

 public slots:
    void slotURLSelected(QString url);
    void slotItemSelected(QListViewItem* index);
    void slotGlossaryItemSelected(QListViewItem* item);
    void slotReloadTree();

 signals:
    void itemSelected(const QString& itemURL);
    void glossSelected(const QString& entry);
    void setBussy(bool bussy);

 private:
    void setupContentsTab();
    void setupIndexTab();
    void setupSearchTab();
    void setupGlossaryTab();
    void buildTree();
    void clearTree();

    void buildManSubTree(khcNavigatorItem *parent);

    void buildManualSubTree(khcNavigatorItem *parent, QString relPath);
    QString documentationURL(KService *s);

    void insertPlugins();

    bool appendEntries (const QString &dirName,  khcNavigatorItem *parent, QList<khcNavigatorItem> *appendList);
    bool processDir(const QString &dirName, khcNavigatorItem *parent,  QList<khcNavigatorItem> *appendList);

    KListView *contentsTree, *glossaryTree;
    SearchWidget *search;

    QList<khcNavigatorItem> staticItems, manualItems, pluginItems;
};

#endif
