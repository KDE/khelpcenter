/*
 *  kmb_mainwindow.cc - part of the KDE Help Center
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

#include "kmb_mainwindow.h"
#include "kmb_view.h"
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
#include <kmenubar.h>
#include <kdebug.h>
#include <kfiledialog.h>

#include <opFrame.h>
#include <opUIUtils.h>
#include <kconfig.h>

kmbMainWindow::kmbMainWindow(const QString& url)
{
    kdebug(KDEBUG_INFO,1400,"kmbMainWindow::kmbMainWindow()");
    setCaption(i18n("KDE ManBrowser"));
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

kmbMainWindow::~kmbMainWindow()
{
  kdebug(KDEBUG_INFO,1400,"kmbMainWindow::~kmbMainWindow()");

  cleanUp();
  slotSaveSettings();
  delete m_pFrame;
}

OPMainWindowIf* kmbMainWindow::interface()
{
  if ( m_pInterface == 0L )
    {    
      m_pkmbInterface = new kmbMainWindowIf( this );
      m_pInterface = m_pkmbInterface;
    }
  return m_pInterface;
}

kmbMainWindowIf* kmbMainWindow::kmbInterface()
{
  if ( m_pInterface == 0L )
    {
      m_pkmbInterface = new kmbMainWindowIf( this );
      m_pInterface = m_pkmbInterface;
    }
  return m_pkmbInterface;
}

void kmbMainWindow::cleanUp()
{
  kdebug(KDEBUG_INFO,1400,"void kmbMainWindow::cleanUp()");
 
  m_vView = 0L;

  // Release the view component. This will delete the component.
  if (m_pFrame)
    m_pFrame->detach();
}

void kmbMainWindow::setupView()
{
    kdebug(KDEBUG_INFO,1400,"kmbMainWindow::setupView()");
   
    m_pFrame = new OPFrame(this);
    CHECK_PTR(m_pFrame);
  
    setView(m_pFrame, true);
    m_pView = new kmbView;
    m_vView = KManBrowser::View::_duplicate(m_pView);
  
    m_vView->setMainWindow(kmbInterface());
    connectView();
    m_pFrame->attach(m_vView);
   
    kdebug(KDEBUG_INFO,1400,"m_pFrame->attach(m_vView);");

    toolBar(0)->setItemEnabled(TB_ZOOMIN, m_vView->canZoomIn());
    toolBar(0)->setItemEnabled(TB_ZOOMOUT, m_vView->canZoomOut());
}

void kmbMainWindow::setupMenuBar()
{
    kdebug(KDEBUG_INFO,1400,"kmbMainWindow::setupMenuBar()");
    KStdAccel stdAccel;
  
    // file menu
    m_pFileMenu = new QPopupMenu;
    CHECK_PTR(m_pFileMenu);

    m_pFileMenu->insertItem(i18n("Open &new window"), this, SLOT(slotNewBrowser()), stdAccel.openNew());
    m_pFileMenu->insertSeparator();
    m_pFileMenu->insertItem(i18n("&Open File..."), this, SLOT(slotOpenFile()), stdAccel.open());
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
    m_pHelpMenu = kapp->getHelpMenu(true,i18n("KDE ManBrowser v" + QString(KMANBROWSER_VERSION) + "\n\n"
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

void kmbMainWindow::setupToolBar()
{
    kdebug(KDEBUG_INFO,1400,"kmbMainWindow::setupToolBar()");
    
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
    toolBar(0)->insertButton(Icon("back.xpm"), TB_BACK, true, i18n("Back"));
    toolBar(0)->setDelayedPopup(TB_BACK, m_pHistoryBackMenu);
    toolBar(0)->setItemEnabled(TB_BACK, false);
  
    toolBar(0)->insertButton(Icon("forward.xpm"), TB_FORWARD, true, i18n("Forward"));
    toolBar(0)->setDelayedPopup(TB_FORWARD, m_pHistoryForwardMenu);
    toolBar(0)->setItemEnabled(TB_FORWARD, false);
  
    toolBar(0)->insertButton(Icon("reload.xpm"), TB_RELOAD, true, i18n("Reload"));
    toolBar(0)->insertButton(Icon("stop.xpm"), TB_STOP, true, i18n("Stop"));

    toolBar(0)->insertSeparator();
  
    toolBar(0)->insertButton(Icon("viewmag+.xpm"), TB_ZOOMIN, true, i18n("Zoom in"));
    toolBar(0)->insertButton(Icon("viewmag-.xpm"), TB_ZOOMOUT, true, i18n("Zoom out"));

    toolBar(0)->insertSeparator();

    toolBar(0)->insertButton(Icon("flag.xpm"), TB_SETBOOKMARK, true, i18n("Bookmark"));
    toolBar(0)->insertButton(Icon("search.xpm"), TB_FIND, true, i18n("Find"));
    toolBar(0)->insertButton(Icon("fileprint.xpm"), TB_PRINT, true, i18n("Print"));

    // connect toolbar
    connect(toolBar(0), SIGNAL(clicked(int)), SLOT(slotToolbarClicked(int)));
}

void kmbMainWindow::setupLocationBar()
{
    kdebug(KDEBUG_INFO,1400,"kmbMainWindow::setupLocationbar()");

    toolBar(1)->insertLined("", 1, SIGNAL(returnPressed()), this, SLOT(slotLocationEntered()));
    toolBar(1)->setFullWidth(TRUE);
    toolBar(1)->setItemAutoSized(1, TRUE);
    toolBar(1)->enable(KToolBar::Show);
}

void kmbMainWindow::setupStatusBar()
{
    kdebug(KDEBUG_INFO,1400,"kmbMainWindow::setupStatusbar()");
    statusBar()->insertItem("", 1);
    enableStatusBar(KStatusBar::Show);
}

void kmbMainWindow::slotReadSettings()
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

void kmbMainWindow::slotSaveSettings()
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
	warning("kmbMainWindow::slotOptionsSave: illegal default in case reached\n");
	break;
    }
}

void kmbMainWindow::slotSetLocation(const QString& _url)
{
    toolBar(1)->setLinedText(1, _url);
}

void kmbMainWindow::slotLocationEntered()
{
    openURL(toolBar(1)->getLinedText(1), true );
}

void kmbMainWindow::slotURLSelected(const QString& _url, int)
{
    openURL( _url, true );
}

void kmbMainWindow::slotToolbarClicked(int item)
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

void kmbMainWindow::slotNewBrowser()
{
    slotOpenNewBrowser(QString(""));
}

void kmbMainWindow::slotOpenNewBrowser(const QString& url)
{
    kmbMainWindow *win = new kmbMainWindow(url);
    win->resize(size());
    win->show();
}

void kmbMainWindow::slotSetStatusText(const QString& text)
{
    statusBar()->changeItem(QString(text), 1);
}

void kmbMainWindow::slotSetTitle( const QString& _title )
{
    QString appCaption = "KDE ManBrowser - ";
    appCaption += _title;
  
    setCaption( appCaption );
}

void kmbMainWindow::slotSetURL(QString url)
{
    openURL(url, true);
}

void kmbMainWindow::slotSetBookmark()
{
  
}

void kmbMainWindow::slotIndex()
{
    QString url = "man:(index)";

    openURL(url, true); 
}

void kmbMainWindow::slotOpenFile()
{
    QString fileName = KFileDialog::getOpenFileName();
    if (!fileName.isNull())
    {
	QString url = "file:";
	url += fileName;
	openURL(url, true);
    }
}

void kmbMainWindow::slotOptionsToolbar()
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

void kmbMainWindow::slotOptionsLocationbar()
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

void kmbMainWindow::slotOptionsStatusbar()
{
    m_showStatusBar = !m_showStatusBar;
    m_pOptionsMenu->setItemChecked(idStatusBar, m_showStatusBar); 
    enableStatusBar(KStatusBar::Toggle);
}

void kmbMainWindow::slotOptionsGeneral()
{
  
}

void kmbMainWindow::slotQuit()
{
    close();
}

void kmbMainWindow::openURL(Browser::URLRequest urlRequest)
{
  slotStopProcessing();
  
  khcHistoryItem *hitem = new khcHistoryItem(urlRequest.url, urlRequest.xOffset, urlRequest.yOffset);
  history.append(hitem);

  kdebug(KDEBUG_INFO,1400,"EMIT_EVENT(m_vView, Browser::eventOpenURL, eventURL)");
  EMIT_EVENT(m_vView, Browser::eventOpenURL, urlRequest);
  slotCheckHistory();
}

void kmbMainWindow::openURL(const char *_url, bool withHistory, long xOffset, long yOffset)
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

void kmbMainWindow::connectView()
{
  try
    {
      m_vView->connect("openURL", kmbInterface(), "openURL");
    }
  catch ( ... )
    {
      kdebug(KDEBUG_WARN,1400,"WARNING: view does not know signal ""openURL"" ");
    }
  try
    {
      m_vView->connect("started", kmbInterface(), "slotURLStarted");
    }
  catch ( ... )
    {
      kdebug(KDEBUG_WARN,1400,"WARNING: view does not know signal ""started"" ");
    }
  try
    {
      m_vView->connect("completed", kmbInterface(), "slotURLCompleted");
    }
  catch ( ... )
    {
      kdebug(KDEBUG_WARN,1400,"WARNING: view does not know signal ""completed"" ");
    }
  try
    {
      m_vView->connect("setStatusBarText", kmbInterface(), "setStatusBarText");
    }
  catch ( ... )
    {
      kdebug(KDEBUG_WARN,1400,"WARNING: view does not know signal ""setStatusBarText"" ");
    }
    try
    {
      m_vView->connect("setLocationBarURL", kmbInterface(), "setLocationBarURL");
    }
  catch ( ... )
    {
      kdebug(KDEBUG_WARN,1400,"WARNING: view does not know signal ""setLocationBarURL"" ");
    }
  
  try
    {
      m_vView->connect("createNewWindow", kmbInterface(), "createNewWindow");
    }
  catch ( ... )
    {
      kdebug(KDEBUG_WARN,1400,"WARNING: view does not know signal ""createNewWindow"" ");
    }
  
}

void kmbMainWindow::slotFind()
{
  
}

void kmbMainWindow::slotFindNext()
{


}

void kmbMainWindow::slotReload()
{
  Browser::EventOpenURL eventURL;
  eventURL.url = m_vView->url();
  eventURL.reload = (CORBA::Boolean)true;
  eventURL.xOffset = m_vView->xOffset();
  eventURL.yOffset = m_vView->yOffset();
  EMIT_EVENT( m_vView, Browser::eventOpenURL, eventURL );
  kdebug(0, 1400, "EMIT_EVENT( m_vView, Browser::eventOpenURL, eventURL );");
}

void kmbMainWindow::slotCopy()
{
  
}

void kmbMainWindow::slotPrint()
{
  m_vView->print();
}

void kmbMainWindow::slotStopProcessing()
{
  m_vView->stop();
}

void kmbMainWindow::slotMagMinus()
{
  m_vView->zoomOut();
  toolBar(0)->setItemEnabled(TB_ZOOMIN,  m_vView->canZoomIn());
  toolBar(0)->setItemEnabled(TB_ZOOMOUT,  m_vView->canZoomOut());
}

void kmbMainWindow::slotMagPlus()
{
  m_vView->zoomIn();
  toolBar(0)->setItemEnabled(TB_ZOOMIN, m_vView->canZoomIn());
  toolBar(0)->setItemEnabled(TB_ZOOMOUT, m_vView->canZoomOut());
}

void kmbMainWindow::slotCheckHistory()
{
  toolBar(0)->setItemEnabled(TB_BACK, history.hasPrev());
  toolBar(0)->setItemEnabled(TB_FORWARD, history.hasNext());
}

void kmbMainWindow::slotForward()
{
  khcHistoryItem *hitem = history.next();
  if (hitem)
    openURL(hitem->url(), false, hitem->xOffset(), hitem->yOffset());
}

void kmbMainWindow::slotBack()
{
  khcHistoryItem *hitem = history.prev();
  if(hitem)
      openURL(hitem->url(), false, hitem->xOffset(), hitem->yOffset());
}

void kmbMainWindow::slotHistoryFillBack()
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

void kmbMainWindow::slotHistoryFillForward()
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

void kmbMainWindow::slotHistoryBackActivated(int id)
{
  int steps = m_pHistoryBackMenu->indexOf(id) + 1;
  khcHistoryItem *item = history.back(steps);
        
  if(item)
    openURL(item->url(), false, item->xOffset(), item->yOffset());
}

void kmbMainWindow::slotHistoryForwardActivated(int id)
{
  int steps = m_pHistoryForwardMenu->indexOf(id) + 1;
  khcHistoryItem *item = history.forward(steps);
  
  if(item)
    openURL(item->url(), false, item->xOffset(), item->yOffset());
}

void kmbMainWindow::slotSetBusy(bool busy)
{
  toolBar(0)->setItemEnabled(TB_STOP, busy);
}

kmbMainWindowIf::kmbMainWindowIf(kmbMainWindow* _main) :
  OPMainWindowIf( _main )
{
  ADD_INTERFACE("IDL:KManBrowser/MainWindow:1.0" );

  m_pkmbMainWindow = _main;
}

kmbMainWindowIf::~kmbMainWindowIf()
{
  cleanUp();
}

void kmbMainWindowIf::setStatusBarText(const CORBA::WChar *_text)
{
  m_pkmbMainWindow->slotSetStatusText(C2Q(_text));
  kdebug(0, 1400, "void kmbMainWindowIf::setStatusBarText(const char *_text)");
}

void kmbMainWindowIf::setLocationBarURL(OpenParts::Id , const char *_url)
{
  m_pkmbMainWindow->slotSetLocation(_url);
  m_pkmbMainWindow->slotSetTitle(_url);
  kdebug(0, 1400, "void kmbMainWindowIf::setLocationBarURL(const char *_url)");
}

void kmbMainWindowIf::createNewWindow(const char *url)
{
  m_pkmbMainWindow->slotOpenNewBrowser(url);
  kdebug(0, 1400, "void kmbMainWindowIf::createNewWindow(const char *url)");
}

void kmbMainWindowIf::slotURLStarted(OpenParts::Id, const char *)
{
  m_pkmbMainWindow->slotSetBusy(true);
  kdebug(0, 1400, "void kmbMainWindowIf::slotURLStarted(const char *url)");
}

void kmbMainWindowIf::slotURLCompleted(OpenParts::Id)
{
  m_pkmbMainWindow->slotSetBusy(false);
  kdebug(0, 1400, "void kmbMainWindowIf::slotURLCompleted()");
}

void kmbMainWindowIf::openURL(const Browser::URLRequest &url)
{
  m_pkmbMainWindow->openURL(url);
  kdebug(0, 1400, "void kmbMainWindowIf::openURL(const Browser::URLRequest &url)");
}

void kmbMainWindowIf::open(const char* url, CORBA::Boolean reload, CORBA::Long xoffset, CORBA::Long yoffset)
{
  Browser::EventOpenURL eventURL;
  eventURL.url = CORBA::string_dup(url);
  eventURL.reload = reload;
  eventURL.xOffset = xoffset;
  eventURL.yOffset = yoffset;
  openURL(eventURL);
}

void kmbMainWindowIf::print()
{
  m_pkmbMainWindow->slotPrint();
}

void kmbMainWindowIf::zoomIn()
{
  m_pkmbMainWindow->slotMagPlus();
}

void kmbMainWindowIf::zoomOut()
{
  m_pkmbMainWindow->slotMagMinus();
}

void kmbMainWindowIf::reload()
{
  m_pkmbMainWindow->slotReload();
}

void kmbMainWindowIf::openFile()
{
  m_pkmbMainWindow->slotOpenFile();
}

void kmbMainWindowIf::index()
{
  m_pkmbMainWindow->slotIndex();
}

void kmbMainWindowIf::back()
{
  m_pkmbMainWindow->slotBack();
}

void kmbMainWindowIf::forward()
{
  m_pkmbMainWindow->slotForward();
}

void kmbMainWindowIf::bookmark()
{
  m_pkmbMainWindow->slotSetBookmark();
}

void kmbMainWindowIf::options()
{
  m_pkmbMainWindow->slotOptionsGeneral();
} 

#include "kmb_mainwindow.moc"
