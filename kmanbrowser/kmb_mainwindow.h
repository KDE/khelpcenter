/*
 *  kmb_mainwindow.h - part of the KDE Help Center
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

#ifndef __kmb_mainwindow_h__
#define __kmb_mainwindow_h__

#include "kmanbrowser.h"
#include "kmb_view.h"
#include "khc_history.h"

#include <opMainWindow.h>
#include <opMainWindowIf.h>

class OPFrame;
class QPopupMenu;
class kmbMainWindow;

class kmbMainWindowIf : virtual public OPMainWindowIf,
			virtual public KManBrowser::MainWindow_skel
{
 public:
  kmbMainWindowIf(kmbMainWindow* _main);
  ~kmbMainWindowIf();
  
  virtual void openURL(const Browser::URLRequest &url );
  virtual void open(const char* url, CORBA::Boolean reload, CORBA::Long xoffset, CORBA::Long yoffset);
  
  virtual void setStatusBarText(const CORBA::WChar *_text);
  virtual void setLocationBarURL(OpenParts::Id id, const char *_url);
  
  virtual void createNewWindow( const char *url );
  virtual void slotURLStarted( OpenParts::Id id, const char *url );
  virtual void slotURLCompleted( OpenParts::Id id );

  virtual void zoomIn();
  virtual void zoomOut();
  virtual void print();
  virtual void reload();

  virtual void openFile();
  virtual void index();
  virtual void forward();
  virtual void back();
  virtual void bookmark();
  virtual void options();

 protected:
  kmbMainWindow* m_pkmbMainWindow;
};

class kmbMainWindow : public OPMainWindow
{
    Q_OBJECT

 public:
    kmbMainWindow(const QString& url = 0);
    virtual ~kmbMainWindow();

    void openURL(Browser::URLRequest urlRequest);
    void openURL(const char *url, bool withHistory = true, long xOffset = 0, long yOffset = 0);

    virtual OPMainWindowIf* interface();
    virtual kmbMainWindowIf* kmbInterface();

 protected:
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void setupLocationBar();
    void setupView();
    void connectView();
    void cleanUp();

 public slots:

    // mainwindow related slots
    void slotNewBrowser();
    void slotOpenFile();

    void slotIndex();
    void slotForward();
    void slotBack();

    void slotSetBusy(bool busy);

    void slotSetBookmark();

    void slotToolbarClicked(int);
    void slotCheckHistory();
    
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
    OPFrame         *m_pFrame;
    kmbMainWindowIf *m_pkmbInterface;
    kmbView         *m_pView;

    KManBrowser::View_var m_vView;
    khcHistory history;

    QPopupMenu *m_pFileMenu, *m_pEditMenu, *m_pViewMenu, *m_pGotoMenu, *m_pOptionsMenu, *m_pHelpMenu,*m_pBookmarkMenu;
    QPopupMenu *m_pHistoryBackMenu, *m_pHistoryForwardMenu;

    // toolbar and menu id's
    enum {TB_BACK, TB_FORWARD, TB_RELOAD, TB_STOP, TB_PRINT, TB_SETBOOKMARK, TB_ZOOMIN, TB_ZOOMOUT, TB_FIND};
    int idCopy, idBack, idForward, idToolBar ,idLocationBar, idStatusBar, idMagPlus, idMagMinus;

    // UI options:
    bool m_showStatusBar, m_showToolBar, m_showLocationBar;
};

#endif
