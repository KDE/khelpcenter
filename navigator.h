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

#ifndef KHC_NAVIGATOR_H
#define KHC_NAVIGATOR_H

#include "glossary.h"
#include "docentry.h"

#include <QProcess>
#include <QTimer>
#include <QUrl>

class QProgressBar;
class QTabWidget;
class QTreeWidget;
class QTreeWidgetItem;
class KLineEdit;
class KProcess;

namespace KHC {

class NavigatorItem;
class Navigator;
class View;
class SearchEngine;
class SearchWidget;

class Navigator : public QWidget
{
    Q_OBJECT
  public:
    explicit Navigator(View *, QWidget *parent= nullptr);
    virtual ~Navigator();

    QUrl homeURL();

    SearchEngine *searchEngine() const;

    const GlossaryEntry &glossEntry(const QString &term) const { return mGlossaryTree->entry( term ); }

    void insertParentAppDocs( const QString &name, NavigatorItem *parent );
    void insertScrollKeeperDocs( NavigatorItem *parent );
    void insertInfoDocs( NavigatorItem *parentItem );
    void insertKCMDocs(const QString &, NavigatorItem*parent, const QString &);
    void insertIOSlaveDocs(const QString &, NavigatorItem*parent);
    
    void createItemFromDesktopFile( NavigatorItem *item, const QString &name );

    void clearSelection();

    void showOverview( NavigatorItem *item, const QUrl &url );

    void readConfig();
    void writeConfig();

  public Q_SLOTS:
    void openInternalUrl( const QUrl &url );
    void slotItemSelected(QTreeWidgetItem* index);
    void slotItemExpanded( QTreeWidgetItem *item );
    void slotItemCollapsed( QTreeWidgetItem *item );
    void slotSearch();
    void slotShowSearchResult( const QString & );
    void slotSelectGlossEntry( const QString &id );
    void selectItem( const QUrl &url );

  Q_SIGNALS:
    void itemSelected(const QString& itemURL);
    void glossSelected(const GlossaryEntry &entry);
    void setStatusBarText(const QString &text);

  protected Q_SLOTS:
    void slotSearchFinished();
    void checkSearchEdit();

    void clearSearch();

    void slotDelayedIndexingStart();
    void slotDoIndexWork();
    void slotProcessExited( int exitCode, QProcess::ExitStatus exitStatus );
    void slotShowIndexingProgressBar();

  protected:
    static QString createChildrenList( QTreeWidgetItem *child, int level );

  private:
    void setupContentsTab();
    void setupIndexTab();
    void setupSearchTab();
    void setupGlossaryTab();

    void insertPlugins();
    void hideSearch();

    QTreeWidget *mContentsTree = nullptr;
    Glossary *mGlossaryTree = nullptr;

    SearchWidget *mSearchWidget = nullptr;

    QTabWidget *mTabWidget = nullptr;

    KLineEdit *mSearchEdit = nullptr;

    SearchEngine *mSearchEngine = nullptr;

    View *mView = nullptr;

    QUrl mHomeUrl;
    
    bool mSelected = false;

    DocEntry::List mIndexingQueue;
    KProcess *mIndexingProc = nullptr;
    QProgressBar *mIndexingBar = nullptr;
    QTimer mIndexingTimer;
};

}

#endif //KHC_NAVIGATOR_H
// vim:ts=2:sw=2:et
