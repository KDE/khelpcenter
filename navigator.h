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
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef __navigator_h__
#define __navigator_h__

#include <qptrlist.h>
#include <qtabwidget.h>
#include <qlistview.h>
#include <qdict.h>
#include <klistview.h>

#include "glossary.h"

#include <regex.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qtimer.h>

class QPushButton;

class KListView;
class KService;
class KProcess;
class KProcIO;

class SearchWidget;
class SearchEngine;

namespace KHC {

class NavigatorItem;
class Navigator;
class View;

class Navigator : public QWidget
{
    Q_OBJECT
  public:
    Navigator(View *, QWidget *parent=0, const char *name=0);
    virtual ~Navigator();

    const GlossaryEntry &glossEntry(const QString &term) const { return mGlossaryTree->entry( term ); }

    void insertParentAppDocs( const QString &name, NavigatorItem *parent );
    void insertAppletDocs( NavigatorItem *parent );
    NavigatorItem *insertScrollKeeperDocs( NavigatorItem *parentItem,
                                 NavigatorItem *after );
    void insertInfoDocs( NavigatorItem *parentItem );
    void insertIOSlaveDocs(const QString &, NavigatorItem*parent);
    
    void createItemFromDesktopFile( NavigatorItem *item, const QString &name );

    bool showMissingDocs() const;

    void selectItem( const KURL &url );
    void clearSelection();

  public slots:
    void slotURLSelected(QString url);
    void slotItemSelected(QListViewItem* index);

    void slotSearch();

    void slotShowSearchResult( const QString & );

    void showPreferencesDialog();

  signals:
    void itemSelected(const QString& itemURL);
    void glossSelected(const GlossaryEntry &entry);

  protected slots:
    void slotSearchFinished();
    void slotSearchTextChanged( const QString & );
    void slotTabChanged( QWidget * );

    bool checkSearchIndex();

  private slots:
    void getScrollKeeperContentsList(KProcIO *proc);

  private:
    void setupContentsTab();
    void setupIndexTab();
    void setupSearchTab();
    void setupGlossaryTab();

    void insertPlugins();
    int insertScrollKeeperSection( NavigatorItem *parentItem,
                                   NavigatorItem *after, QDomNode sectNode,
                                   NavigatorItem * &created );
    void insertScrollKeeperDoc(NavigatorItem *parentItem,QDomNode docNode);

    void hideSearch();

    KListView *mContentsTree;
    Glossary *mGlossaryTree;

    SearchWidget *mSearchWidget;

    QTabWidget *mTabWidget;

    QFrame *mSearchFrame;
    QLineEdit *mSearchEdit;
    QPushButton *mSearchButton;

    QPtrList<NavigatorItem> manualItems, pluginItems, scrollKeeperItems;

    bool mScrollKeeperShowEmptyDirs;

    QString mScrollKeeperContentsList;
    
    bool mShowMissingDocs;
    
    SearchEngine *mSearchEngine;

    View *mView;
};

}

#endif
// vim:ts=2:sw=2:et
