/*
 *  khc_mainview.h - part of the KDE Help Center
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

#ifndef __khc_mainview_h__
#define __khc_mainview_h__

#include "khelpcenter.h"
#include "khc_history.h"


#include <opPart.h>
#include <openparts_ui.h>

#include <qstring.h>
#include <qwidget.h>
#include <qlist.h>

class QSplitter;
class OPFrame;
class khcNavigator;
class khcHTMLView;
class KAccel;

class khcMainView : public QWidget,
		    virtual public OPPartIf,
		    virtual public KHelpCenter::MainView_skel
{
    Q_OBJECT

 public:
    khcMainView(const char *url = 0L, QWidget *parent = 0L);
    ~khcMainView();

    virtual void init();
    virtual void cleanUp();

    virtual Browser::View_ptr childView();
    virtual OpenParts::Id childViewId();

    virtual bool event(const char* event, const CORBA::Any& value);
    bool mappingCreateMenubar(OpenPartsUI::MenuBar_ptr menuBar);
    bool mappingCreateToolbar(OpenPartsUI::ToolBarFactory_ptr factory);
    bool mappingParentGotFocus(OpenParts::Part_ptr child);
    bool mappingChildGotFocus(OpenParts::Part_ptr child);
    bool mappingOpenURL(Browser::EventOpenURL eventURL);

    void saveProperties(KConfig *config);
    void readProperties(KConfig *config);
    
    // IDL
    virtual void openURL(const Browser::URLRequest &url );
    virtual void open(const char* url, CORBA::Boolean reload, CORBA::Long xoffset = 0, CORBA::Long yoffset = 0);
    
    virtual void setStatusBarText(const CORBA::WChar *_text);
    virtual void setLocationBarURL(OpenParts::Id id, const char *_url);
    
    virtual void createNewWindow( const char *url );
    virtual void slotURLStarted( OpenParts::Id id, const char *url );
    virtual void slotURLCompleted( OpenParts::Id id );
    

 protected:
    
    virtual void resizeEvent(QResizeEvent *e);

    void createViewMenu();
    void createEditMenu();
    void createViewToolBar();
    void checkExtensions();
    void enableNavigator(bool enable);

    void connectView();
    void clearViewGUIElements();
    
 public slots:

    virtual void slotNewWindow();
    virtual void slotOpenFile();

    virtual void slotStop();
    virtual void slotReload();
    virtual void slotReloadNavigator();

    virtual void slotZoomIn();
    virtual void slotZoomOut();

    virtual void slotPrint();
    
    virtual void slotIntroduction();
    virtual void slotForward();
    virtual void slotBack();
    virtual void slotBookmark();

    virtual void slotShowNavigator();
    virtual void slotShowMenubar();
    virtual void slotShowToolbar();
    virtual void slotShowLocationbar();
    virtual void slotShowStatusbar();
    virtual void slotSettings();

    virtual void slotHelpContents();
    virtual void slotHelpAbout();

    virtual void slotHistoryFillBack();
    virtual void slotHistoryFillForward();
    virtual void slotHistoryBackActivated(CORBA::Long);
    virtual void slotHistoryForwardActivated(CORBA::Long);
    virtual void slotMenuEditAboutToShow();
    virtual void slotMenuViewAboutToShow();
    virtual void slotURLEntered(const CORBA::WChar *_url);

    void slotSetBusy(bool busy);
    void slotSetURL(const QString& _url);
    void slotCheckHistory();
    void slotSaveSettings();
    void slotReadSettings();

 protected:
    QSplitter       *m_pSplitter;
    khcNavigator    *m_pNavigator;
    OPFrame         *m_pFrame;
    khcHTMLView     *m_pView;
    KAccel          *m_pAccel;
    QString         m_initURL;

    bool m_bEditMenuDirty;
    bool m_bViewMenuDirty;
    CORBA::Long m_lToolBarViewStartIndex;

    Browser::View_var m_vView;
    khcHistory history;

    OpenPartsUI::ToolBar_var m_vToolBar;
    OpenPartsUI::ToolBar_var m_vLocationBar;
    OpenPartsUI::MenuBar_var m_vMenuBar;
    OpenPartsUI::StatusBar_var m_vStatusBar;

    OpenPartsUI::Menu_var m_vMenuFile;
    OpenPartsUI::Menu_var m_vMenuEdit;
    OpenPartsUI::Menu_var m_vMenuView;
    OpenPartsUI::Menu_var m_vMenuGo;
    OpenPartsUI::Menu_var m_vMenuBookmarks;
    OpenPartsUI::Menu_var m_vMenuOptions;
    OpenPartsUI::Menu_var m_vMenuHelp;

    OpenPartsUI::Menu_var m_vHistoryBackMenu;
    OpenPartsUI::Menu_var m_vHistoryForwardMenu;


    // tool-/menubar ids:
    enum { TB_NAVIGATOR, TB_BACK, TB_FORWARD, TB_RELOAD, TB_STOP, TB_PRINT,
	   TB_BOOKMARK, TB_ZOOMIN, TB_ZOOMOUT, TB_HELP, TB_URL,MFILE_NEWWINDOW,
	   MFILE_OPENFILE, MFILE_RELOAD, MFILE_PRINT, MGO_BACK, MGO_FORWARD,
	   MGO_INTRODUCTION, MOPTIONS_SHOWNAVIGATOR, MOPTIONS_SHOWMENUBAR,
	   MOPTIONS_SHOWTOOLBAR, MOPTIONS_SHOWLOCATIONBAR, MOPTIONS_SHOWSTATUSBAR,
	   MOPTIONS_SETTINGS, MHELP_CONTENTS, MHELP_ABOUT, MVIEW_ZOOMIN, MVIEW_ZOOMOUT,
	   MVIEW_RELOAD, MVIEW_RELOADNAVIGATOR };
    
    // UI options:
    bool m_showStatusBar, m_showToolBar, m_showLocationBar, m_showNavigator;
};

#endif // #ifndef __khc_mainview_h__
