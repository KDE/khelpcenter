/*
    SPDX-FileCopyrightText: 2002 Frerich Raabe <raabe@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KHC_HISTORY_H
#define KHC_HISTORY_H

#include <QUrl>
#include <QObject>
#include <QList>

class KActionCollection;
class KXmlGuiWindow;
class KToolBarPopupAction;
class QMenu;
class QAction;

namespace KHC {

class View;

class History : public QObject
{
    Q_OBJECT
  public:
    friend class foo; // to make gcc shut up
    struct Entry
    {
      Entry() {}

      View *view = nullptr;
      QUrl url;
      QString title;
      QByteArray buffer;
      bool search = false;
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
    KToolBarPopupAction *m_backAction = nullptr;
    KToolBarPopupAction *m_forwardAction = nullptr;
};

}

#endif // KHC_HISTORY_H
// vim:ts=2:sw=2:et
