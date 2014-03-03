/*
 *  This file is part of the KDE Help Center
 *
 *  Copyright (C) 2002 Frerich Raabe <raabe@kde.org>
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
#ifndef KHC_HISTORY_H
#define KHC_HISTORY_H

#include <QUrl>

#include <QObject>

#include <QMenu>
#include <QList>

class KActionCollection;
class KXmlGuiWindow;
class KToolBarPopupAction;
class QMenu;

namespace KHC {

class View;

class History : public QObject
{
    Q_OBJECT
  public:
    friend class foo; // to make gcc shut up
    struct Entry
    {
      Entry() : view( 0 ), search( false ) {}

      View *view;
      QUrl url;
      QString title;
      QByteArray buffer;
      bool search;
    };

    static History &self();

    void setupActions( KActionCollection *coll );
    void updateActions();

    void installMenuBarHook( KXmlGuiWindow *mainWindow );

    void createEntry();
    void updateCurrentEntry( KHC::View *view );

  Q_SIGNALS:
    void goInternalUrl( const QUrl & );
    void goUrl( const QUrl & );

  private Q_SLOTS:
    void backActivated( QAction *action );
    void fillBackMenu();
    void forwardActivated( QAction *action );
    void fillForwardMenu();
    void goMenuActivated( QAction* action );
    void fillGoMenu();
    void back();
    void forward();
    void goHistoryActivated( int steps );
    void goHistory( int steps );
    void goHistoryDelayed();

  private:
    History();
    History( const History &rhs );
    History &operator=( const History &rhs );
    ~History();
    
    typedef QList<Entry*> EntryList;

    bool canGoBack() const;
    bool canGoForward() const;
    void fillHistoryPopup( QMenu *, bool, bool, bool, uint = 0 );
    
    /**
     *  dumps the history with a kDebug and mark wihch one is the current one
     *  This is a debugging function.
     */
    void dumpHistory() const;

    static History *m_instance;

    EntryList m_entries;
    EntryList::Iterator m_entries_current;


    int m_goBuffer;
    int m_goMenuIndex;
    int m_goMenuHistoryStartPos;
    int m_goMenuHistoryCurrentPos;
  public:
    KToolBarPopupAction *m_backAction;
    KToolBarPopupAction *m_forwardAction;
};

}

#endif // KHC_HISTORY_H
// vim:ts=2:sw=2:et
