/*
 *  khc_mainwindow.h - part of the KDE Help Center
 *
 *  Copyright (c) 199 Matthias Elter (me@kde.org)
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

#ifndef __khc_mainwindow_h__
#define __khc_mainwindow_h__

#include "khelpcenter.h"

#include <opMainWindow.h>
#include <opMainWindowIf.h>

#include <qlist.h>

class QSplitter;
class OPFrame;
class QPopupMenu;
class KHelpNavigator;
class KHCBaseView;
class KFileBookmark;

class KHelpBrowser : public OPMainWindow
{
    Q_OBJECT

 public:
    KHelpBrowser(const QString& url = 0);
    virtual ~KHelpBrowser();

    int openURL(const char *URL, bool withHistory = true);

    unsigned long getListIndex() { return listIndex; }
    static KHelpBrowser *getHelpWindowAt(unsigned long id) 
	{ return helpWindowList.at(id); }

 protected:
    void cleanUp();
  
 private:
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void setupLocationBar();
    void setupView();

    void enableMenuItems();
    void enableNavigator(bool enable);

    void fillBookmarkMenu(KFileBookmark *parent, QPopupMenu *menu, int &id);

 public slots:

    // mainwindow related slots
    void slotNewBrowser();
    void slotOpenFile();

    void slotIntroduction();
    void slotForward();
    void slotBack();

    void slotSetBookmark();

    void slotToolbarClicked(int);
    
    void slotOptionsNavigator();
    void slotOptionsToolbar();
    void slotOptionsLocationbar();
    void slotOptionsStatusbar();
    void slotOptionsGeneral();

    void slotSaveSettings();
    void slotReadSettings();

    void slotQuit();

    // slots the views connect to
    void slotOpenNewBrowser(const QString& url);
    void slotURLSelected(const QString& _url, int _button);
    void slotSetTitle(const QString& _title );
    void slotSetLocation(const QString& _url);
    void slotSetURL(QString url);
    void slotSetStatusText(const QString& text);
    void slotEnableMenuItems();

    // forward actions to the views
    void slotFind();
    void slotFindNext();
    void slotReload();
    void slotCopy();
    void slotPrint();
    void slotMagMinus();
    void slotMagPlus();
    void slotStopProcessing();

 private slots:
    void slotLocationEntered();
    void slotHistoryFillBack();
    void slotHistoryFillForward();
    void slotHistoryBackActivated(int id);
    void slotHistoryForwardActivated(int id);

 private:
    QSplitter       *m_pSplitter;
    KHelpNavigator  *m_pNavigator;
    OPFrame         *m_pFrame;
    KHelpCenter::View_var m_vView;

    QPopupMenu *m_pFileMenu, *m_pEditMenu, *m_pViewMenu, *m_pGotoMenu, *m_pOptionsMenu, *m_pHelpMenu,*m_pBookmarkMenu;
    QPopupMenu *m_pHistoryBackMenu, *m_pHistoryForwardMenu;

    // file type
    enum FileType { HTMLFile, MANFile, INFOFile, TEXTFile, UNKNOWNFile };

    // toolbar and menu id's
    enum {TB_NAVIGATOR, TB_BACK, TB_FORWARD, TB_RELOAD, TB_STOP, TB_PRINT, TB_SETBOOKMARK, TB_ZOOMIN,
	  TB_ZOOMOUT, TB_FIND};
    int idCopy, idBack, idForward, idNavigator, idToolBar ,idLocationBar, idStatusBar, idMagPlus, idMagMinus;

    // UI options:
    bool m_showStatusBar, m_showToolBar, m_showLocationBar, m_showNavigator;
    
    // static list of KHelpBrowser windows
    static QList<KHelpBrowser> helpWindowList;

    // index of KHelpBrowser instance in helpWindowList
    unsigned long listIndex;
};

#endif
