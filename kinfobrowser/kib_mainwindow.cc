/*
 *  kib_mainwindow.cc - part of KInfoBrowser
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

#include "kib_mainwindow.h"
#include "kib_view.h"
#include "version.h"

#include <qkeycode.h>
#include <qmsgbox.h>
#include <qpopupmenu.h>

#include <kapp.h>
#include <kiconloader.h>
#include <kstdaccel.h>
#include <kcursor.h>
#include <klocale.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <kdebug.h>
#include <kfiledialog.h>

#include <opFrame.h>
#include <opUIUtils.h>
#include <kconfig.h>
#include <kmenubar.h>

kibMainWindow::kibMainWindow(const QString& url)
{
    kdebug(KDEBUG_INFO,1400,"kibMainWindow::kibMainWindow()");
    setCaption(i18n("KDE InfoBrowser"));
    resize(800, 580);
    setMinimumSize(200, 100);
    
    // setup UI
    setupView();
    setupMenuBar();
    setupToolBar();
    setupLocationBar();
    setupStatusBar();

    // read settings
    slotReadSettings();

    //open url
    if (!url || url.isEmpty())
      slotIndex();
    else
      openURL(url, true);
}

kibMainWindow::~kibMainWindow()
{
  kdebug(KDEBUG_INFO,1400,"kibMainWindow::~kibMainWindow()");

  cleanUp();
  slotSaveSettings();
  delete m_pFrame;
}

OPMainWindowIf* kibMainWindow::interface()
{
  if ( m_pInterface == 0L )
    {    
      m_pkibInterface = new kibMainWindowIf( this );
      m_pInterface = m_pkibInterface;
    }
  return m_pInterface;
}

kibMainWindowIf* kibMainWindow::kibInterface()
{
  if ( m_pInterface == 0L )
    {
      m_pkibInterface = new kibMainWindowIf( this );
      m_pInterface = m_pkibInterface;
    }
  return m_pkibInterface;
}

void kibMainWindow::cleanUp()
{
  kdebug(KDEBUG_INFO,1400,"void kibMainWindow::cleanUp()");
 
  m_vView = 0L;

  // Release the view component. This will delete the component.
  if (m_pFrame)
    m_pFrame->detach();
}

void kibMainWindow::setupView()
{
    kdebug(KDEBUG_INFO,1400,"kibMainWindow::setupView()");
   
    m_pFrame = new OPFrame(this);
    CHECK_PTR(m_pFrame);
  
    setView(m_pFrame, true);
    m_pView = new kibView;
    m_vView = KInfoBrowser::View::_duplicate(m_pView);
  
    m_vView->setMainWindow(kibInterface());
    connectView();
    m_pFrame->attach(m_vView);
   
    kdebug(KDEBUG_INFO,1400,"m_pFrame->attach(m_vView);");

    toolBar(0)->setItemEnabled(TB_ZOOMIN, m_vView->canZoomIn());
    toolBar(0)->setItemEnabled(TB_ZOOMOUT, m_vView->canZoomOut());
}

void kibMainWindow::setupMenuBar()
{
    kdebug(KDEBUG_INFO,1400,"kibMainWindow::setupMenuBar()");
    KStdAccel stdAccel;
  
    // file menu
    m_pFileMenu = new QPopupMenu;
    CHECK_PTR(m_pFileMenu);

    m_pFileMenu->insertItem(i18n("Open &new window"), this, SLOT(slotNewBrowser()), stdAccel.openNew());
    m_pFileMenu->insertSeparator();
    m_pFileMenu->insertItem(i18n("&Reload"), this, SLOT(slotReload()), Key_F5);
    m_pFileMenu->insertItem(i18n("&Print"), this, SLOT(slotPrint()), stdAccel.print());
    m_pFileMenu->insertSeparator();
    m_pFileMenu->insertItem(i18n("&Quit"), this, SLOT(slotQuit()), stdAccel.quit());
  
    // edit menu
    m_pEditMenu = new QPopupMenu;
    CHECK_PTR(m_pEditMenu);

    idCopy = m_pEditMenu->insertItem(i18n("&Copy"), this, SLOT(slotCopy()), stdAccel.copy());
    m_pEditMenu->insertSeparator();
    m_pEditMenu->insertItem(i18n("&Find..."), this, SLOT(slotFind()), stdAccel.find());
     m_pEditMenu->insertItem(i18n("Find &next"), this, SLOT(slotFindNext()), Key_F3);
  
    // goto menu
    m_pGotoMenu = new QPopupMenu;
    CHECK_PTR(m_pGotoMenu);

    idBack = m_pGotoMenu->insertItem(i18n("&Back"), this, SLOT(slotBack()));
    idForward = m_pGotoMenu->insertItem(i18n("&Forward"), this, SLOT(slotForward()));
    m_pGotoMenu->insertSeparator();
    m_pGotoMenu->insertItem(i18n("&Index"), this, SLOT(slotIndex()));
  
    // view menu
    m_pViewMenu = new QPopupMenu;
    CHECK_PTR(m_pViewMenu);

    idMagPlus = m_pViewMenu->insertItem(i18n("Zoom &in"), this, SLOT(slotMagPlus()));
    idMagMinus = m_pViewMenu->insertItem(i18n("Zoom &out"), this, SLOT(slotMagMinus()));
    m_pViewMenu->insertSeparator();
    m_pViewMenu->insertItem(i18n("&Reload"), this, SLOT(slotReload()), Key_F5);
  
    // bookmark menu
    m_pBookmarkMenu = new QPopupMenu;
    CHECK_PTR(m_pBookmarkMenu);

    /*
    connect(m_pBookmarkMenu, SIGNAL(activated(int)), this, SLOT(slotBookmarkSelected(int)));
    connect(m_pBookmarkMenu, SIGNAL(highlighted(int)), this, SLOT(slotBookmarkHighlighted(int)));
    */

    // options menu
    m_pOptionsMenu = new QPopupMenu;
    CHECK_PTR(m_pOptionsMenu);
    m_pOptionsMenu->setCheckable(true);

    idToolBar = m_pOptionsMenu->insertItem(i18n("Show &Toolbar"), this, SLOT(slotOptionsToolbar()));
    idLocationBar = m_pOptionsMenu->insertItem(i18n("Show &Location"), this, SLOT(slotOptionsLocationbar()));
    idStatusBar = m_pOptionsMenu->insertItem(i18n("Show Status&bar"), this, SLOT(slotOptionsStatusbar()));
    m_pOptionsMenu->insertSeparator();
    m_pOptionsMenu->insertItem(i18n("&Settings..."), this, SLOT(slotOptionsGeneral()));
   
  
    // help menu
    m_pHelpMenu = kapp->getHelpMenu(true,i18n("KDE InfoBrowser v" + QString(KINFOBROWSER_VERSION) + "\n\n"
			     "(c) 1999 Matthias Elter <me@kde.org>"));
    
    // insert menu's into menubar
    menuBar()->insertItem(i18n("&File"), m_pFileMenu);
    menuBar()->insertItem(i18n("&Edit"), m_pEditMenu);
    menuBar()->insertItem(i18n("&View"), m_pViewMenu);
    menuBar()->insertItem(i18n("&Go"), m_pGotoMenu);
    menuBar()->insertItem(i18n("&Options"), m_pOptionsMenu);
    menuBar()->insertItem(i18n("&Bookmarks"), m_pBookmarkMenu);
    menuBar()->insertSeparator();
    menuBar()->insertItem(i18n("&Help"), m_pHelpMenu);
}

void kibMainWindow::setupToolBar()
{
    kdebug(KDEBUG_INFO,1400,"kibMainWindow::setupToolBar()");
    
    // history popup menus
    m_pHistoryBackMenu = new QPopupMenu;
    CHECK_PTR(m_pHistoryBackMenu);
    connect(m_pHistoryBackMenu, SIGNAL(aboutToShow()), this, SLOT(slotHistoryFillBack()));
    connect(m_pHistoryBackMenu, SIGNAL(activated(int)), this, SLOT(slotHistoryBackActivated(int)));

    m_pHistoryForwardMenu = new QPopupMenu;
    CHECK_PTR(m_pHistoryForwardMenu);
    connect(m_pHistoryForwardMenu, SIGNAL(aboutToShow()), this, SLOT(slotHistoryFillForward()));
    connect(m_pHistoryForwardMenu, SIGNAL(activated(int)), this, SLOT(slotHistoryForwardActivated(int)));
    
    // explicitely instanciate a toolbar, to make sure it gets the height (40) we want
    KToolBar *bar = new KToolBar(this, 0, 40);
    addToolBar(bar, 0);
    toolBar(0)->setIconText(3);

    // insert toolbar buttons    
    toolBar(0)->insertButton(BarIcon("back"), TB_BACK, true, i18n("Back"));
    toolBar(0)->setDelayedPopup(TB_BACK, m_pHistoryBackMenu);
    toolBar(0)->setItemEnabled(TB_BACK, false);
  
    toolBar(0)->insertButton(BarIcon("forward"), TB_FORWARD, true, i18n("Forward"));
    toolBar(0)->setDelayedPopup(TB_FORWARD, m_pHistoryForwardMenu);
    toolBar(0)->setItemEnabled(TB_FORWARD, false);
  
    toolBar(0)->insertButton(BarIcon("reload"), TB_RELOAD, true, i18n("Reload"));
    toolBar(0)->insertButton(BarIcon("stop"), TB_STOP, true, i18n("Stop"));

    toolBar(0)->insertSeparator();
  
    toolBar(0)->insertButton(BarIcon("viewmag+"), TB_ZOOMIN, true, i18n("Zoom in"));
    toolBar(0)->insertButton(BarIcon("viewmag-"), TB_ZOOMOUT, true, i18n("Zoom out"));

    toolBar(0)->insertSeparator();

    toolBar(0)->insertButton(BarIcon("flag"), TB_SETBOOKMARK, true, i18n("Bookmark"));
    toolBar(0)->insertButton(BarIcon("search"), TB_FIND, true, i18n("Find"));
    toolBar(0)->insertButton(BarIcon("fileprint"), TB_PRINT, true, i18n("Print"));

    // connect toolbar
    connect(toolBar(0), SIGNAL(clicked(int)), SLOT(slotToolbarClicked(int)));
}

void kibMainWindow::setupLocationBar()
{
    kdebug(KDEBUG_INFO,1400,"kibMainWindow::setupLocationbar()");

    toolBar(1)->insertLined("", 1, SIGNAL(returnPressed()), this, SLOT(slotLocationEntered()));
    toolBar(1)->setFullWidth(TRUE);
    toolBar(1)->setItemAutoSized(1, TRUE);
    toolBar(1)->enable(KToolBar::Show);
}

void kibMainWindow::setupStatusBar()
{
    kdebug(KDEBUG_INFO,1400,"kibMainWindow::setupStatusbar()");
    statusBar()->insertItem("", 1);
    enableStatusBar(KStatusBar::Show);
}

void kibMainWindow::slotReadSettings()
{
    KConfig *config = KApplication::getKApplication()->getConfig();
    config->setGroup("Appearance");

    // show tool-, location-, statusbar and navigator?
    QString o = config->readEntry("ShowToolBar");
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
    o = config->readEntry("ToolBarPos");
    if (o.isEmpty())
	toolBar(0)->setBarPos(KToolBar::Top);
    else if ("Top" == o) 
	toolBar(0)->setBarPos(KToolBar::Top);
    else if ("Bottom" == o)
	toolBar(0)->setBarPos(KToolBar::Bottom);
    else if ("Left" == o)
	toolBar(0)->setBarPos(KToolBar::Left);
    else if ("Right" == o)
	toolBar(0)->setBarPos(KToolBar::Right);
    else if ("Floating" == o)
	toolBar(0)->setBarPos(KToolBar::Floating);
    else
	toolBar(0)->setBarPos(KToolBar::Top);
  
    // locationbar location
    o = config->readEntry("LocationBarPos");
    if ( o.isEmpty() )
   	toolBar(1)->setBarPos(KToolBar::Top);
    else if ("Top" == o) 
	toolBar(1)->setBarPos(KToolBar::Top);
    else if ("Bottom" == o)
	toolBar(1)->setBarPos(KToolBar::Bottom);
    else if ("Left" == o)
	toolBar(1)->setBarPos(KToolBar::Left);
    else if ("Right" == o)
	toolBar(1)->setBarPos(KToolBar::Right);
    else if ("Floating" == o)
	toolBar(1)->setBarPos(KToolBar::Floating);
    else
	toolBar(1)->setBarPos(KToolBar::Top);

    // set configuration
    if (m_showStatusBar)
	enableStatusBar(KStatusBar::Show);
    else
	enableStatusBar(KStatusBar::Hide);
  
    if (m_showToolBar) 
	enableToolBar(KToolBar::Show, 0);
    else
	enableToolBar(KToolBar::Hide, 0);

    if (m_showLocationBar)
	enableToolBar(KToolBar::Show, 1);
    else
	enableToolBar(KToolBar::Hide, 1);

    // toggle menu items
    if (m_pOptionsMenu)
    {
      m_pOptionsMenu->setItemChecked(idToolBar, m_showToolBar);
      m_pOptionsMenu->setItemChecked(idLocationBar, m_showLocationBar);
      m_pOptionsMenu->setItemChecked(idStatusBar, m_showStatusBar);
    }
}

void kibMainWindow::slotSaveSettings()
{
    KConfig *config = KApplication::getKApplication()->getConfig();

    config->setGroup( "Appearance" );
    config->writeEntry( "ShowToolBar", m_showToolBar ? "Yes" : "No" );
    config->writeEntry( "ShowStatusBar", m_showStatusBar ? "Yes" : "No" );  
    config->writeEntry( "ShowLocationBar", m_showLocationBar ? "Yes" : "No" );

    switch (toolBar(0)->barPos())
    {
    case KToolBar::Top:
	config->writeEntry( "ToolBarPos", "Top");
	break;
    case KToolBar::Bottom:
	config->writeEntry( "ToolBarPos", "Bottom");
	break;
    case KToolBar::Left:
	config->writeEntry( "ToolBarPos", "Left");
	break;
    case KToolBar::Right:
	config->writeEntry( "ToolBarPos", "Right");
	break;
    case KToolBar::Floating:
	config->writeEntry( "ToolBarPos", "Floating");
	break;
    default:
	warning("helpCenter::slotOptionsSave: illegal default in case reached\n");
	break;
    }

    switch (toolBar(1)->barPos())
    {
    case KToolBar::Top:
	config->writeEntry( "LocationBarPos", "Top");
	break;
    case KToolBar::Bottom:
	config->writeEntry( "LocationBarPos", "Bottom");
	break;
    case KToolBar::Left:
	config->writeEntry( "LocationBarPos", "Left");
	break;
    case KToolBar::Right:
	config->writeEntry( "LocationBarPos", "Right");
	break;
    case KToolBar::Floating:
	config->writeEntry( "LocationBarPos", "Floating");
	break;
    default:
	warning("kibMainWindow::slotOptionsSave: illegal default in case reached\n");
	break;
    }
}

void kibMainWindow::slotSetLocation(const QString& _url)
{
    toolBar(1)->setLinedText(1, _url);
}

void kibMainWindow::slotLocationEntered()
{
    openURL(toolBar(1)->getLinedText(1), true );
}

void kibMainWindow::slotURLSelected(const QString& _url, int)
{
    openURL( _url, true );
}

void kibMainWindow::slotToolbarClicked(int item)
{
    switch (item)
    {
    case TB_SETBOOKMARK:
	slotSetBookmark();
	break;
    case TB_PRINT:
	slotPrint();
	break;
    case TB_ZOOMIN:
	slotMagPlus();
	break;
    case TB_FIND:
	slotFind();
	break;
    case TB_ZOOMOUT:
	slotMagMinus();
	break;
    case TB_BACK:
	slotBack();
	break;
    case TB_FORWARD:
	slotForward();
	break;
    case TB_RELOAD:
	slotReload();
	break;
    case TB_STOP:
	slotStopProcessing();
	break;
    }
}

void kibMainWindow::slotNewBrowser()
{
    slotOpenNewBrowser(QString(""));
}

void kibMainWindow::slotOpenNewBrowser(const QString& url)
{
    kibMainWindow *win = new kibMainWindow(url);
    win->resize(size());
    win->show();
}

void kibMainWindow::slotSetStatusText(const QString& text)
{
    statusBar()->changeItem(QString(text), 1);
}

void kibMainWindow::slotSetTitle( const QString& _title )
{
    QString appCaption = "KDE InfoBrowser - ";
    appCaption += _title;
  
    setCaption( appCaption );
}

void kibMainWindow::slotSetURL(QString url)
{
    openURL(url, true);
}

void kibMainWindow::slotSetBookmark()
{
  
}

void kibMainWindow::slotIndex()
{
    QString url = "info:(dir)Top";

    openURL(url, true); 
}

void kibMainWindow::slotOpenFile()
{
    QString fileName = KFileDialog::getOpenFileName();
    if (!fileName.isNull())
    {
	QString url = "file:";
	url += fileName;
	openURL(url, true);
    }
}

void kibMainWindow::slotOptionsToolbar()
{
    if (m_showToolBar)
    {
	enableToolBar(KToolBar::Hide, 0);
	m_showToolBar = false;
    }
    else
    {
	enableToolBar(KToolBar::Show, 0);
	m_showToolBar = true;
    }
  
    m_pOptionsMenu->setItemChecked(idToolBar, m_showToolBar);
}

void kibMainWindow::slotOptionsLocationbar()
{ 
    if (m_showLocationBar)
    {
	enableToolBar(KToolBar::Hide, 1);
	m_showLocationBar = false;
    }
    else
    {
	enableToolBar(KToolBar::Show, 1);
	m_showLocationBar = true;
    }
    m_pOptionsMenu->setItemChecked(idLocationBar, m_showLocationBar); 
}

void kibMainWindow::slotOptionsStatusbar()
{
    m_showStatusBar = !m_showStatusBar;
    m_pOptionsMenu->setItemChecked(idStatusBar, m_showStatusBar); 
    enableStatusBar(KStatusBar::Toggle);
}

void kibMainWindow::slotOptionsGeneral()
{
  
}

void kibMainWindow::slotQuit()
{
    close();
}

void kibMainWindow::openURL(Browser::URLRequest urlRequest)
{
  slotStopProcessing();
  
  khcHistoryItem *hitem = new khcHistoryItem(urlRequest.url, urlRequest.xOffset, urlRequest.yOffset);
  history.append(hitem);

  kdebug(KDEBUG_INFO,1400,"EMIT_EVENT(m_vView, Browser::eventOpenURL, eventURL)");
  EMIT_EVENT(m_vView, Browser::eventOpenURL, urlRequest);
  slotCheckHistory();
}

void kibMainWindow::openURL(const char *_url, bool withHistory, long xOffset, long yOffset)
{
  slotStopProcessing();
    
  Browser::EventOpenURL eventURL;
  kdebug(KDEBUG_INFO,1400,"openURL:" + QString(_url));
  eventURL.url = CORBA::string_dup(_url);
  eventURL.reload = (CORBA::Boolean)false;
  eventURL.xOffset = xOffset;
  eventURL.yOffset = yOffset;

  if (withHistory)
    {
      khcHistoryItem *hitem = new khcHistoryItem(_url, xOffset, yOffset);
      history.append(hitem);
    }

  kdebug(KDEBUG_INFO,1400,"EMIT_EVENT(m_vView, Browser::eventOpenURL, eventURL)");
  EMIT_EVENT(m_vView, Browser::eventOpenURL, eventURL);
  slotCheckHistory();
}

void kibMainWindow::connectView()
{
  try
    {
      m_vView->connect("openURL", kibInterface(), "openURL");
    }
  catch ( ... )
    {
      kdebug(KDEBUG_WARN,1400,"WARNING: view does not know signal ""openURL"" ");
    }
  try
    {
      m_vView->connect("started", kibInterface(), "slotURLStarted");
    }
  catch ( ... )
    {
      kdebug(KDEBUG_WARN,1400,"WARNING: view does not know signal ""started"" ");
    }
  try
    {
      m_vView->connect("completed", kibInterface(), "slotURLCompleted");
    }
  catch ( ... )
    {
      kdebug(KDEBUG_WARN,1400,"WARNING: view does not know signal ""completed"" ");
    }
  try
    {
      m_vView->connect("setStatusBarText", kibInterface(), "setStatusBarText");
    }
  catch ( ... )
    {
      kdebug(KDEBUG_WARN,1400,"WARNING: view does not know signal ""setStatusBarText"" ");
    }
    try
    {
      m_vView->connect("setLocationBarURL", kibInterface(), "setLocationBarURL");
    }
  catch ( ... )
    {
      kdebug(KDEBUG_WARN,1400,"WARNING: view does not know signal ""setLocationBarURL"" ");
    }
  
  try
    {
      m_vView->connect("createNewWindow", kibInterface(), "createNewWindow");
    }
  catch ( ... )
    {
      kdebug(KDEBUG_WARN,1400,"WARNING: view does not know signal ""createNewWindow"" ");
    }
  
}

void kibMainWindow::slotFind()
{
  m_vView->slotFind();
}

void kibMainWindow::slotFindNext()
{
  m_vView->slotFindNext();
}

void kibMainWindow::slotReload()
{
  Browser::EventOpenURL eventURL;
  eventURL.url = m_vView->url();
  eventURL.reload = (CORBA::Boolean)true;
  eventURL.xOffset = m_vView->xOffset();
  eventURL.yOffset = m_vView->yOffset();
  EMIT_EVENT( m_vView, Browser::eventOpenURL, eventURL );
  kdebug(0, 1400, "EMIT_EVENT( m_vView, Browser::eventOpenURL, eventURL );");
}

void kibMainWindow::slotCopy()
{
  
}

void kibMainWindow::slotPrint()
{
  m_vView->print();
}

void kibMainWindow::slotStopProcessing()
{
  m_vView->stop();
}

void kibMainWindow::slotMagMinus()
{
  m_vView->zoomOut();
  toolBar(0)->setItemEnabled(TB_ZOOMIN,  m_vView->canZoomIn());
  toolBar(0)->setItemEnabled(TB_ZOOMOUT,  m_vView->canZoomOut());
}

void kibMainWindow::slotMagPlus()
{
  m_vView->zoomIn();
  toolBar(0)->setItemEnabled(TB_ZOOMIN, m_vView->canZoomIn());
  toolBar(0)->setItemEnabled(TB_ZOOMOUT, m_vView->canZoomOut());
}

void kibMainWindow::slotCheckHistory()
{
  toolBar(0)->setItemEnabled(TB_BACK, history.hasPrev());
  toolBar(0)->setItemEnabled(TB_FORWARD, history.hasNext());
}

void kibMainWindow::slotForward()
{
  khcHistoryItem *hitem = history.next();
  if (hitem)
    openURL(hitem->url(), false, hitem->xOffset(), hitem->yOffset());
}

void kibMainWindow::slotBack()
{
  khcHistoryItem *hitem = history.prev();
  if(hitem)
      openURL(hitem->url(), false, hitem->xOffset(), hitem->yOffset());
}

void kibMainWindow::slotHistoryFillBack()
{
  m_pHistoryBackMenu->clear();
  
  QList<khcHistoryItem> list = history.backList();
  khcHistoryItem *item = list.first();
  
  while (item)
    {
      QString url = item->url();
      m_pHistoryBackMenu->insertItem(url, this, 0);
      item = list.next();
    }
}

void kibMainWindow::slotHistoryFillForward()
{
  m_pHistoryForwardMenu->clear();
  
  QList<khcHistoryItem> list = history.forwardList();
  khcHistoryItem *item = list.first();
  
  while (item)
    {
      QString url = item->url();
      m_pHistoryForwardMenu->insertItem(url, this, 0);
      item = list.next();
    }
}

void kibMainWindow::slotHistoryBackActivated(int id)
{
  int steps = m_pHistoryBackMenu->indexOf(id) + 1;
  khcHistoryItem *item = history.back(steps);
        
  if(item)
    openURL(item->url(), false, item->xOffset(), item->yOffset());
}

void kibMainWindow::slotHistoryForwardActivated(int id)
{
  int steps = m_pHistoryForwardMenu->indexOf(id) + 1;
  khcHistoryItem *item = history.forward(steps);
  
  if(item)
    openURL(item->url(), false, item->xOffset(), item->yOffset());
}

void kibMainWindow::slotSetBusy(bool busy)
{
  toolBar(0)->setItemEnabled(TB_STOP, busy);
}

kibMainWindowIf::kibMainWindowIf(kibMainWindow* _main) :
  OPMainWindowIf( _main )
{
  ADD_INTERFACE("IDL:KInfoBrowser/MainWindow:1.0" );

  m_pkibMainWindow = _main;
}

kibMainWindowIf::~kibMainWindowIf()
{
  cleanUp();
}

void kibMainWindowIf::setStatusBarText(const CORBA::WChar *_text)
{
  m_pkibMainWindow->slotSetStatusText(C2Q(_text));
  kdebug(0, 1400, "void kibMainWindowIf::setStatusBarText(const char *_text)");
}

void kibMainWindowIf::setLocationBarURL(OpenParts::Id , const char *_url)
{
  m_pkibMainWindow->slotSetLocation(_url);
  m_pkibMainWindow->slotSetTitle(_url);
  kdebug(0, 1400, "void kibMainWindowIf::setLocationBarURL(const char *_url)");
}

void kibMainWindowIf::createNewWindow(const char *url)
{
  m_pkibMainWindow->slotOpenNewBrowser(url);
  kdebug(0, 1400, "void kibMainWindowIf::createNewWindow(const char *url)");
}

void kibMainWindowIf::slotURLStarted(OpenParts::Id , const char *)
{
  m_pkibMainWindow->slotSetBusy(true);
  kdebug(0, 1400, "void kibMainWindowIf::slotURLStarted(const char *url)");
}

void kibMainWindowIf::slotURLCompleted(OpenParts::Id )
{
  m_pkibMainWindow->slotSetBusy(false);
  kdebug(0, 1400, "void kibMainWindowIf::slotURLCompleted()");
}

void kibMainWindowIf::openURL(const Browser::URLRequest &url)
{
  m_pkibMainWindow->openURL(url);
  kdebug(0, 1400, "void kibMainWindowIf::openURL(const Browser::URLRequest &url)");
}

void kibMainWindowIf::open(const char* url, CORBA::Boolean reload, CORBA::Long xoffset, CORBA::Long yoffset)
{
  Browser::EventOpenURL eventURL;
  eventURL.url = CORBA::string_dup(url);
  eventURL.reload = reload;
  eventURL.xOffset = xoffset;
  eventURL.yOffset = yoffset;
  openURL(eventURL);
}

void kibMainWindowIf::print()
{
  m_pkibMainWindow->slotPrint();
}

void kibMainWindowIf::zoomIn()
{
  m_pkibMainWindow->slotMagPlus();
}

void kibMainWindowIf::zoomOut()
{
  m_pkibMainWindow->slotMagMinus();
}

void kibMainWindowIf::reload()
{
  m_pkibMainWindow->slotReload();
}

void kibMainWindowIf::openFile()
{
  m_pkibMainWindow->slotOpenFile();
}

void kibMainWindowIf::index()
{
  m_pkibMainWindow->slotIndex();
}

void kibMainWindowIf::back()
{
  m_pkibMainWindow->slotBack();
}

void kibMainWindowIf::forward()
{
  m_pkibMainWindow->slotForward();
}

void kibMainWindowIf::bookmark()
{
  m_pkibMainWindow->slotSetBookmark();
}

void kibMainWindowIf::options()
{
  m_pkibMainWindow->slotOptionsGeneral();
} 

#include "kib_mainwindow.moc"
