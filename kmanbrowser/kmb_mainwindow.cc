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

#include <qkeycode.h>

#include <kapp.h>
#include <kiconloader.h>
#include <kstdaccel.h>
#include <kcursor.h>
#include <klocale.h>
#include <kstddirs.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <kconfig.h>
#include <kaction.h>
#include <kstdaction.h>

#include "kmb_mainwindow.h"
#include "kmb_view.h"
#include "version.h"

kmbMainWindow::kmbMainWindow(const QString& url)
{
    kDebugInfo( 1400,"kmbMainWindow::kmbMainWindow()");

    setCaption(i18n("KDE ManBrowser"));
    resize(800, 580);
    setMinimumSize(200, 100);

    // setup UI
    setupActions();
    setupView();
    createGUI( "kmanbrowser.rc" );
    setupLocationBar();
    setupStatusBar();

    // read settings
    //slotReadSettings();

    //open url
    if( url.isEmpty() )
      slotIndex();
    else
      openURL(url, true);
}

kmbMainWindow::~kmbMainWindow()
{
  kDebugInfo( 1400,"kmbMainWindow::~kmbMainWindow()");

  slotSaveSettings();
}

void kmbMainWindow::setupView()
{
    kDebugInfo( 1400, "kmbMainWindow::setupView()" );

    m_pView = new kmbView( this );
    setView( m_pView, true );

    m_zoomIn->setEnabled( m_pView->canZoomIn() );
    m_zoomOut->setEnabled( m_pView->canZoomOut() );
}

void kmbMainWindow::setupActions()
{
    kDebugInfo( 1400, "kmbMainWindow::setupActions()" );

    new KAction( i18n( "Open &new window" ), 0, this, SLOT( slotNewBrowser() ), actionCollection(), "new_window" );

    KStdAction::open( this, SLOT( slotOpenFile() ), actionCollection(), "open" );

    new KAction( i18n( "&Reload" ), Key_F5, this, SLOT( slotReload() ), actionCollection(), "reload" );

    KStdAction::print( this, SLOT( slotPrint() ), actionCollection(), "print" );

    KStdAction::quit( this, SLOT( slotQuit() ), actionCollection(), "quit" );

    KStdAction::copy( this, SLOT( slotCopy() ), actionCollection(), "copy" );

    KStdAction::find( this, SLOT( slotFind() ), actionCollection(), "find" );

    KStdAction::findNext( this, SLOT( slotFindNext() ), actionCollection(), "find_next" );

    m_back = KStdAction::back( this, SLOT( slotBack() ), actionCollection(), "back" );

    m_forward = KStdAction::forward( this, SLOT( slotForward() ), actionCollection(), "forward" );

    new KAction( i18n( "&Index" ), 0, this, SLOT( slotIndex() ), actionCollection(), "index" );

    m_zoomIn = KStdAction::zoomIn( this, SLOT( slotMagPlus() ), actionCollection(), "zoom_in" );

    m_zoomOut = KStdAction::zoomOut( this, SLOT( slotMagMinus() ), actionCollection(), "zoom_out" );

    m_stop = new KAction( i18n( "&Stop" ), 0, this, SLOT( slotStop() ), actionCollection(), "stop" );

    /*
    connect(m_pBookmarkMenu, SIGNAL(activated(int)), this, SLOT(slotBookmarkSelected(int)));
    connect(m_pBookmarkMenu, SIGNAL(highlighted(int)), this, SLOT(slotBookmarkHighlighted(int)));
    */

    m_toolBar = KStdAction::showToolbar( this, SLOT( slotOptionsToolbar() ), actionCollection(), "show_toolbar" );

    m_locationBar = new KToggleAction( i18n( "Show &Location"), 0, this, SLOT( slotOptionsLocationbar() ), actionCollection(), "show_locationbar" );

    m_statusBar = KStdAction::showStatusbar( this, SLOT( slotOptionsStatusbar() ), actionCollection(), "show_statusbar" );

    KStdAction::preferences( this, SLOT( slotOptionsGeneral() ), actionCollection(), "settings" );

/*
    m_pHelpMenu = kapp->helpMenu(true,i18n("KDE ManBrowser v" + QString(KMANBROWSER_VERSION) + "\n\n"
                                           "(c) 1999 Matthias Elter <me@kde.org>"));
*/
}

void kmbMainWindow::setupLocationBar()
{
    kDebugInfo( 1400, "kmbMainWindow::setupLocationbar()" );

    toolBar(1)->insertLined( "", 1, SIGNAL( returnPressed() ), this, SLOT( slotLocationEntered() ) );
    toolBar(1)->setFullWidth( TRUE );
    toolBar(1)->setItemAutoSized( 1, TRUE );
    toolBar(1)->enable( KToolBar::Show );
}

void kmbMainWindow::setupStatusBar()
{
    kDebugInfo( 1400, "kmbMainWindow::setupStatusbar()" );

    statusBar()->insertItem( "", 1 );
    enableStatusBar( KStatusBar::Show );
}

void kmbMainWindow::slotReadSettings()
{
    KConfig *config = KApplication::kApplication()->config();
    config->setGroup("Appearance");

    // show tool-, location-, statusbar
    m_showToolBar = config->readBoolEntry( "ShowToolBar", true );
    m_showStatusBar = config->readBoolEntry( "ShowStatusBar", true );
    m_showLocationBar = config->readBoolEntry( "ShowLocationBar", true );

    // toolbar location
    QString o = config->readEntry( "ToolBarPos", "Top" );

    if ("Top" == o)
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
    o = config->readEntry( "LocationBarPos", "Top" );
    if ("Top" == o)
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
	enableStatusBar( KStatusBar::Show );
    else
	enableStatusBar( KStatusBar::Hide );

    if (m_showToolBar)
	enableToolBar(KToolBar::Show, 0);
    else
	enableToolBar(KToolBar::Hide, 0);

    if (m_showLocationBar)
	enableToolBar(KToolBar::Show, 1);
    else
	enableToolBar(KToolBar::Hide, 1);

    // toggle menu items
    m_toolBar->setChecked( m_showToolBar );
    m_locationBar->setChecked( m_showLocationBar );
    m_statusBar->setChecked( m_showStatusBar );
}

void kmbMainWindow::slotSaveSettings()
{
    KConfig *config = KApplication::kApplication()->config();

    config->setGroup( "Appearance" );

    config->writeEntry( "ShowToolBar", m_showToolBar );
    config->writeEntry( "ShowStatusBar", m_showStatusBar );
    config->writeEntry( "ShowLocationBar", m_showLocationBar );

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

void kmbMainWindow::slotSetLocation(const QString& url)
{
    toolBar(1)->setLinedText(1, url);
}

void kmbMainWindow::slotLocationEntered()
{
    openURL(toolBar(1)->getLinedText(1), true );
}

void kmbMainWindow::slotURLSelected(const QString& url, int)
{
    openURL( url, true );
}

void kmbMainWindow::slotNewBrowser()
{
    slotOpenNewBrowser("");
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

void kmbMainWindow::slotSetTitle( const QString& title )
{
    setCaption( "KDE ManBrowser - " + title );
}

void kmbMainWindow::slotSetURL(QString url)
{
    openURL(url, true);
}

void kmbMainWindow::slotAddBookmark()
{
}

void kmbMainWindow::slotIndex()
{
    openURL("man:(index)", true);
}

void kmbMainWindow::slotOpenFile()
{
    KURL url = KFileDialog::getOpenURL();

    if( url.isEmpty() )
      return;

    if( !url.isLocalFile() )
    {
	KMessageBox::sorry( 0L, i18n( "Only local files supoorted yet." ) );
	return;
    }
	
    openURL( url.path(), true );
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

    m_toolBar->setChecked(m_showToolBar);
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
    m_locationBar->setChecked(m_showLocationBar);
}

void kmbMainWindow::slotOptionsStatusbar()
{
    m_showStatusBar = !m_showStatusBar;
    m_statusBar->setChecked(m_showStatusBar);
    enableStatusBar(KStatusBar::Toggle);
}

void kmbMainWindow::slotOptionsGeneral()
{
}

void kmbMainWindow::slotQuit()
{
    close();
}

void kmbMainWindow::openURL( const QString& url )
{
    debug( "kmbMainWindow::openURL" );
    
  slotStopProcessing();

  //khcHistoryItem *hitem = new khcHistoryItem(urlRequest.url, urlRequest.xOffset, urlRequest.yOffset);
  khcHistoryItem *hitem = new khcHistoryItem( url );
  history.append( hitem );

  slotSetLocation( url );

  //EMIT_EVENT(m_vView, Browser::eventOpenURL, urlRequest);
  slotCheckHistory();
}

void kmbMainWindow::openURL( const QString& url, bool withHistory, long xOffset, long yOffset )
{
  debug( "kmbMainWindow::openURL 2" );
    
  slotStopProcessing();

  if( withHistory )
  {
      khcHistoryItem *hitem = new khcHistoryItem( url, xOffset, yOffset );
      history.append( hitem );
  }

  slotSetLocation( url );

  m_pView->openURL( url, withHistory, xOffset, yOffset );
  slotCheckHistory();
}

/*
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
*/

void kmbMainWindow::slotFind()
{
}

void kmbMainWindow::slotFindNext()
{
}

void kmbMainWindow::slotReload()
{
    /*
  Browser::EventOpenURL eventURL;
  eventURL.url = m_vView->url();
  eventURL.reload = true;
  eventURL.xOffset = m_vView->xOffset();
  eventURL.yOffset = m_vView->yOffset();
  EMIT_EVENT( m_vView, Browser::eventOpenURL, eventURL );
  kdebug(0, 1400, "EMIT_EVENT( m_vView, Browser::eventOpenURL, eventURL );");
    */
}

void kmbMainWindow::slotCopy()
{
}

void kmbMainWindow::slotPrint()
{
  m_pView->print();
}

void kmbMainWindow::slotStopProcessing()
{
  m_pView->stop();
}

void kmbMainWindow::slotMagMinus()
{
  m_pView->zoomOut();
  m_zoomIn->setEnabled( m_pView->canZoomIn() );
  m_zoomOut->setEnabled( m_pView->canZoomOut() );
}

void kmbMainWindow::slotMagPlus()
{
  m_pView->zoomIn();
  m_zoomIn->setEnabled( m_pView->canZoomIn() );
  m_zoomOut->setEnabled( m_pView->canZoomOut() );
}

void kmbMainWindow::slotCheckHistory()
{
  m_back->setEnabled( history.hasPrev() );
  m_forward->setEnabled( history.hasNext() );
}

void kmbMainWindow::slotForward()
{
  khcHistoryItem *hitem = history.next();

  if( hitem )
    openURL( hitem->url(), false, hitem->xOffset(), hitem->yOffset() );
}

void kmbMainWindow::slotBack()
{
  khcHistoryItem *hitem = history.prev();

  if( hitem )
      openURL( hitem->url(), false, hitem->xOffset(), hitem->yOffset() );
}

void kmbMainWindow::slotHistoryFillBack()
{
    /*
  m_pHistoryBackMenu->clear();

  QList<khcHistoryItem> list = history.backList();
  khcHistoryItem *item = list.first();

  while( item )
    {
      QString url = item->url();
      m_pHistoryBackMenu->insertItem(url, this, 0);
      item = list.next();
    }
    */
}

void kmbMainWindow::slotHistoryFillForward()
{
    /*
  m_pHistoryForwardMenu->clear();

  QList<khcHistoryItem> list = history.forwardList();
  khcHistoryItem *item = list.first();

  while (item)
    {
      QString url = item->url();
      m_pHistoryForwardMenu->insertItem(url, this, 0);
      item = list.next();
    }
    */
}

void kmbMainWindow::slotHistoryBackActivated( int )
{
    /*
  int steps = m_pHistoryBackMenu->indexOf(id) + 1;
  khcHistoryItem *item = history.back(steps);

  if( item )
    openURL( item->url(), false, item->xOffset(), item->yOffset() );
    */
}

void kmbMainWindow::slotHistoryForwardActivated( int )
{
    /*
  int steps = m_pHistoryForwardMenu->indexOf(id) + 1;
  khcHistoryItem *item = history.forward(steps);

  if( item )
    openURL( item->url(), false, item->xOffset(), item->yOffset() );
    */
}

void kmbMainWindow::slotSetBusy( bool busy )
{
  m_stop->setEnabled( busy );
}

#include "kmb_mainwindow.moc"
