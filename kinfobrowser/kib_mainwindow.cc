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

#include <kapp.h>
#include <kdebug.h>
#include <klocale.h>
#include <kstddirs.h>
#include <kconfig.h>
#include <kaction.h>
#include <kstdaction.h>
//#include <khelpmenu.h>
#include <kfiledialog.h>

#include "version.h"
#include "kib_view.h"
#include "kib_mainwindow.h"

#define ID_TOOLBAR_NORMAL   0
#define ID_TOOLBAR_LOCATION 1
#define ID_STATUSBAR_INFO   1

kibMainWindow::kibMainWindow( const QString& url, char *name )
  : KTMainWindow( name )
{
  kDebugInfo( 1400, "kibMainWindow::kibMainWindow" );

  setCaption( i18n( "KDE InfoBrowser" ) );
  resize( 800, 580 );
  setMinimumSize( 200, 100 );

  // setup UI
  m_pView = new kibView( this );
  setView( m_pView );

  setupActions();
  setupStatusBar();
  createGUI( "kinfobrowser.rc" );
  setupLocationBar();

  connect( m_pView, SIGNAL( statusMsg( const QString& ) ), this, SLOT( slotStatusMsg( const QString& ) ) );

  // read settings
  slotReadSettings();

  //open url
  if( url.isEmpty() )
    slotIndex();
  else
  {
    slotSetLocation( url );
    openURL( url, false );
  }
}

kibMainWindow::~kibMainWindow()
{
  slotSaveSettings();
}

void kibMainWindow::setupActions()
{
    new KAction( i18n( "Open &new window" ), 0, this, SLOT( slotNewBrowser() ), actionCollection(), "new_window" );

    new KAction( i18n( "&Reload" ), BarIcon( "reload" ), Key_F5, this, SLOT( slotReload() ), actionCollection(), "reload" );

    KStdAction::print( this, SLOT( slotPrint() ), actionCollection(), "print" );

    KStdAction::quit( this, SLOT( slotQuit() ), actionCollection(), "quit" );

    KStdAction::copy( this, SLOT( slotCopy() ), actionCollection(), "copy" );

    KStdAction::find( this, SLOT( slotFind() ), actionCollection(), "find" );

    KStdAction::findNext( this, SLOT( slotFind() ), actionCollection(), "find:next" );

    m_back = KStdAction::back( this, SLOT( slotBack() ), actionCollection(), "back" );

    m_forward = KStdAction::forward( this, SLOT( slotForward() ), actionCollection(), "forward" );

    m_stop = new KAction( i18n( "&Stop" ), BarIcon( "stop" ), 0, this, SLOT( slotStop() ), actionCollection(), "stop" );

    new KAction( i18n( "&Index" ), 0, this, SLOT( slotIndex() ), actionCollection(), "index" );

    m_zoomIn = KStdAction::zoomIn( this, SLOT( slotMagPlus() ), actionCollection(), "zoom_in" );

    m_zoomOut = KStdAction::zoomOut( this, SLOT( slotMagMinus() ), actionCollection(), "zoom_out" );

    new KAction( i18n( "&Add bookmark" ), BarIcon( "find"), 0, this, SLOT( slotAddBookmark() ), actionCollection(), "add_bookmark" );

    new KAction( i18n( "&Clear Bookmarks" ), 0, this, SLOT( slotClearBookmarks() ), actionCollection(), "clear_bookmarks" );

    m_bookmarks = new KListAction( actionCollection(), "bookmark_list" );

    //connect(m_pBookmarkMenu, SIGNAL(activated(int)), this, SLOT(slotBookmarkSelected(int)));
    //connect(m_pBookmarkMenu, SIGNAL(highlighted(int)), this, SLOT(slotBookmarkHighlighted(int)));

    m_toolbar = KStdAction::showToolbar( this, SLOT( slotOptionsToolbar() ), actionCollection(), "show_toolbar" );

    m_statusbar = KStdAction::showStatusbar( this, SLOT( slotOptionsStatusbar() ), actionCollection(), "show_statusbar" );

    m_location = new KToggleAction( i18n( "Show &Location" ), 0, this, SLOT( slotOptionsLocationbar() ), actionCollection(), "show_location" );

    KStdAction::saveOptions( this, SLOT( slotOptionsGeneral() ), actionCollection(), "save_options" );

    KStdAction::whatsThis( this, SLOT( slotWhatThis() ), actionCollection(), "help_whats_this" );

    KStdAction::reportBug( this, SLOT( slotReportBug() ), actionCollection(), "help_report_bug" );

/*
    m_pHelpMenu = new KHelpMenu(this,i18n("KDE InfoBrowser v" + QString(KINFOBROWSER_VERSION) + "\n\n"
                                           "(c) 1999 Matthias Elter <me@kde.org>"),true);
*/
}

void kibMainWindow::setupLocationBar()
{
  toolBar( ID_TOOLBAR_LOCATION )->insertLined( "", 1, SIGNAL( returnPressed() ), this, SLOT( slotLocationEntered() ) );
  toolBar( ID_TOOLBAR_LOCATION )->setFullWidth( true );
  toolBar( ID_TOOLBAR_LOCATION )->setItemAutoSized( 1, true );
  toolBar( ID_TOOLBAR_LOCATION )->enable( KToolBar::Show );
  toolBar( ID_TOOLBAR_NORMAL )->setFullWidth( true );
}

void kibMainWindow::setupStatusBar()
{
  statusBar()->insertItem("", ID_STATUSBAR_INFO );
  enableStatusBar( KStatusBar::Show );
}

void kibMainWindow::slotReadSettings()
{
    KConfig *config = KApplication::kApplication()->config();

    config->setGroup( "Appearance" );

    // show tool-, location-, statusbar
    m_showToolBar = config->readBoolEntry( "ShowToolBar", true );
    m_showStatusBar = config->readBoolEntry( "ShowStatusBar", true );
    m_showLocationBar = config->readBoolEntry( "ShowLocationBar", true );

    // toolbar location
    QString o = config->readEntry( "ToolBarPos", "Top" );

    if( o.isEmpty() )
	toolBar( 0 )->setBarPos( KToolBar::Top );
    if( "Top" == o )
	toolBar( 0 )->setBarPos( KToolBar::Top );
    else if( "Bottom" == o )
	toolBar( 0 )->setBarPos( KToolBar::Bottom );
    else if( "Left" == o )
	toolBar( 0 )->setBarPos( KToolBar::Left );
    else if( "Right" == o )
	toolBar( 0 )->setBarPos( KToolBar::Right );
    else if( "Floating" == o )
	toolBar( 0 )->setBarPos( KToolBar::Floating );
    else
	toolBar( 0 )->setBarPos( KToolBar::Top );
	
    // locationbar location
    o = config->readEntry("LocationBarPos", "Top" );

    if ( o.isEmpty() )
   	toolBar( 1 )->setBarPos( KToolBar::Top );
    else if ( "Top" == o )
	toolBar( 1 )->setBarPos( KToolBar::Top );
    else if ( "Bottom" == o)
	toolBar( 1 )->setBarPos( KToolBar::Bottom );
    else if ( "Left" == o)
	toolBar( 1 )->setBarPos( KToolBar::Left );
    else if ( "Right" == o)
	toolBar( 1 )->setBarPos( KToolBar::Right );
    else if ( "Floating" == o)
	toolBar( 1 )->setBarPos( KToolBar::Floating );
    else
	toolBar( 1 )->setBarPos( KToolBar::Top );

/*
    // set configuration
    if( m_showStatusBar )
	enableStatusBar( KStatusBar::Show );
    else
	enableStatusBar( KStatusBar::Hide );

    if( m_showToolBar )
	enableToolBar( KToolBar::Show, 0 );
    else
	enableToolBar(KToolBar::Hide, 0 );

    if( m_showLocationBar )
	enableToolBar( KToolBar::Show, 1 );
    else
	enableToolBar( KToolBar::Hide, 1 );

    // toggle menu items
    m_toolbar->setChecked( m_showToolBar );
    m_location->setChecked( m_showLocationBar );
    m_statusbar->setChecked( m_showStatusBar );
*/
}

void kibMainWindow::slotSaveSettings()
{
    KConfig *config = KApplication::kApplication()->config();

    config->setGroup( "Appearance" );

    config->writeEntry( "ShowToolBar", m_showToolBar );
    config->writeEntry( "ShowStatusBar", m_showStatusBar );
    config->writeEntry( "ShowLocationBar", m_showLocationBar );

    switch( toolBar( 0 )->barPos() )
    {
    case KToolBar::Top:
	config->writeEntry( "ToolBarPos", "Top" );
	break;
    case KToolBar::Bottom:
	config->writeEntry( "ToolBarPos", "Bottom" );
	break;
    case KToolBar::Left:
	config->writeEntry( "ToolBarPos", "Left" );
	break;
    case KToolBar::Right:
	config->writeEntry( "ToolBarPos", "Right" );
	break;
    case KToolBar::Floating:
	config->writeEntry( "ToolBarPos", "Floating" );
	break;
    default:
	warning( "helpCenter::slotOptionsSave: illegal default in case reached\n");
	break;
    }

    switch( toolBar( 1 )->barPos() )
    {
    case KToolBar::Top:
	config->writeEntry( "LocationBarPos", "Top" );
	break;
    case KToolBar::Bottom:
	config->writeEntry( "LocationBarPos", "Bottom" );
	break;
    case KToolBar::Left:
	config->writeEntry( "LocationBarPos", "Left" );
	break;
    case KToolBar::Right:
	config->writeEntry( "LocationBarPos", "Right" );
	break;
    case KToolBar::Floating:
	config->writeEntry( "LocationBarPos", "Floating" );
	break;
    default:
	warning("kibMainWindow::slotOptionsSave: illegal default in case reached\n");
	break;
    }
}

void kibMainWindow::slotSetLocation( const QString& url )
{
  toolBar( ID_TOOLBAR_LOCATION )->setLinedText( 1, url );
}

void kibMainWindow::slotLocationEntered()
{
  openURL( toolBar( ID_TOOLBAR_LOCATION )->getLinedText( 1 ), true );
}

void kibMainWindow::slotURLSelected( const QString& url, const QString& target, int withHistory )
{
  openURL( url, target, withHistory );
}

void kibMainWindow::slotNewBrowser()
{
  slotOpenNewBrowser();
}

void kibMainWindow::slotOpenNewBrowser( const QString& url )
{
  kibMainWindow *win = new kibMainWindow( url );
  win->resize( size() );
  win->show();
}

void kibMainWindow::slotStatusMsg( const QString& text )
{
  statusBar()->changeItem( text, ID_STATUSBAR_INFO );
}

void kibMainWindow::slotSetTitle( const QString& title )
{
  setCaption( "KDE InfoBrowser - " + title );
}

void kibMainWindow::slotSetURL( const QString& url )
{
  openURL( url, true );
}

void kibMainWindow::slotIndex()
{
  kDebugInfo( 1400,"kibMainWindow::slotIndex" );

  QString url = "info:(dir)Top";

  slotSetLocation( url );
  openURL( url, true );
}

void kibMainWindow::slotOpenFile()
{
  KURL url = KFileDialog::getOpenURL();

  if( url.isEmpty() )
      return;

  openURL( url.url(), true );
}

void kibMainWindow::slotOptionsToolbar()
{
  if( m_showToolBar )
  {
    enableToolBar( KToolBar::Hide, 0 );
    m_showToolBar = false;
  }
  else
  {
    enableToolBar( KToolBar::Show, 0 );
    m_showToolBar = true;
  }

  m_toolbar->setChecked( m_showToolBar );
}

void kibMainWindow::slotOptionsLocationbar()
{
  if( m_showLocationBar )
  {
    enableToolBar( KToolBar::Hide, 1 );
    m_showLocationBar = false;
  }
  else
  {
    enableToolBar( KToolBar::Show, 1 );
    m_showLocationBar = true;
  }

  m_location->setChecked( m_showLocationBar );
}

void kibMainWindow::slotOptionsStatusbar()
{
  m_showStatusBar = !m_showStatusBar;
  m_statusbar->setChecked( m_showStatusBar );
  enableStatusBar( KStatusBar::Toggle );
}

void kibMainWindow::slotOptionsGeneral()
{
}

void kibMainWindow::slotQuit()
{
  close();
}

void kibMainWindow::openURL( const QString& urlRequest )
{
  kDebugInfo( 1400, "kibMainWindow::openURL 1" );

  slotStopProcessing();

  //khcHistoryItem *hitem = new khcHistoryItem( urlRequest.url, urlRequest.xOffset, urlRequest.yOffset );
  khcHistoryItem *hitem = new khcHistoryItem( urlRequest );
  m_history.append( hitem );

  kDebugInfo( 1400, "EMIT_EVENT(m_pView, Browser::eventOpenURL, eventURL)" );
  //EMIT_EVENT( m_pView, Browser::eventOpenURL, urlRequest );
  slotCheckHistory();
}

void kibMainWindow::openURL( const QString& url, bool withHistory, long xOffset, long yOffset )
{
  kDebugInfo( 1400, "kibMainWindow::openURL 2" );

  slotStopProcessing();

  if( withHistory )
  {
    khcHistoryItem *hitem = new khcHistoryItem( url, xOffset, yOffset );
    m_history.append( hitem );
  }

  m_pView->openURL( url, withHistory, xOffset, yOffset );
  slotCheckHistory();
}

/*
void kibMainWindow::connectView()
{
  try
    {
      m_pView->connect("openURL", kibInterface(), "openURL");
    }
  catch ( ... )
    {
      kDebugWarn(1400,"WARNING: view does not know signal ""openURL"" ");
    }
  try
    {
      m_pView->connect("started", kibInterface(), "slotURLStarted");
    }
  catch ( ... )
    {
      kDebugWarn(1400,"WARNING: view does not know signal ""started"" ");
    }
  try
    {
      m_pView->connect("completed", kibInterface(), "slotURLCompleted");
    }
  catch ( ... )
    {
      kDebugWarn(1400,"WARNING: view does not know signal ""completed"" ");
    }
  try
    {
      m_pView->connect("setStatusBarText", kibInterface(), "setStatusBarText");
    }
  catch ( ... )
    {
      kDebugWarn(1400,"WARNING: view does not know signal ""setStatusBarText"" ");
    }
    try
    {
      m_pView->connect("setLocationBarURL", kibInterface(), "setLocationBarURL");
    }
  catch ( ... )
    {
      kDebugWarn(1400,"WARNING: view does not know signal ""setLocationBarURL"" ");
    }

  try
    {
      m_pView->connect("createNewWindow", kibInterface(), "createNewWindow");
    }
  catch ( ... )
    {
      kDebugWarn(1400,"WARNING: view does not know signal ""createNewWindow"" ");
    }
}
*/

void kibMainWindow::slotFind()
{
  m_pView->slotFind();
}

void kibMainWindow::slotFindNext()
{
  m_pView->slotFindNext();
}

void kibMainWindow::slotReload()
{
}

void kibMainWindow::slotCopy()
{
}

void kibMainWindow::slotPrint()
{
  m_pView->print();
}

void kibMainWindow::slotStopProcessing()
{
  m_pView->stop();
}

void kibMainWindow::slotAddBookmark()
{
}

void kibMainWindow::slotClearBookmarks()
{
}

void kibMainWindow::slotMagMinus()
{
  m_pView->zoomOut();
  m_zoomIn->setEnabled( m_pView->canZoomIn() );
  m_zoomOut->setEnabled( m_pView->canZoomOut() );
}

void kibMainWindow::slotMagPlus()
{
  m_pView->zoomIn();
  m_zoomIn->setEnabled( m_pView->canZoomIn() );
  m_zoomOut->setEnabled( m_pView->canZoomOut() );
}

void kibMainWindow::slotCheckHistory()
{
  m_back->setEnabled( m_history.hasPrev() );
  m_forward->setEnabled( m_history.hasNext() );
}

void kibMainWindow::slotForward()
{
  khcHistoryItem *hitem = m_history.next();

  if( hitem )
  {
    QString url = hitem->url();

    slotSetLocation( url );
    openURL( url, false, hitem->xOffset(), hitem->yOffset() );
  }
}

void kibMainWindow::slotBack()
{
  khcHistoryItem *hitem = m_history.prev();

  if( hitem )
  {
    QString url = hitem->url();

    slotSetLocation( url );
    openURL( url, false, hitem->xOffset(), hitem->yOffset() );
  }
}

void kibMainWindow::slotHistoryFillBack()
{
/*
  m_pHistoryBackMenu->clear();

  QList<khcHistoryItem> list = m_history.backList();
  khcHistoryItem *item = list.first();

  while (item)
    {
      m_pHistoryBackMenu->insertItem(item->url().url(), this, 0);
      item = list.next();
    }
*/
}

void kibMainWindow::slotHistoryFillForward()
{
/*
  m_pHistoryForwardMenu->clear();

  QList<khcHistoryItem> list = m_history.forwardList();
  khcHistoryItem *item = list.first();

  while (item)
    {
      m_pHistoryForwardMenu->insertItem(item->url().url(), this, 0);
      item = list.next();
    }
*/
}

void kibMainWindow::slotHistoryBackActivated( int )
{
/*
  int steps = m_pHistoryBackMenu->indexOf(id) + 1;
  khcHistoryItem *item = m_history.back(steps);

  if(item)
    openURL(QString(item->url()), false, item->xOffset(), item->yOffset());
*/
}

void kibMainWindow::slotHistoryForwardActivated( int )
{
/*
  int steps = m_pHistoryForwardMenu->indexOf(id) + 1;
  khcHistoryItem *item = m_history.forward(steps);

  if(item)
    openURL(QString(item->url()), false, item->xOffset(), item->yOffset());
*/
}

void kibMainWindow::slotSetBusy( bool busy )
{
  m_stop->setEnabled( busy );
}

#include "kib_mainwindow.moc"
