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
#include "khc_history.h"

#include <opMainWindow.h>
#include <opMainWindowIf.h>

#include <qlist.h>

class QSplitter;
class OPFrame;
class QPopupMenu;
class khcNavigator;
class khcBaseView;
class KFileBookmark;
class khcMainWindow;
class khcHTMLView;

class khcMainWindowIf : virtual public OPMainWindowIf,
			virtual public KHelpCenter::MainWindow_skel
{
public:
  khcMainWindowIf(khcMainWindow* _main);
  ~khcMainWindowIf();

  virtual void openURL( const Browser::URLRequest &url );
  
  virtual void setStatusBarText(const CORBA::WChar *_text);
  virtual void setLocationBarURL(OpenParts::Id id, const char *_url);
  
  virtual void createNewWindow( const char *url );
  virtual void slotURLStarted( OpenParts::Id id, const char *url );
  virtual void slotURLCompleted( OpenParts::Id id );
  
 protected:
  khcMainWindow* m_pkhcMainWindow;
};

class khcMainWindow : public OPMainWindow
{
    Q_OBJECT

 public:
    khcMainWindow(const QString& url = 0);
    virtual ~khcMainWindow();

    void openURL(Browser::URLRequest urlRequest);
    void openURL(const char *url, bool withHistory = true, long xOffset = 0, long yOffset = 0);

    unsigned long getListIndex() { return listIndex; }
    static khcMainWindow *getHelpWindowAt(unsigned long id) 
	{ return helpWindowList.at(id); }

    virtual OPMainWindowIf* interface();
    virtual khcMainWindowIf* khcInterface();

 protected:
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void setupLocationBar();
    void setupView();
    void connectView();
    void cleanUp();

    void enableNavigator(bool enable);

 public slots:

    // mainwindow related slots
    void slotNewBrowser();
    void slotOpenFile();

    void slotIntroduction();
    void slotForward();
    void slotBack();

    void slotSetBusy(bool busy);

    void slotSetBookmark();

    void slotToolbarClicked(int);
    void slotCheckHistory();
    
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

 protected:
    QSplitter       *m_pSplitter;
    khcNavigator    *m_pNavigator;
    OPFrame         *m_pFrame;
    khcHTMLView     *m_pView;
    khcMainWindowIf *m_pkhcInterface;

    Browser::View_var m_vView;
    khcHistory history;

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
    
    // static list of khcMainWindow windows
    static QList<khcMainWindow> helpWindowList;

    // index of khcMainWindow instance in helpWindowList
    unsigned long listIndex;
};

#endif
