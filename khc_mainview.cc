/*
 *  khc_mainview.cc - part of the KDE Helpcenter
 *
 *  Copyright (c) 1999 Matthias Elter (me@kde.org)
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

#include "khc_mainwindow.h"
#include "khc_mainview.h"
#include "khc_htmlview.h"
#include "khc_navigator.h"
#include "version.h"

#include <qkeycode.h>
#include <qvaluelist.h>
#include <qsplitter.h>

#include <kapp.h>
#include <kaccel.h>
#include <kiconloader.h>
#include <kstdaccel.h>
#include <klocale.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <kdebug.h>
#include <kfiledialog.h>
#include <kconfig.h>
#include <kpixmapcache.h>
#include <kmessagebox.h>

#include <opUIUtils.h>
#include <opToolBar.h>
#include <opStatusBar.h>
#include <opFrame.h>

khcMainView::khcMainView(const char *url, QWidget *parent )
  : QWidget(parent)
{
  kdebug(KDEBUG_INFO,1400,"+khcMainView");
  ADD_INTERFACE("IDL:KHelpCenter/MainView:1.0");

  setWidget(this);
  OPPartIf::setFocusPolicy(OpenParts::Part::ClickFocus);
  
  m_vMenuFile = 0L;
  m_vMenuEdit = 0L;
  m_vMenuView = 0L;
  m_vMenuGo = 0L;
  m_vMenuBookmarks = 0L;
  m_vMenuOptions = 0L;
  m_vToolBar = 0L;
  m_vLocationBar = 0L;
  m_vMenuBar = 0L;
  m_vStatusBar = 0L;
  m_vView = 0L;

  m_initURL = url;

  m_pAccel = new KAccel(this);
  m_pAccel->insertItem(i18n("History Back"), "Back", ALT+Key_Left);
  m_pAccel->insertItem(i18n("History Forward"), "Forward", ALT+Key_Right);
  m_pAccel->connectItem("Back", this, SLOT(slotBack()));
  m_pAccel->connectItem("Forward", this, SLOT(slotForward()));
  m_pAccel->readSettings();

  kdebug(KDEBUG_INFO,1400,"+khcMainView : done");
}

khcMainView::~khcMainView()
{
  kdebug(KDEBUG_INFO,1400,"-khcMainView");
  
  slotSaveSettings();
  
  cleanUp();
  
  delete m_pFrame;
  m_pFrame = 0L;
  delete m_pNavigator;
  m_pNavigator = 0L;
  delete m_pSplitter;
  m_pSplitter = 0L;

  kdebug(KDEBUG_INFO,1400,"-khcMainView : done");
}

void khcMainView::init()
{
  kdebug(KDEBUG_INFO,1400,"khcMainView::init()");

  // register myself at the mainwindows GUI-managers
  OpenParts::MenuBarManager_var menuBarManager = m_vMainWindow->menuBarManager();
  if (!CORBA::is_nil(menuBarManager))
    menuBarManager->registerClient(id(), this);
  
  OpenParts::ToolBarManager_var toolBarManager = m_vMainWindow->toolBarManager();
  if (!CORBA::is_nil(toolBarManager))
    toolBarManager->registerClient(id(), this);
  
  //OpenParts::StatusBarManager_var statusBarManager = m_vMainWindow->statusBarManager();
  //if (!CORBA::is_nil(statusBarManager))
  //  m_vStatusBar = statusBarManager->registerClient(id());
  
  // this is causing a MICO exception when called by khcclient ?!?
  // add a statusbar field
  //CORBA::WString_var item = Q2C(i18n("KDE Helpcenter"));
  //if (!CORBA::is_nil(m_vStatusBar))
  ///  m_vStatusBar->insertItem(item, 1);
  
  // the splitter managing the OPFrame and khcNavigator
  m_pSplitter = new QSplitter(QSplitter::Horizontal, this);
  CHECK_PTR(m_pSplitter);

  // the navigator
  m_pNavigator = new khcNavigator(m_pSplitter);
  CHECK_PTR(m_pNavigator);
  QObject::connect(m_pNavigator , SIGNAL(itemSelected(const QString&)), this, SLOT(slotSetURL(const QString&)));

  // the OPFrame used to attach child views
  m_pFrame = new OPFrame(m_pSplitter);
  CHECK_PTR(m_pFrame);
  
  // set splitter sizes and geometry
  QValueList<int> sizes;
  sizes << 200 << 600;
  m_pSplitter->setSizes(sizes);
  m_pSplitter->setGeometry(0, 0, width(), height()); 
  m_pSplitter->show();

  // embed a HTMLView since automatic view embeding based on protocol/mimetype is not implemented yet
  m_pView = new khcHTMLView;
  m_vView = KHelpCenter::HTMLView::_duplicate(m_pView);
  m_vView->setMainWindow(m_vMainWindow);
  m_vView->setParent(this);
  
  m_pFrame->attach(m_vView);
  connectView();
  

  // open the initial url or show the intro page
  if (m_initURL.isEmpty()) slotIntroduction();
  else open(m_initURL, false);

  kdebug(KDEBUG_INFO,1400,"khcMainView::init() : done");
}

void khcMainView::cleanUp()
{
  if (m_bIsClean)
    return;
  
  kdebug(KDEBUG_INFO,1400,"khcMainView::cleanUp()");
 
  m_vStatusBar = 0L;
  
  // unregister from GUI managers
  OpenParts::MenuBarManager_var menuBarManager = m_vMainWindow->menuBarManager();
  if (!CORBA::is_nil(menuBarManager))
    menuBarManager->unregisterClient(id());

  OpenParts::ToolBarManager_var toolBarManager = m_vMainWindow->toolBarManager();
  if (!CORBA::is_nil(toolBarManager))
    toolBarManager->unregisterClient(id());
  
  OpenParts::StatusBarManager_var statusBarManager = m_vMainWindow->statusBarManager();
  if (!CORBA::is_nil(statusBarManager))
    statusBarManager->unregisterClient(id());

  delete m_pAccel;
  
  // Release the view component. This will delete the component.
  if (m_pFrame)
    m_pFrame->detach();

  m_vView = 0L;

  OPPartIf::cleanUp();
  kdebug(KDEBUG_INFO,1400,"khcMainView::cleanUp() : done");
}

bool khcMainView::event(const char* event, const CORBA::Any& value)
{
  EVENT_MAPPER(event, value);

  MAPPING(OpenPartsUI::eventCreateMenuBar, OpenPartsUI::typeCreateMenuBar_ptr, mappingCreateMenubar);
  MAPPING(OpenPartsUI::eventCreateToolBar, OpenPartsUI::typeCreateToolBar_ptr, mappingCreateToolbar);
  MAPPING(OpenParts::eventParentGotFocus, OpenParts::Part_ptr, mappingParentGotFocus);
  MAPPING(OpenParts::eventChildGotFocus, OpenParts::Part_ptr, mappingParentGotFocus);
  MAPPING(Browser::eventOpenURL, Browser::EventOpenURL, mappingOpenURL);

  END_EVENT_MAPPER;
  return false;
}

bool khcMainView::mappingCreateMenubar(OpenPartsUI::MenuBar_ptr menuBar)
{
  kdebug(KDEBUG_INFO,1400,"khcMainView::mappingCreateMenubar()");

  m_vMenuBar = OpenPartsUI::MenuBar::_duplicate(menuBar);
  
  if (CORBA::is_nil(menuBar))
    {
      m_vMenuEdit->disconnect("aboutToShow", this, "slotMenuEditAboutToShow");
      m_vMenuView->disconnect("aboutToShow", this, "slotMenuViewAboutToShow");
      
      m_vMenuFile = 0L;
      m_vMenuEdit = 0L;
      m_vMenuView = 0L;
      m_vMenuGo = 0L;
      m_vMenuBookmarks = 0L;
      m_vMenuOptions = 0L;
      m_vMenuHelp = 0L;
      
      return true;
    }
  
  OpenPartsUI::Pixmap_var pix;
  KStdAccel stdAccel;
  
  // file menu
  CORBA::WString_var text = Q2C(i18n("&File"));
  CORBA::Long m_idMenuFile = menuBar->insertMenu(text, m_vMenuFile, -1, -1);
  
  // open new window
  text = Q2C(i18n("New &Window"));
  m_vMenuFile->insertItem4(text, this, "slotNewWindow", stdAccel.openNew(), MFILE_NEWWINDOW, -1);
  
  // seperator
  m_vMenuFile->insertSeparator(-1);
  
  // open file...
  pix = OPUIUtils::convertPixmap(*KPixmapCache::toolbarPixmap("fileopen.xpm"));
  text = Q2C(i18n("&Open file..."));
  m_vMenuFile->insertItem6(pix, text, this, "slotOpenFile", stdAccel.open(), MFILE_OPENFILE, -1);
  
  // reload
  pix = OPUIUtils::convertPixmap(*KPixmapCache::toolbarPixmap("reload.xpm"));
  text = Q2C(i18n("&Reload"));
  m_vMenuFile->insertItem6(pix, text, this, "slotReload", Key_F5, MFILE_RELOAD, -1);
  
  // print
  pix = OPUIUtils::convertPixmap(*KPixmapCache::toolbarPixmap("fileprint.xpm"));
  text = Q2C(i18n("&Print"));
  m_vMenuFile->insertItem6(pix, text, this, "slotPrint", stdAccel.print(), MFILE_PRINT, -1);
  
  // let the mainwindow know about the file menu so it cna replace its default file menu
  menuBar->setFileMenu(m_idMenuFile);
  
  // edit menu
  text = Q2C(i18n("&Edit"));
  menuBar->insertMenu(text, m_vMenuEdit, -1, -1);
  
  m_vMenuEdit->connect("aboutToShow", this, "slotMenuEditAboutToShow");
  m_bEditMenuDirty = true;
  createEditMenu();
  
  // view menu
  text = Q2C(i18n("&View"));
  menuBar->insertMenu( text, m_vMenuView, -1, -1 );  
  
  m_vMenuView->connect("aboutToShow", this, "slotMenuViewAboutToShow");
  m_bViewMenuDirty = true;
  createViewMenu();
  
  // go menu
  text = Q2C(i18n("&Go"));
  menuBar->insertMenu(text, m_vMenuGo, -1, -1);

  // back
  pix = OPUIUtils::convertPixmap(*KPixmapCache::toolbarPixmap("back.xpm"));
  text = Q2C(i18n("&Back"));
  m_vMenuGo->insertItem6(pix, text, this, "slotBack", 0, MGO_BACK, -1);
  
  // forward
  pix = OPUIUtils::convertPixmap(*KPixmapCache::toolbarPixmap("forward.xpm"));
  text = Q2C(i18n("&Forward"));
  m_vMenuGo->insertItem6(pix, text, this, "slotForward", 0, MGO_FORWARD, -1);
  
  // seperator
  m_vMenuGo->insertSeparator(-1);
  
  // introduction
  pix = OPUIUtils::convertPixmap(*KPixmapCache::toolbarPixmap("home.xpm"));
  text = Q2C(i18n("&Introduction"));
  m_vMenuGo->insertItem6(pix, text, this, "slotIntroduction", 0, MGO_INTRODUCTION, -1);
  
  // bookmark menu
  text = Q2C(i18n("&Bookmarks"));
  menuBar->insertMenu(text, m_vMenuBookmarks, -1, -1);
  //m_pBookmarkMenu = new KBookmarkMenu( this, m_vMenuBookmarks, this, true );
  
  // options menu
  text = Q2C(i18n("&Options"));
  menuBar->insertMenu(text, m_vMenuOptions, -1, -1);
  
  // show navigator
  text = Q2C(i18n("Show &Navigator"));
  m_vMenuOptions->insertItem4(text, this, "slotShowNavigator", 0, MOPTIONS_SHOWNAVIGATOR, -1 );
  
  // show menubar
  text = Q2C(i18n("Show &Menubar"));
  m_vMenuOptions->insertItem4(text, this, "slotShowMenubar", 0, MOPTIONS_SHOWMENUBAR, -1 );
  
  // show toolbar
  text = Q2C(i18n("Show &Toolbar"));
  m_vMenuOptions->insertItem4( text, this, "slotShowToolbar", 0, MOPTIONS_SHOWTOOLBAR, -1 );
  
  // show locationbar
  text = Q2C(i18n("Show &Locationbar"));
  m_vMenuOptions->insertItem4( text, this, "slotShowLocationbar", 0, MOPTIONS_SHOWLOCATIONBAR, -1 );
  
  // show statusbar
  text = Q2C( i18n("Show &Statusbar"));
  m_vMenuOptions->insertItem4( text, this, "slotShowStatusbar", 0, MOPTIONS_SHOWSTATUSBAR, -1 );
  
  // seperator
  m_vMenuOptions->insertSeparator(-1);
  
  // settings
  pix = OPUIUtils::convertPixmap(*KPixmapCache::toolbarPixmap("kde1.xpm"));
  text = Q2C(i18n("&Settings..."));
  m_vMenuOptions->insertItem6(pix, text, this, "slotSettings", 0, MOPTIONS_SETTINGS, -1);
  
  // help menu
  text = Q2C(i18n("&Help"));
  CORBA::Long helpId = m_vMenuBar->insertMenu(text, m_vMenuHelp, -1, -1);

  // similar to the file menu the help menu is handled specially by the mainwindow
  m_vMenuBar->setHelpMenu(helpId);
  
  // contents
  text = Q2C(i18n("&Contents"));
  m_vMenuHelp->insertItem4(text, this, "slotHelpContents", 0, MHELP_CONTENTS, -1);
  
  // seperator
  m_vMenuHelp->insertSeparator(-1);
  
  // about khc
  text = Q2C(i18n("&About KHelpcenter"));
  m_vMenuHelp->insertItem4(text, this, "slotHelpAbout", 0, MHELP_ABOUT, -1);
  
  m_vMenuOptions->setItemChecked(MOPTIONS_SHOWNAVIGATOR, true);
  m_vMenuOptions->setItemChecked(MOPTIONS_SHOWMENUBAR, true);
  m_vMenuOptions->setItemChecked(MOPTIONS_SHOWSTATUSBAR, true);
  m_vMenuOptions->setItemChecked(MOPTIONS_SHOWTOOLBAR, true);
  m_vMenuOptions->setItemChecked(MOPTIONS_SHOWLOCATIONBAR, true);
  
  checkExtensions();
  
  kdebug(KDEBUG_INFO,1400,"khcMainView::mappingCreateMenubar() : done");
  return true;
}

bool khcMainView::mappingCreateToolbar(OpenPartsUI::ToolBarFactory_ptr factory)
{
  kdebug(KDEBUG_INFO,1400,"khcMainView::mappingCreateToolbar()");
  
  if (CORBA::is_nil(factory))
    {
      m_vHistoryBackMenu->disconnect("aboutToShow", this, "slotHistoryFillBack");
      m_vHistoryBackMenu->disconnect("activated", this, "slotHistoryBackActivated");
      m_vHistoryForwardMenu->disconnect("aboutToShow", this, "slotHistoryFillForward");
      m_vHistoryForwardMenu->disconnect("activated", this, "slotHistoryForwardActivated");
      
      m_vHistoryBackMenu = 0L;
      m_vHistoryForwardMenu = 0L;
      
      m_vToolBar = 0L;
      m_vLocationBar = 0L;
      
      return true;
    }
  
  // setup toolbar
  m_vToolBar = factory->create2(OpenPartsUI::ToolBarFactory::Transient, 40);
  m_vToolBar->setIconText(3);
  m_vToolBar->setFullWidth(true);
				    
  CORBA::WString_var text;
  OpenPartsUI::Pixmap_var pix;
  
  // show/hide navigator
  text = Q2C(i18n("Navigator"));
  pix = OPUIUtils::convertPixmap(*KPixmapCache::toolbarPixmap("hidenavigator.xpm"));
  m_vToolBar->insertButton2(pix, TB_NAVIGATOR, SIGNAL(clicked()), this, "slotShowNavigator", true, text, -1);

  // seperator
  m_vToolBar->insertSeparator(-1);
  
  // back
  text = Q2C(i18n("Back"));
  pix = OPUIUtils::convertPixmap(*KPixmapCache::toolbarPixmap("back.xpm"));
  m_vToolBar->insertButton2(pix, TB_BACK, SIGNAL(clicked()), this, "slotBack", false, text, -1);
  m_vToolBar->setDelayedPopup(TB_BACK, m_vHistoryBackMenu);
  m_vToolBar->setItemEnabled(TB_BACK, false);
  m_vHistoryBackMenu->connect("aboutToShow", this, "slotHistoryFillBack");
  m_vHistoryBackMenu->connect("activated", this, "slotHistoryBackActivated");

  // forward
  text = Q2C(i18n("Forward"));
  pix = OPUIUtils::convertPixmap(*KPixmapCache::toolbarPixmap("forward.xpm"));
  m_vToolBar->insertButton2(pix, TB_FORWARD, SIGNAL(clicked()), this, "slotForward", false, text, -1);
  m_vToolBar->setDelayedPopup(TB_FORWARD, m_vHistoryForwardMenu);
  m_vToolBar->setItemEnabled(TB_FORWARD, false);
  m_vHistoryForwardMenu->connect("aboutToShow", this, "slotHistoryFillForward");
  m_vHistoryForwardMenu->connect("activated", this, "slotHistoryForwardActivated");

  // seperator
  m_vToolBar->insertSeparator(-1);

  // reload
  text = Q2C(i18n("Reload"));
  pix = OPUIUtils::convertPixmap(*KPixmapCache::toolbarPixmap("reload.xpm"));
  m_vToolBar->insertButton2(pix, TB_RELOAD, SIGNAL(clicked()), this, "slotReload", true, text, -1);
   
  // stop
  text = Q2C(i18n("Stop"));
  pix = OPUIUtils::convertPixmap(*KPixmapCache::toolbarPixmap("stop.xpm"));
  m_vToolBar->insertButton2(pix, TB_STOP, SIGNAL(clicked()), this, "slotStop", false, text, -1);

  // bookmark
  text = Q2C(i18n("Bookmark"));
  pix = OPUIUtils::convertPixmap(*KPixmapCache::toolbarPixmap("flag.xpm"));
  m_vToolBar->insertButton2(pix, TB_BOOKMARK, SIGNAL(clicked()), this, "slotBookmark", true, text, -1);

  // seperator
  m_vToolBar->insertSeparator(-1);
  
  // zoom in
  text = Q2C(i18n("Zoom in"));
  pix = OPUIUtils::convertPixmap(*KPixmapCache::toolbarPixmap("viewmag+.xpm"));
  m_vToolBar->insertButton2(pix, TB_ZOOMIN, SIGNAL(clicked()), this, "slotZoomIn", true, text, -1);

  // zoom out
  text = Q2C(i18n("Zoom out"));
  pix = OPUIUtils::convertPixmap(*KPixmapCache::toolbarPixmap("viewmag-.xpm"));
  m_vToolBar->insertButton2(pix, TB_ZOOMOUT, SIGNAL(clicked()), this, "slotZoomOut", true, text, -1);
  
  // seperator
  CORBA::Long childButtonIndex = m_vToolBar->insertSeparator(-1);
  
  // print
  text = Q2C(i18n("Print"));
  pix = OPUIUtils::convertPixmap(*KPixmapCache::toolbarPixmap("fileprint.xpm"));
  m_vToolBar->insertButton2(pix, TB_PRINT, SIGNAL(clicked()), this, "slotPrint", true, text, -1);

  // seperator				
  m_vToolBar->insertSeparator( -1 );				

  // help
  text = Q2C(i18n("Help"));
  pix = OPUIUtils::convertPixmap(*KPixmapCache::toolbarPixmap("help.xpm"));
  m_vToolBar->insertButton2(pix, TB_HELP, SIGNAL(clicked()), this, "slotHelpContents", true, text, -1);
  m_vToolBar->alignItemRight(TB_HELP, true);				
  
  // view specific buttons
  m_lToolBarViewStartIndex = childButtonIndex;
  if (m_vView)
    createViewToolBar();

  // setup location bar
  m_vLocationBar = factory->create(OpenPartsUI::ToolBarFactory::Transient);
  m_vLocationBar->setFullWidth(true);
  
  text = Q2C(i18n("Location: "));
  m_vLocationBar->insertTextLabel(text, -1, -1);
  CORBA::WString_var toolTip = Q2C(i18n("Current location"));
  
  text = Q2C(QString::null);
  OpenPartsUI::WStrList items;
  items.length(0);

  if (m_vView)
    {
      items.length(1);
      items[0] = Q2C(m_vView->url());
    }
  
  m_vLocationBar->insertCombo3(items, TB_URL, true, SIGNAL(activated(const QString &)),
			       this, "slotURLEntered", true, toolTip, 70, -1,
			       OpenPartsUI::AfterCurrent);
  
  m_vLocationBar->setComboAutoCompletion(TB_URL, true);
  m_vLocationBar->setItemAutoSized(TB_URL, true);
  checkExtensions();
  
  // read and apply settings
  slotReadSettings();
  
  kdebug(KDEBUG_INFO,1400,"khcMainView::mappingCreateToolbar() : done");
  return true;
}

bool khcMainView::mappingParentGotFocus(OpenParts::Part_ptr )
{
  kdebug(0, 1400, "bool khcMainView::mappingParentGotFocus( OpenParts::Part_ptr child )");
 
  if (m_vView)
    {
      clearViewGUIElements();
    }

  m_vMenuGo->setItemEnabled(MGO_BACK, false );
  m_vMenuGo->setItemEnabled(MGO_FORWARD, false );
  return true;
}

bool khcMainView::mappingChildGotFocus(OpenParts::Part_ptr)
{
  kdebug(KDEBUG_INFO, 1400, "bool khcMainView::mappingChildGotFocus(OpenParts::Part_ptr child)");
  return true;
}

bool khcMainView::mappingOpenURL( Browser::EventOpenURL eventURL )
{
  openURL(eventURL);
  return true;
}

void khcMainView::createViewMenu()
{
  if (!CORBA::is_nil(m_vMenuView) && m_bViewMenuDirty)
    {
      kdebug(KDEBUG_INFO,1400,"khcMainView::createViewMenu()");
      m_vMenuView->clear();
      m_vMenuView->setCheckable(true);
      
      CORBA::WString_var text;
      OpenPartsUI::Pixmap_var pix;
      
      // zoom in
      pix = OPUIUtils::convertPixmap(*KPixmapCache::toolbarPixmap("viewmag+.xpm"));
      text = Q2C(i18n("Zoom &in"));
      m_vMenuView->insertItem6(pix, text, this, "slotZoomIn", 0, MVIEW_ZOOMIN, -1);
      
      // zoom out
      pix = OPUIUtils::convertPixmap(*KPixmapCache::toolbarPixmap("viewmag+.xpm"));
      text = Q2C(i18n("Zoom &out"));
      m_vMenuView->insertItem6(pix, text, this, "slotZoomOut", 0, MVIEW_ZOOMOUT, -1);
      
      // child view specific menu entries
      if (m_vView)
	{ 
	  EMIT_EVENT(m_vView, Browser::View::eventFillMenuView, m_vMenuView);
	}
      
      // seperator
      m_vMenuView->insertSeparator(-1);

      // reload document
      pix = OPUIUtils::convertPixmap(*KPixmapCache::toolbarPixmap("reload.xpm"));
      text = Q2C(i18n("Reload &document"));
      m_vMenuView->insertItem6(pix, text, this, "slotReload", 0, MVIEW_RELOAD, -1);

      // reload navigator
      text = Q2C(i18n("Reload &navigator"));
      m_vMenuView->insertItem4(text, this, "slotReloadNavigator", 0, MVIEW_RELOADNAVIGATOR, -1);
      m_bViewMenuDirty = false;
  }
}

void khcMainView::createEditMenu()
{
  if (!CORBA::is_nil(m_vMenuEdit) && m_bEditMenuDirty)
    {
      kdebug(KDEBUG_INFO,1400,"khcMainView::createEditMenu()");
      m_vMenuEdit->clear();
      m_vMenuEdit->setCheckable(true);

      // child view specific menu entries
      if (m_vView)
	{ 
	  EMIT_EVENT(m_vView, Browser::View::eventFillMenuEdit, m_vMenuEdit);
	}
      m_bEditMenuDirty = false;
  }
}

void khcMainView::createViewToolBar()
{
  if (CORBA::is_nil(m_vToolBar))
    return;

  kdebug(KDEBUG_INFO,1400,"khcMainView::createViewToolBar()");
  
  Browser::View::EventFillToolBar ev;
  ev.create = (CORBA::Boolean)true;
  ev.startIndex = m_lToolBarViewStartIndex;
  ev.toolBar = OpenPartsUI::ToolBar::_duplicate(m_vToolBar);
  EMIT_EVENT(m_vView, Browser::View::eventFillToolBar, ev);
}


void khcMainView::clearViewGUIElements()
{
  // clear child-view  edit menu entries
  EMIT_EVENT(m_vView, Browser::View::eventFillMenuEdit, 0L);

  // clear child-view view menu entries
  EMIT_EVENT(m_vView, Browser::View::eventFillMenuView, 0L);
  
  // clear child-view toolbar buttons
  Browser::View::EventFillToolBar ev;
  ev.create = (CORBA::Boolean)false;
  ev.toolBar = OpenPartsUI::ToolBar::_duplicate(m_vToolBar);
  EMIT_EVENT(m_vView, Browser::View::eventFillToolBar, ev);
}

void khcMainView::checkExtensions()
{
  if (!CORBA::is_nil(m_vView))
    {
      // does the child-view support the printing extension interface ?
      bool print = m_vView->supportsInterface("IDL:Browser/PrintingExtension:1.0");
      bool canZoomIn, canZoomOut = false;

      // does the child-view support the magnifying extension interface?
      if (m_vView->supportsInterface("IDL:Browser/MagnifyingExtension:1.0"))
	{
	  CORBA::Object_var obj = m_vView->getInterface("IDL:Browser/MagnifyingExtension:1.0");
	  Browser::MagnifyingExtension_var magExt = Browser::MagnifyingExtension::_narrow(obj);
	  
	  // can we currently zoom in/out?
	  canZoomIn =  magExt->canZoomIn();
	  canZoomIn =  magExt->canZoomOut();
	}

      if (!CORBA::is_nil(m_vMenuView))
	{
	  m_vMenuView->setItemEnabled(MVIEW_ZOOMIN, canZoomIn);
	  m_vMenuView->setItemEnabled(MVIEW_ZOOMOUT, canZoomOut);
	}

      if(!CORBA::is_nil(m_vMenuFile))
	m_vMenuFile->setItemEnabled(MFILE_PRINT, print);

      if (!CORBA::is_nil(m_vToolBar))
	{
	  m_vToolBar->setItemEnabled(TB_PRINT, print);
	  m_vToolBar->setItemEnabled(TB_ZOOMIN, canZoomIn);
	  m_vToolBar->setItemEnabled(TB_ZOOMOUT, canZoomOut);
	}
    }
}

Browser::View_ptr khcMainView::childView()
{
  if (!CORBA::is_nil(m_vView))
    return Browser::View::_duplicate(m_vView);
  else
    return Browser::View::_nil();
}

OpenParts::Id khcMainView::childViewId()
{
  if (!CORBA::is_nil(m_vView))
    return m_vView->id();
  else
    return 0;
}

void khcMainView::resizeEvent(QResizeEvent *)
{
  // adjust geometry of the splitter holding the navigator and the OPFrame
  if(m_pSplitter)
    m_pSplitter->setGeometry(0, 0, width(), height()); 
}

void khcMainView::enableNavigator(bool enable)
{
  // simply enable/disable navigator by setting splitter sizes
  if (enable)
    {
      QValueList<int> sizes;
      sizes.append(200);
      sizes.append(600);

      m_pSplitter->setSizes(sizes);
    }
  else
    {
      QValueList<int> sizes;
      sizes.append(0);
      sizes.append(800);

      m_pSplitter->setSizes(sizes);
    }
}

void khcMainView::slotReadSettings()
{
  KConfig *config = KApplication::getKApplication()->getConfig();
  config->setGroup("Appearance");
  
  // show tool-, location-, statusbar and navigator?
  QString o = config->readEntry("ShowNavigator");
  if (!o.isEmpty() && o.find("No", 0, false ) == 0)
    m_showNavigator = false;
  else
    m_showNavigator = true;

  o = config->readEntry("ShowMenuBar");
  if (!o.isEmpty() && o.find("No", 0, false ) == 0)
    m_showMenuBar = false;
  else
    m_showMenuBar = true;
  
  o = config->readEntry("ShowToolBar");
  if (!o.isEmpty() && o.find( "No", 0, false) == 0 )
    m_showToolBar = false;
  else
    m_showToolBar = true;
  
  o = config->readEntry("ShowStatusBar");
  if ( !o.isEmpty() && o.find("No", 0, false ) == 0)
    m_showStatusBar = false;
  else
    m_showStatusBar = true;
  
  o = config->readEntry("ShowLocationBar");
  if (!o.isEmpty() && o.find("No", 0, false ) == 0)
    m_showLocationBar = false;
  else
    m_showLocationBar = true;
  
  // toolbar location
  if (!CORBA::is_nil(m_vToolBar))
    {
      o = config->readEntry("ToolBarPos");
      if (o.isEmpty())
	m_vToolBar->setBarPos(OpenPartsUI::Top);
      else if ("Top" == o) 
	m_vToolBar->setBarPos(OpenPartsUI::Top);
      else if ("Bottom" == o)
	m_vToolBar->setBarPos(OpenPartsUI::Bottom);
      else if ("Left" == o)
	m_vToolBar->setBarPos(OpenPartsUI::Left);
      else if ("Right" == o)
	m_vToolBar->setBarPos(OpenPartsUI::Right);
      else if ("Floating" == o)
	m_vToolBar->setBarPos(OpenPartsUI::Floating);
      else
	m_vToolBar->setBarPos(OpenPartsUI::Top);
    }

  // locationbar location
  if (!CORBA::is_nil(m_vLocationBar))
    {
      o = config->readEntry("LocationBarPos");
      if ( o.isEmpty() )
	m_vLocationBar->setBarPos(OpenPartsUI::Top);
      else if ("Top" == o) 
	m_vLocationBar->setBarPos(OpenPartsUI::Top);
      else if ("Bottom" == o)
	m_vLocationBar->setBarPos(OpenPartsUI::Bottom);
      else if ("Left" == o)
	m_vLocationBar->setBarPos(OpenPartsUI::Left);
      else if ("Right" == o)
	m_vLocationBar->setBarPos(OpenPartsUI::Right);
      else if ("Floating" == o)
	m_vLocationBar->setBarPos(OpenPartsUI::Floating);
      else
	m_vLocationBar->setBarPos(OpenPartsUI::Top);
    }
  
  // set configuration
  if (m_showNavigator)
    enableNavigator(true);
  else
    enableNavigator(false);
  
  if(!CORBA::is_nil(m_vStatusBar))
    {
      if (m_showStatusBar)
	m_vStatusBar->enable(OpenPartsUI::Show);
      else
	m_vStatusBar->enable(OpenPartsUI::Hide);
    }
  
  if(!CORBA::is_nil(m_vToolBar))
    {
      if (m_showToolBar) 
	m_vToolBar->enable(OpenPartsUI::Show);
      else
	m_vToolBar->enable(OpenPartsUI::Hide);
    }

  if(!CORBA::is_nil(m_vLocationBar))
    {
      if (m_showLocationBar)
	m_vLocationBar->enable(OpenPartsUI::Show);
      else
	m_vLocationBar->enable(OpenPartsUI::Hide);
    }
  
  // toggle menu items
  if (!CORBA::is_nil(m_vMenuOptions))
    {
      m_vMenuOptions->setItemChecked(MOPTIONS_SHOWNAVIGATOR, m_showNavigator);
      m_vMenuOptions->setItemChecked(MOPTIONS_SHOWTOOLBAR, m_showToolBar);
      m_vMenuOptions->setItemChecked(MOPTIONS_SHOWLOCATIONBAR, m_showLocationBar);
      m_vMenuOptions->setItemChecked(MOPTIONS_SHOWSTATUSBAR, m_showStatusBar);
    }
}

void khcMainView::slotSaveSettings()
{
  KConfig *config = KApplication::getKApplication()->getConfig();
  
  config->setGroup( "Appearance" );
  config->writeEntry( "ShowMenuBar", m_showMenuBar ? "Yes" : "No" ); 
  config->writeEntry( "ShowNavigator", m_showNavigator ? "Yes" : "No" ); 
  config->writeEntry( "ShowToolBar", m_showToolBar ? "Yes" : "No" );
  config->writeEntry( "ShowStatusBar", m_showStatusBar ? "Yes" : "No" );  
  config->writeEntry( "ShowLocationBar", m_showLocationBar ? "Yes" : "No" );
  
  if (!CORBA::is_nil(m_vToolBar))
    {
      switch (m_vToolBar->barPos())
	{
	case OpenPartsUI::Top:
	  config->writeEntry( "ToolBarPos", "Top");
	  break;
	case OpenPartsUI::Bottom:
	  config->writeEntry( "ToolBarPos", "Bottom");
	  break;
	case OpenPartsUI::Left:
	  config->writeEntry( "ToolBarPos", "Left");
	  break;
	case OpenPartsUI::Right:
	  config->writeEntry( "ToolBarPos", "Right");
	  break;
	case OpenPartsUI::Floating:
	  config->writeEntry( "ToolBarPos", "Floating");
	  break;
	default:
	  warning("helpCenter::slotOptionsSave: illegal default in case reached\n");
	  break;
	}
    }
  
  if (!CORBA::is_nil(m_vLocationBar))
    {
      switch (m_vLocationBar->barPos())
	{
	case OpenPartsUI::Top:
	  config->writeEntry( "LocationBarPos", "Top");
	  break;
	case OpenPartsUI::Bottom:
	  config->writeEntry( "LocationBarPos", "Bottom");
	  break;
	case OpenPartsUI::Left:
	  config->writeEntry( "LocationBarPos", "Left");
	  break;
	case OpenPartsUI::Right:
	  config->writeEntry( "LocationBarPos", "Right");
	  break;
	case OpenPartsUI::Floating:
	  config->writeEntry( "LocationBarPos", "Floating");
	  break;
	default:
	  warning("khcMainView::slotOptionsSave: illegal default in case reached\n");
	  break;
	}
    }
}

void khcMainView::slotNewWindow()
{
  QString url = m_vView->url();
  khcMainWindow *m_pWin = new khcMainWindow(url);
  m_pWin->show();
}

void khcMainView::slotOpenFile()
{
  // show KFileDialog
  QString fileName = KFileDialog::getOpenFileName();
  if (!fileName.isNull())
    {
      QString url = "file:";
      url += fileName;
      open(url, false);
    }
}

void khcMainView::slotStop()
{
  // stop processing the current url
  if (!CORBA::is_nil(m_vView))
    m_vView->stop();
}

void khcMainView::slotReload()
{
  Browser::EventOpenURL eventURL;
  eventURL.url = m_vView->url();
  eventURL.reload = (CORBA::Boolean)true;
  eventURL.xOffset = m_vView->xOffset();
  eventURL.yOffset = m_vView->yOffset();
  EMIT_EVENT( m_vView, Browser::eventOpenURL, eventURL );
  kdebug(0, 1400, "EMIT_EVENT( m_vView, Browser::eventOpenURL, eventURL );");
}

void khcMainView::slotReloadNavigator()
{
  if (m_pNavigator)
    m_pNavigator->slotReloadTree();
}

void khcMainView::slotZoomOut()
{
  if (m_vView->supportsInterface("IDL:Browser/MagnifyingExtension:1.0"))
    {
      CORBA::Object_var obj = m_vView->getInterface("IDL:Browser/MagnifyingExtension:1.0");
      Browser::MagnifyingExtension_var magExt = Browser::MagnifyingExtension::_narrow(obj);
      magExt->zoomOut();

      if (!CORBA::is_nil(m_vToolBar))
	{
	  m_vToolBar->setItemEnabled(TB_ZOOMIN, magExt->canZoomIn());
	  m_vToolBar->setItemEnabled(TB_ZOOMOUT, magExt->canZoomOut());
	}

      if (!CORBA::is_nil(m_vMenuView))
	{
	  m_vMenuView->setItemEnabled(MVIEW_ZOOMIN, magExt->canZoomIn());
	  m_vMenuView->setItemEnabled(MVIEW_ZOOMOUT, magExt->canZoomOut());
	}
    }
}

void khcMainView::slotZoomIn()
{
  if (m_vView->supportsInterface("IDL:Browser/MagnifyingExtension:1.0"))
  {
    CORBA::Object_var obj = m_vView->getInterface("IDL:Browser/MagnifyingExtension:1.0");
    Browser::MagnifyingExtension_var magExt = Browser::MagnifyingExtension::_narrow(obj);
    magExt->zoomIn();

    if (!CORBA::is_nil(m_vToolBar))
      {
	m_vToolBar->setItemEnabled(TB_ZOOMIN, magExt->canZoomIn());
	m_vToolBar->setItemEnabled(TB_ZOOMOUT, magExt->canZoomOut());
      }
    
    if (!CORBA::is_nil(m_vMenuView))
      {
	m_vMenuView->setItemEnabled(MVIEW_ZOOMIN, magExt->canZoomIn());
	m_vMenuView->setItemEnabled(MVIEW_ZOOMOUT, magExt->canZoomOut());
      }
  }
}

void khcMainView::slotPrint()
{
  if (m_vView->supportsInterface("IDL:Browser/PrintingExtension:1.0"))
    {
      CORBA::Object_var obj = m_vView->getInterface("IDL:Browser/PrintingExtension:1.0");
      Browser::PrintingExtension_var printExt = Browser::PrintingExtension::_narrow(obj);
      printExt->print();
    }
}

void khcMainView::slotIntroduction()
{
  // show intro page
  QString url = "file:" + locate("html", "default/khelpcenter/main.html");
  open(url, false); 
}

void khcMainView::slotForward()
{
  // history forward
  khcHistoryItem *hitem = history.next();
  if (hitem)
    open(hitem->url(), true, hitem->xOffset(), hitem->yOffset());
}

void khcMainView::slotBack()
{
  // history back
  khcHistoryItem *hitem = history.prev();
  if(hitem)
      open(hitem->url(), true, hitem->xOffset(), hitem->yOffset());
}

void khcMainView::slotBookmark()
{
  // TODO
}

void khcMainView::slotShowNavigator()
{
  if (!m_showNavigator)
    {
      enableNavigator(true);
      m_showNavigator = true;
      
      if (!CORBA::is_nil(m_vToolBar))
	{
	  OpenPartsUI::Pixmap_var pix = OPUIUtils::convertPixmap(*KPixmapCache::toolbarPixmap("shownavigator.xpm"));
	  CORBA::WString_var text = Q2C(i18n("Navigator"));
  	  m_vToolBar->setButtonPixmap(TB_NAVIGATOR, pix); 
	}
    }
  else
    {
      enableNavigator(false);
      m_showNavigator = false;
      
      if (!CORBA::is_nil(m_vToolBar))
	{
	  OpenPartsUI::Pixmap_var pix = OPUIUtils::convertPixmap(*KPixmapCache::toolbarPixmap("hidenavigator.xpm"));
	  CORBA::WString_var text = Q2C(i18n("Navigator"));
	  m_vToolBar->setButtonPixmap(TB_NAVIGATOR, pix); 
	}
    }

  if (!CORBA::is_nil(m_vMenuOptions))
    m_vMenuOptions->setItemChecked(MOPTIONS_SHOWNAVIGATOR, m_showNavigator);
}

void khcMainView::slotShowMenubar()
{
  if (CORBA::is_nil(m_vMenuBar))
    return;
  
  m_vMenuBar->enable(OpenPartsUI::Toggle);
  m_showMenuBar = m_vMenuBar->isVisible();
  m_vMenuOptions->setItemChecked(MOPTIONS_SHOWMENUBAR, m_showMenuBar);
}

void khcMainView::slotShowToolbar()
{
  if (CORBA::is_nil(m_vToolBar))
    return;

  m_vToolBar->enable(OpenPartsUI::Toggle);
  m_showToolBar = m_vToolBar->isVisible();
  m_vMenuOptions->setItemChecked(MOPTIONS_SHOWTOOLBAR, m_showToolBar);
}

void khcMainView::slotShowLocationbar()
{
  if (CORBA::is_nil(m_vLocationBar))
    return;

  m_vLocationBar->enable(OpenPartsUI::Toggle);
  m_showLocationBar = m_vLocationBar->isVisible();
  m_vMenuOptions->setItemChecked(MOPTIONS_SHOWLOCATIONBAR, m_showLocationBar);
}

void khcMainView::slotShowStatusbar()
{
  if (CORBA::is_nil(m_vStatusBar))
    return;
  
  m_vStatusBar->enable(OpenPartsUI::Toggle);
  m_showStatusBar = m_vStatusBar->isVisible();
  m_vMenuOptions->setItemChecked(MOPTIONS_SHOWSTATUSBAR, m_showStatusBar);
}

void khcMainView::slotSettings()
{
  // TODO: start kcmkhelpcenter
}

void khcMainView::slotHelpContents()
{
  kapp->invokeHTMLHelp("khelpcenter/index.html", "");
}

void khcMainView::slotHelpAbout()
{
  KMessageBox::about(0L,
		     i18n("KDE Helpcenter v%1\n\n"
			  "Copyright (c) 1998/99 Matthias Elter <me@kde.org>\n\n"
			  "Additional credits:\n"
			  "René Beutler <rbeutler@g26.ethz.ch>: kassistant, kcmhelpcenter, kwid,konitemhelp\n"
			  "Martin Jones <mjones@kde.org> Some code is based on kdehelp written 1997 by Martin.\n"
			  "The Konqueror team. I have shamelessly :) ripped code and ideas from Konqy.\n").arg(HELPCENTER_VERSION));
}

void khcMainView::slotHistoryFillBack()
{
  // fill back-button popup menu
  if (CORBA::is_nil(m_vHistoryBackMenu))
    return;

  m_vHistoryBackMenu->clear();
  
  QList<khcHistoryItem> list = history.backList();
  khcHistoryItem *item = list.first();
  
  while (item)
    {
      const CORBA::WChar *text;
      QString url = item->url(); 
      m_vHistoryBackMenu->insertItem7((text = Q2C(url)), -1, -1);
      item = list.next();
    }
}

void khcMainView::slotHistoryFillForward()
{
  // fill forward-button popup menu
  if (CORBA::is_nil(m_vHistoryForwardMenu))
    return;

  m_vHistoryForwardMenu->clear();
  
  QList<khcHistoryItem> list = history.forwardList();
  khcHistoryItem *item = list.first();
  
  while (item)
    {
      const CORBA::WChar *text;
      QString url = item->url(); 
      m_vHistoryForwardMenu->insertItem7((text = Q2C(url)), -1, -1);
      item = list.next();
    }
}

void khcMainView::slotHistoryBackActivated(CORBA::Long id)
{
  // back-button popupmenu-entry selected
  int steps = m_vHistoryBackMenu->indexOf(id) + 1;
  khcHistoryItem *item = history.back(steps);
  
  if(item)
    open(item->url(), true, item->xOffset(), item->yOffset());
}

void khcMainView::slotHistoryForwardActivated(CORBA::Long id)
{
  // forward-button popupmenu-entry selected
  int steps = m_vHistoryForwardMenu->indexOf(id) + 1;
  khcHistoryItem *item = history.forward(steps);
  
  if(item)
    open(item->url(), true, item->xOffset(), item->yOffset());
}

void khcMainView::slotMenuEditAboutToShow()
{
  // recreate the edit menu every time its about to show (the child view might have changed)
  createEditMenu();
}

void khcMainView::slotMenuViewAboutToShow()
{
  // recreate the view menu every time its about to show (the child view might have changed)
  createViewMenu();
}

void khcMainView::slotURLEntered(const CORBA::WChar *_url)
{
  QString url = C2Q(_url);
  
  if (url.isEmpty()) return;
  
  open(url.ascii(), false);
  m_vLocationBar->setCurrentComboItem(TB_URL, 0);
}

void khcMainView::slotCheckHistory()
{
  // can we go back/foreward?
  bool canGoBack = history.hasPrev();
  bool canGoForward = history.hasNext();
  
  if (!CORBA::is_nil(m_vToolBar))
    {
      m_vToolBar->setItemEnabled(TB_BACK, canGoBack);
      m_vToolBar->setItemEnabled(TB_FORWARD, canGoForward);
    }

 if (!CORBA::is_nil(m_vToolBar))
   {
     m_vMenuGo->setItemEnabled(MGO_BACK, canGoBack);
     m_vMenuGo->setItemEnabled(MGO_FORWARD, canGoForward);
   }
}

void khcMainView::slotSetBusy(bool busy)
{
  // (de)activate stop button
  if (CORBA::is_nil(m_vToolBar))
    return;
  
  m_vToolBar->setItemEnabled(TB_STOP, busy);
}
   
void khcMainView::setStatusBarText(const CORBA::WChar *_text)
{
  if (!CORBA::is_nil(m_vStatusBar))
    m_vStatusBar->changeItem(_text, 1);
}

void khcMainView::setLocationBarURL(OpenParts::Id, const char *_url)
{
  CORBA::WString_var wurl = Q2C(QString(_url));
    
  if (!CORBA::is_nil(m_vLocationBar))
    m_vLocationBar->changeComboItem(TB_URL, wurl, 0);

}

void khcMainView::createNewWindow(const char *url)
{
  khcMainWindow *m_pWin = new khcMainWindow(url);
  m_pWin->show();
}

void khcMainView::slotURLStarted(OpenParts::Id , const char *)
{
  // child view started loading a url
  slotSetBusy(true);
}

void khcMainView::slotURLCompleted(OpenParts::Id )
{
  // child view completed loading a url
  checkExtensions();
  slotSetBusy(false);
}

void khcMainView::openURL(const Browser::URLRequest &urlRequest)
{
  open(urlRequest.url, urlRequest.reload, urlRequest.xOffset, urlRequest.yOffset);
}

void khcMainView::slotSetURL(const QString& _url)
{
  // the navigator connects to this slot
  open(_url, false, 0, 0);
}

void khcMainView::open(const char* url, CORBA::Boolean reload, CORBA::Long xoffset, CORBA::Long yoffset)
{
  kdebug(KDEBUG_INFO,1400,"khcMainView::open()");

  slotStop();
  
  if (! reload)
    {
      khcHistoryItem *hitem = new khcHistoryItem(url, xoffset, yoffset);
      history.append(hitem);
    }
  
  Browser::EventOpenURL eventURL;
  eventURL.url = CORBA::string_dup(url);
  eventURL.reload = reload;
  eventURL.xOffset = xoffset;
  eventURL.yOffset = yoffset;
  kdebug(KDEBUG_INFO,1400,"khcMainView->EMIT_EVENT(m_vView, Browser::eventOpenURL, eventURL)");
  EMIT_EVENT(m_vView, Browser::eventOpenURL, eventURL);

  slotCheckHistory();
}

void khcMainView::connectView()
{
  // connect child-view signals

  if(CORBA::is_nil(m_vView))
    return;

  try
    {
      m_vView->connect("openURL", this, "openURL");
    }
  catch ( ... )
    {
      kdebug(KDEBUG_WARN,1400,"WARNING: view does not know signal ""openURL"" ");
    }
  try
    {
      m_vView->connect("started", this, "slotURLStarted");
    }
  catch ( ... )
    {
      kdebug(KDEBUG_WARN,1400,"WARNING: view does not know signal ""started"" ");
    }
  try
    {
      m_vView->connect("completed", this, "slotURLCompleted");
    }
  catch ( ... )
    {
      kdebug(KDEBUG_WARN,1400,"WARNING: view does not know signal ""completed"" ");
    }
  try
    {
      m_vView->connect("setStatusBarText", this, "setStatusBarText");
    }
  catch ( ... )
    {
      kdebug(KDEBUG_WARN,1400,"WARNING: view does not know signal ""setStatusBarText"" ");
    }
    try
    {
      m_vView->connect("setLocationBarURL", this, "setLocationBarURL");
    }
  catch ( ... )
    {
      kdebug(KDEBUG_WARN,1400,"WARNING: view does not know signal ""setLocationBarURL"" ");
    }
  
  try
    {
      m_vView->connect("createNewWindow", this, "createNewWindow");
    }
  catch ( ... )
    {
      kdebug(KDEBUG_WARN,1400,"WARNING: view does not know signal ""createNewWindow"" ");
    }
  
}

#include "khc_mainview.moc"
