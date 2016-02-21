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

#include <QUrl>
#include <QTimer>
#include <QProcess>

class QFrame;
class QProgressBar;
class QPushButton;
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
class Formatter;

class Navigator : public QWidget
{
    Q_OBJECT
  public:
    explicit Navigator(View *, QWidget *parent=0, const char *name=0);
    virtual ~Navigator();

    QUrl homeURL();

    SearchEngine *searchEngine() const;
    Formatter *formatter() const;

    const GlossaryEntry &glossEntry(const QString &term) const { return mGlossaryTree->entry( term ); }

    void insertParentAppDocs( const QString &name, NavigatorItem *parent );
    NavigatorItem *insertScrollKeeperDocs( NavigatorItem *parentItem,
                                 NavigatorItem *after );
    void insertInfoDocs( NavigatorItem *parentItem );
    void insertKCMDocs(const QString &, NavigatorItem*parent, const QString &);
    void insertIOSlaveDocs(const QString &, NavigatorItem*parent);
    
    void createItemFromDesktopFile( NavigatorItem *item, const QString &name );

    bool showMissingDocs() const;

    void clearSelection();

    void showOverview( NavigatorItem *item, const QUrl &url );

    void readConfig();
    void writeConfig();

  public Q_SLOTS:
    void openInternalUrl( const QUrl &url );
    void slotItemSelected(QTreeWidgetItem* index);
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
    void checkSearchButton();

    void clearSearch();

    void slotDelayedIndexingStart();
    void slotDoIndexWork();
    void slotProcessExited( int exitCode, QProcess::ExitStatus exitStatus );
    void slotShowIndexingProgressBar();

  protected:
    QString createChildrenList( QTreeWidgetItem *child );

  private:
    void setupContentsTab();
    void setupIndexTab();
    void setupSearchTab();
    void setupGlossaryTab();

    void insertPlugins();
    void hideSearch();

    QTreeWidget *mContentsTree;
    Glossary *mGlossaryTree;

    SearchWidget *mSearchWidget;

    QTabWidget *mTabWidget;

    QFrame *mSearchFrame;
    KLineEdit *mSearchEdit;
    QPushButton *mSearchButton;

    bool mShowMissingDocs;
    
    SearchEngine *mSearchEngine;

    View *mView;

    QUrl mHomeUrl;
    
    bool mSelected;

    QUrl mLastUrl;

    int mDirLevel;

    DocEntry::List mIndexingQueue;
    KProcess *mIndexingProc;
    QProgressBar *mIndexingBar;
    QTimer mIndexingTimer;
};

}

#endif //KHC_NAVIGATOR_H
// vim:ts=2:sw=2:et
