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
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __navigator_h__
#define __navigator_h__

#include <qptrlist.h>
#include <qtabwidget.h>
#include <qlistview.h>
#include <qdict.h>
#include <klistview.h>

#include "glossary.h"
#include "toc.h"

#include <regex.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qtimer.h>
#include <map>

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
class InfoNode;
class InfoHierarchyMaker;
class View;

class Navigator : public QWidget
{
    Q_OBJECT
  public:
    Navigator(View *, QWidget *parent=0, const char *name=0);
    virtual ~Navigator();

    const GlossaryEntry &glossEntry(const QString &term) const { return glossaryTree->entry( term ); }

    void buildInfoSubTree(NavigatorItem *parent);

  public slots:
    void slotURLSelected(QString url);
    void slotItemSelected(QListViewItem* index);
    void slotItemExpanded(QListViewItem* index);
    void slotInfoHierarchyCreated(uint key, uint nErrorCode, const InfoNode* pRootNode);
    void slotCleanHierarchyMakers();

    void slotSearch();

    void slotShowSearchResult( const QString & );

  signals:
    void itemSelected(const QString& itemURL);
    void glossSelected(const GlossaryEntry &entry);

  protected slots:
    void slotSearchFinished();
    void slotSearchTextChanged( const QString & );

  private slots:
    void getScrollKeeperContentsList(KProcIO *proc);
    /* Cog-wheel animation handling -- enable after creating the icons
    void slotAnimation();
    */

  private:
    void setupContentsTab();
    void setupIndexTab();
    void setupSearchTab();
    void setupGlossaryTab();
    void setupTOCTab();
    void buildTree();
    void clearTree();

    QString findInfoDirFile();
    bool readInfoDirFile(QString& sFileContents);
    bool parseInfoSubjectLine(QString sLine, QString& sItemTitle, QString& sItemURL);
    void addChildren(const InfoNode* pParentNode, NavigatorItem* pParentTreeItem);
    NavigatorItem *addInfoNode( NavigatorItem *parent,
                                   NavigatorItem *next,
                                   const QString &line );

    void buildManSubTree(NavigatorItem *parent);

    void insertPlugins();
    void insertScrollKeeperItems();
    int insertScrollKeeperSection(NavigatorItem *parentItem,QDomNode sectNode);
    void insertScrollKeeperDoc(NavigatorItem *parentItem,QDomNode docNode);

    void hideSearch();

    KListView *contentsTree;
    Glossary *glossaryTree;
    TOC *tocTree;

    SearchWidget *mSearchWidget;

    QTabWidget *mTabWidget;

    QFrame *mSearchFrame;
    QLineEdit *mSearchEdit;
    QPushButton *mSearchButton;

    QPtrList<NavigatorItem> manualItems, pluginItems, scrollKeeperItems;

    regex_t compInfoRegEx;
    std::map<NavigatorItem*, InfoHierarchyMaker*> hierarchyMakers;
    QTimer cleaningTimer;

    /* Cog-wheel animation handling -- enable after creating the icons
    struct AnimationInfo
    {
        AnimationInfo( const char * _iconBaseName, uint _iconCount, const QPixmap & _originalPixmap )
            : iconBaseName(_iconBaseName), iconCount(_iconCount), iconNumber(1), originalPixmap(_originalPixmap) {}
        AnimationInfo() : iconCount(0) {}
        QCString iconBaseName;
        uint iconCount;
        uint iconNumber;
        QPixmap originalPixmap;
    };
    typedef QMap<NavigatorItem *, AnimationInfo> MapCurrentOpeningSubjects;
    MapCurrentOpeningSubjects m_mapCurrentOpeningSubjects;

    QTimer *m_animationTimer;

    void startAnimation(NavigatorItem * item, const char * iconBaseName = "kde", uint iconCount = 6);
    void stopAnimation(NavigatorItem * item);
    */

    bool mScrollKeeperShowEmptyDirs;
    QString mScrollKeeperContentsList;
    
    SearchEngine *mSearchEngine;

    View *mView;
};

}

#endif
