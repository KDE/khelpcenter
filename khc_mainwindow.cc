/*
 *  khc_mainwindow.cc - part of the KDE Help Center
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
#include "khc_testpart.h"
#include "khc_baseview.h"
#include "khc_htmlview.h"
#include "khc_navigator.h"
#include "version.h"

#include <qkeycode.h>
#include <qmsgbox.h>
#include <qvaluelist.h>
#include <qpopupmenu.h>
#include <qsplitter.h>

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

// static list of khcMainWindow windows:
QList<khcMainWindow> khcMainWindow::helpWindowList;

khcMainWindow::khcMainWindow(const QString& url)
{
    kdebug(KDEBUG_INFO,1400,"khcMainWindow::khcMainWindow()");
    setCaption(i18n("KDE HelpCenter"));
    
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

    // connect navigator
    connect(m_pNavigator , SIGNAL(itemSelected(QString)), this, SLOT(slotSetURL(QString)));
 
    // append current window to helpWindoList
    helpWindowList.setAutoDelete(false);
    helpWindowList.append(this);
    listIndex = helpWindowList.at();

    //open url
    if (!url || url.isEmpty())
      slotIntroduction();
    else
      openURL(url, true);
}

khcMainWindow::~khcMainWindow()
{
  kdebug(KDEBUG_INFO,1400,"khcMainWindow::~khcMainWindow()");

  cleanUp();
  slotSaveSettings();
  helpWindowList.removeRef(this);
  delete m_pFrame;
  delete m_pNavigator;
  if (m_pSplitter)
    delete m_pSplitter;
}

OPMainWindowIf* khcMainWindow::interface()
{
  if ( m_pInterface == 0L )
    {    
      m_pkhcInterface = new khcMainWindowIf( this );
      m_pInterface = m_pkhcInterface;
    }
  return m_pInterface;
}

khcMainWindowIf* khcMainWindow::khcInterface()
{
  if ( m_pInterface == 0L )
    {
      m_pkhcInterface = new khcMainWindowIf( this );
      m_pInterface = m_pkhcInterface;
    }
  return m_pkhcInterface;
}

void khcMainWindow::cleanUp()
{
  kdebug(KDEBUG_INFO,1400,"void khcMainWindow::cleanUp()");
 
  m_vView = 0L;

  // Release the view component. This will delete the component.
  if (m_pFrame)
    m_pFrame->detach();
}

void khcMainWindow::setupView()
{
    kdebug(KDEBUG_INFO,1400,"khcMainWindow::setupView()");
    m_pSplitter = new QSplitter(QSplitter::Horizontal, this);
    CHECK_PTR(m_pSplitter);

    m_pNavigator = new KHelpNavigator(m_pSplitter);
    m_pFrame = new OPFrame(m_pSplitter);
    CHECK_PTR(m_pNavigator);
    CHECK_PTR(m_pFrame);
  
    QValueList<int> sizes;
    sizes.append(200);
    sizes.append(600);

    m_pSplitter->setSizes(sizes);
    m_pSplitter->show();
    setView(m_pSplitter, true);
    m_pView = new khcHTMLView;
    m_vView = KHelpCenter::View::_duplicate(m_pView);
    kdebug(KDEBUG_INFO,1400,"m_vView = KHelpCenter::View::_duplicate(new khcHTMLView);");
  
    m_vView->setMainWindow(khcInterface());
    kdebug(KDEBUG_INFO,1400,"m_vView->setMainWindow(khcInterface());");
    connectView();
    m_pFrame->attach(m_vView);
   
    kdebug(KDEBUG_INFO,1400,"m_pFrame->attach(m_vView);");
}

void khcMainWindow::setupMenuBar()
{
    kdebug(KDEBUG_INFO,1400,"khcMainWindow::setupMenuBar()");
    KStdAccel stdAccel;
  
    // file menu
    m_pFileMenu = new QPopupMenu;
    CHECK_PTR(m_pFileMenu);

    m_pFileMenu->insertItem(i18n("&New Help Browser"), this, SLOT(slotNewBrowser()), stdAccel.openNew());
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
    m_pGotoMenu->insertItem(i18n("&Introduction"), this, SLOT(slotIntroduction()));
  
    // view menu
    m_pViewMenu = new QPopupMenu;
    CHECK_PTR(m_pViewMenu);

    idMagPlus = m_pViewMenu->insertItem(i18n("Zoom &in"), this, SLOT(slotMagPlus()));
    idMagMinus = m_pViewMenu->insertItem(i18n("Zoom &out"), this, SLOT(slotMagMinus()));
    m_pViewMenu->insertSeparator();
    m_pViewMenu->insertItem(i18n("&Reload Navigator"), m_pNavigator, SLOT(slotReloadTree()));
    m_pViewMenu->insertSeparator();
    m_pViewMenu->insertItem(i18n("Reload &Document"), this, SLOT(slotReload()), Key_F5);
  
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

    idNavigator = m_pOptionsMenu->insertItem(i18n("Show &Navigator"), this, SLOT(slotOptionsNavigator()));
    idToolBar = m_pOptionsMenu->insertItem(i18n("Show &Toolbar"), this, SLOT(slotOptionsToolbar()));
    idLocationBar = m_pOptionsMenu->insertItem(i18n("Show &Location"), this, SLOT(slotOptionsLocationbar()));
    idStatusBar = m_pOptionsMenu->insertItem(i18n("Show Status&bar"), this, SLOT(slotOptionsStatusbar()));
    m_pOptionsMenu->insertSeparator();
    m_pOptionsMenu->insertItem(i18n("&Settings..."), this, SLOT(slotOptionsGeneral()));
   
  
    // help menu
    m_pHelpMenu = kapp->getHelpMenu(true,i18n("The KDE HelpCenter v" + QString(HELPCENTER_VERSION) + "\n\n"
			     "(c) 1998,99 Matthias Elter <me@kde.org>: khelpcenter base application, kwelcome\n"
			     "(c) 1998,99 René Beutler <rbeutler@g26.ethz.ch>: kassistant, kcmhelpcenter, kwid,konitemhelp\n"
			     "(c) 1997 Martin Jones <mjones@kde.org> Some code is based on kdehelp written by Martin."));
    
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

void khcMainWindow::setupToolBar()
{
    kdebug(KDEBUG_INFO,1400,"khcMainWindow::setupToolBar()");
    
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
    toolBar(0)->insertButton(Icon("hidenavigator.xpm"), TB_NAVIGATOR, true, i18n("Hide Navigator"));

    toolBar(0)->insertSeparator();

    toolBar(0)->insertButton(Icon("back.xpm"), TB_BACK, true, i18n("Back"));
    toolBar(0)->setDelayedPopup(TB_BACK, m_pHistoryBackMenu);

    toolBar(0)->insertButton(Icon("forward.xpm"), TB_FORWARD, true, i18n("Forward"));
    toolBar(0)->setDelayedPopup(TB_FORWARD, m_pHistoryForwardMenu);
  
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

void khcMainWindow::setupLocationBar()
{
    kdebug(KDEBUG_INFO,1400,"khcMainWindow::setupLocationbar()");

    toolBar(1)->insertLined("", 1, SIGNAL(returnPressed()), this, SLOT(slotLocationEntered()));
    toolBar(1)->setFullWidth(TRUE);
    toolBar(1)->setItemAutoSized(1, TRUE);
    toolBar(1)->enable(KToolBar::Show);
}

void khcMainWindow::setupStatusBar()
{
    kdebug(KDEBUG_INFO,1400,"khcMainWindow::setupStatusbar()");
    statusBar()->insertItem("", 1);
    enableStatusBar(KStatusBar::Show);
}

void khcMainWindow::enableNavigator(bool enable)
{
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
  updateRects();

  /*
  if (enable)
    {
	m_pSplitter = new QSplitter(QSplitter::Horizontal, this);
	CHECK_PTR(m_pSplitter);

	m_pNavigator->recreate(m_pSplitter, 0, QPoint(0,0),true);
	m_pFrame->recreate(m_pSplitter, 0, QPoint(0,0),true);
	m_pNavigator->show();
	
	QValueList<int> sizes;
	sizes.append(200);
	sizes.append(600);

	m_pSplitter->setSizes(sizes);
	m_pSplitter->show();

	setView(m_pSplitter, TRUE);
    }
    else
    { 
	m_pNavigator->recreate(this, 0, QPoint(0,0),false);
	m_pFrame->recreate(this, 0, QPoint(0,0),true);
	setView(m_pFrame, true);
	delete m_pSplitter;
	m_pSplitter = 0L;
	m_pNavigator->hide();
    }
    updateRects();
  */
}

void khcMainWindow::slotReadSettings()
{
    KConfig *config = KApplication::getKApplication()->getConfig();
    config->setGroup("Appearance");

    // show tool-, location-, statusbar and navigator?
    QString o = config->readEntry("ShowNavigator");
    if (!o.isEmpty() && o.find("No", 0, false ) == 0)
	m_showNavigator = false;
    else
	m_showNavigator = true;
  
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
    if (!m_showNavigator)
	enableNavigator(false);

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
	m_pOptionsMenu->setItemChecked(idNavigator, m_showNavigator);
	m_pOptionsMenu->setItemChecked(idToolBar, m_showToolBar);
	m_pOptionsMenu->setItemChecked(idLocationBar, m_showLocationBar);
	m_pOptionsMenu->setItemChecked(idStatusBar, m_showStatusBar);
    }
}

void khcMainWindow::slotSaveSettings()
{
    KConfig *config = KApplication::getKApplication()->getConfig();

    config->setGroup( "Appearance" );
    config->writeEntry( "ShowNavigator", m_showNavigator ? "Yes" : "No" ); 
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
	warning("khcMainWindow::slotOptionsSave: illegal default in case reached\n");
	break;
    }
}

void khcMainWindow::slotSetLocation(const QString& _url)
{
    toolBar(1)->setLinedText(1, _url);
}

void khcMainWindow::slotLocationEntered()
{
    openURL(toolBar(1)->getLinedText(1), true );
}

void khcMainWindow::slotURLSelected(const QString& _url, int)
{
    openURL( _url, true );
}

void khcMainWindow::slotToolbarClicked(int item)
{
    switch (item)
    {
    case TB_NAVIGATOR:
	slotOptionsNavigator();
	break;
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

void khcMainWindow::slotNewBrowser()
{
    slotOpenNewBrowser(QString(""));
}

void khcMainWindow::slotOpenNewBrowser(const QString& url)
{
    khcMainWindow *win = new khcMainWindow(url);
    win->resize(size());
    win->show();
}

void khcMainWindow::slotSetStatusText(const QString& text)
{
    statusBar()->changeItem(text, 1);
}

void khcMainWindow::slotSetTitle( const QString& _title )
{
    QString appCaption = "KDE HelpCenter - ";
    appCaption += _title;
  
    setCaption( appCaption );
}

void khcMainWindow::slotSetURL(QString url)
{
    openURL(url, true);
}

void khcMainWindow::slotSetBookmark()
{
  
}

void khcMainWindow::slotIntroduction()
{
    QString url = "file:";
    url += locate("html", "default/khelpcenter/main.html");

    openURL(url, true); 
}

void khcMainWindow::slotOpenFile()
{
    QString fileName = KFileDialog::getOpenFileName();
    if (!fileName.isNull())
    {
	QString url = "file:";
	url += fileName;
	openURL(url, true);
    }
}

void khcMainWindow::slotOptionsNavigator()
{
    if (m_showNavigator)
    {
	enableNavigator(false);
	m_showNavigator = false;
	toolBar(0)->setButtonPixmap(TB_NAVIGATOR, Icon("shownavigator.xpm"));
	toolBar(0)->getButton(TB_NAVIGATOR)->setText(i18n("Show Navigator"));
    }
    else
    {
	enableNavigator(true);
	m_showNavigator = true;
	toolBar(0)->setButtonPixmap(TB_NAVIGATOR, Icon("hidenavigator.xpm"));
	toolBar(0)->getButton(TB_NAVIGATOR)->setText(i18n("Hide Navigator"));
    }
    m_pOptionsMenu->setItemChecked(idNavigator, m_showNavigator);
}

void khcMainWindow::slotOptionsToolbar()
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

void khcMainWindow::slotOptionsLocationbar()
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

void khcMainWindow::slotOptionsStatusbar()
{
    m_showStatusBar = !m_showStatusBar;
    m_pOptionsMenu->setItemChecked(idStatusBar, m_showStatusBar); 
    enableStatusBar(KStatusBar::Toggle);
}

void khcMainWindow::slotOptionsGeneral()
{
  
}

void khcMainWindow::slotQuit()
{
    close();
}

void khcMainWindow::openURL(KHelpCenter::URLRequest urlRequest)
{
  slotStopProcessing();
  
  khcHistoryItem *hitem = new khcHistoryItem(urlRequest.url, urlRequest.xOffset, urlRequest.yOffset);
  history.append(hitem);

  kdebug(KDEBUG_INFO,1400,"EMIT_EVENT(m_vView, KHelpCenter::eventOpenURL, eventURL)");
  EMIT_EVENT(m_vView, KHelpCenter::eventOpenURL, urlRequest);
}

void khcMainWindow::openURL(const char *_url, bool withHistory, long xOffset, long yOffset)
{
  slotStopProcessing();
    
  KHelpCenter::EventOpenURL eventURL;
  eventURL.url = CORBA::string_dup(_url);
  eventURL.reload = (CORBA::Boolean)false;
  eventURL.xOffset = xOffset;
  eventURL.yOffset = yOffset;

  if (withHistory)
    {
      khcHistoryItem *hitem = new khcHistoryItem(_url, xOffset, yOffset);
      history.append(hitem);
    }

  kdebug(KDEBUG_INFO,1400,"EMIT_EVENT(m_vView, KHelpCenter::eventOpenURL, eventURL)");
  EMIT_EVENT(m_vView, KHelpCenter::eventOpenURL, eventURL);
}

void khcMainWindow::connectView()
{
  try
    {
      m_vView->connect("openURL", khcInterface(), "openURL");
    }
  catch ( ... )
    {
      kdebug(KDEBUG_WARN,1400,"WARNING: view does not know signal ""openURL"" ");
    }
  try
    {
      m_vView->connect("started", khcInterface(), "slotURLStarted");
    }
  catch ( ... )
    {
      kdebug(KDEBUG_WARN,1400,"WARNING: view does not know signal ""started"" ");
    }
  try
    {
      m_vView->connect("completed", khcInterface(), "slotURLCompleted");
    }
  catch ( ... )
    {
      kdebug(KDEBUG_WARN,1400,"WARNING: view does not know signal ""completed"" ");
    }
  try
    {
      m_vView->connect("setStatusBarText", khcInterface(), "setStatusBarText");
    }
  catch ( ... )
    {
      kdebug(KDEBUG_WARN,1400,"WARNING: view does not know signal ""setStatusBarText"" ");
    }
    try
    {
      m_vView->connect("setLocationBarURL", khcInterface(), "setLocationBarURL");
    }
  catch ( ... )
    {
      kdebug(KDEBUG_WARN,1400,"WARNING: view does not know signal ""setLocationBarURL"" ");
    }
  
  try
    {
      m_vView->connect("createNewWindow", khcInterface(), "createNewWindow");
    }
  catch ( ... )
    {
      kdebug(KDEBUG_WARN,1400,"WARNING: view does not know signal ""createNewWindow"" ");
    }
  
}

void khcMainWindow::slotFind()
{
  
}

void khcMainWindow::slotFindNext()
{
  
}

void khcMainWindow::slotReload()
{
  if (m_pView)
    m_pView->slotReload();
}

void khcMainWindow::slotCopy()
{
  
}

void khcMainWindow::slotPrint()
{
  
}

void khcMainWindow::slotStopProcessing()
{
  if (m_pView)
   m_pView->slotStop();
}

void khcMainWindow::slotMagMinus()
{
    /*if(fontBase > 2)
    {
	if(fontBase == 5)
	{
	    m_pViewMenu->setItemEnabled(idMagPlus, true);
	    toolBar(0)->setItemEnabled(TB_ZOOMIN, true);
	}
	fontBase--;
	//htmlview->setDefaultFontBase( fontBase );
	//htmlview->slotReload();
	if(fontBase == 2)
	{
	    m_pViewMenu->setItemEnabled(idMagMinus, false);
	    toolBar(0)->setItemEnabled(TB_ZOOMOUT, false);
	}		
	}*/
}

void khcMainWindow::slotMagPlus()
{
    /*if(fontBase < 5)
    {
	if (fontBase == 2)
	{
	    m_pViewMenu->setItemEnabled(idMagMinus, true);
	    toolBar(0)->setItemEnabled(TB_ZOOMOUT, true);
	}
	fontBase++;
	//htmlview->setDefaultFontBase(fontBase);
	//htmlview->slotReload();
	if (fontBase == 5)
	{
	    m_pViewMenu->setItemEnabled(idMagPlus, false);
	    toolBar(0)->setItemEnabled(TB_ZOOMIN, false);
	}
	}*/	
}

void khcMainWindow::slotForward()
{
  khcHistoryItem *hitem = history.next();
  if (hitem)
    openURL(hitem->url(), false, hitem->xOffset(), hitem->yOffset());
}

void khcMainWindow::slotBack()
{
  khcHistoryItem *hitem = history.prev();
  if(hitem)
      openURL(hitem->url(), false, hitem->xOffset(), hitem->yOffset());
}

void khcMainWindow::slotHistoryFillBack()
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

void khcMainWindow::slotHistoryFillForward()
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

void khcMainWindow::slotHistoryBackActivated(int id)
{
  int steps = m_pHistoryBackMenu->indexOf(id) + 1;
  khcHistoryItem *item = history.back(steps);
        
  if(item)
    openURL(item->url(), false, item->xOffset(), item->yOffset());
}

void khcMainWindow::slotHistoryForwardActivated(int id)
{
  int steps = m_pHistoryForwardMenu->indexOf(id) + 1;
  khcHistoryItem *item = history.forward(steps);
  
  if(item)
    openURL(item->url(), false, item->xOffset(), item->yOffset());
}

void khcMainWindow::slotSetBusy(bool busy)
{
  toolBar(0)->setItemEnabled(TB_STOP, busy);
}

khcMainWindowIf::khcMainWindowIf(khcMainWindow* _main) :
  OPMainWindowIf( _main )
{
  ADD_INTERFACE("IDL:KHelpCenter/MainWindow:1.0" );
  
  m_pkhcMainWindow = _main;
}

khcMainWindowIf::~khcMainWindowIf()
{
  cleanUp();
}

void khcMainWindowIf::setStatusBarText(const char *_text)
{
  m_pkhcMainWindow->slotSetStatusText(_text);
  kdebug(0, 1400, "void khcMainWindowIf::setStatusBarText(const char *_text)");
}

void khcMainWindowIf::setLocationBarURL(const char *_url)
{
  m_pkhcMainWindow->slotSetLocation(_url);
  m_pkhcMainWindow->slotSetTitle(_url);
  kdebug(0, 1400, "void khcMainWindowIf::setLocationBarURL(const char *_url)");
}

void khcMainWindowIf::createNewWindow(const char *url)
{
  m_pkhcMainWindow->slotOpenNewBrowser(QString(url));
  kdebug(0, 1400, "void khcMainWindowIf::createNewWindow(const char *url)");
}

void khcMainWindowIf::slotURLStarted(const char *)
{
  m_pkhcMainWindow->slotSetBusy(true);
  kdebug(0, 1400, "void khcMainWindowIf::slotURLStarted(const char *url)");
}

void khcMainWindowIf::slotURLCompleted()
{
  m_pkhcMainWindow->slotSetBusy(false);
  kdebug(0, 1400, "void khcMainWindowIf::slotURLCompleted()");
}

void khcMainWindowIf::openURL(const KHelpCenter::URLRequest &url)
{
  m_pkhcMainWindow->openURL(url);
  kdebug(0, 1400, "void khcMainWindowIf::openURL(const KHelpCenter::URLRequest &url)");
}


#include "khc_mainwindow.moc"
