/*
 *  toplevel.cpp - part of the KDE Help Center
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

#include "toplevel.h"
#include "htabview.h"
#include "khelpview.h"

#include <qkeycode.h>
#include <qmsgbox.h>

#include <kiconloader.h>
#include <kstdaccel.h>
#include <kcursor.h>

// static list of HelpCenter windows:
QList<HelpCenter> HelpCenter::helpWindowList;
QList<QPixmap> HelpCenter::animatedWheel;

HelpCenter::HelpCenter()
{
  setCaption(i18n("KDE Help Center"));

  resize(800, 580);
  setMinimumSize( 200, 100 );

  // setup GUI
  setupView();
  setupMenubar();
  setupToolbar();
  setupLocationbar();
  setupStatusbar();

  // setup animated wheel
  animatedWheelTimer = new QTimer(this);
  animatedWheelCounter = 0;
  connect(animatedWheelTimer, SIGNAL(timeout()),
		  this, SLOT(slotAnimatedWheelTimeout()));

  // read configuration
  slotReadConfig();

  fontBase = 3;
  htmlview->setDefaultFontBase(fontBase);

  // toggle menu items
  optionsMenu->setItemChecked(idTree, showTree);
  optionsMenu->setItemChecked(idToolbar, showToolbar);
  optionsMenu->setItemChecked(idLocationbar, showLocationbar);
  optionsMenu->setItemChecked(idStatusbar, showStatusbar);

  // connect misc. signals
  connect (htmlview, SIGNAL (setBusy(bool)), 
  		   this, SLOT (slotSetBusy(bool)));
  connect (htmlview, SIGNAL ( enableMenuItems() ), 
		   this, SLOT ( slotEnableMenuItems() ) );
  connect (htmlview, SIGNAL ( openNewWindow(const QString&) ),
		   this, SLOT ( slotNewWindow(const QString&) ) );
  connect (htmlview, SIGNAL ( setURL(const QString&) ),
		   this, SLOT ( slotSetStatusText(const QString&) ) );
  connect (htmlview, SIGNAL ( setLocation(const QString&) ),
		   this, SLOT ( slotSetLocation(const QString&) ) );  
  connect (htmlview, SIGNAL ( bookmarkChanged(KFileBookmark *) ),
		   this, SLOT ( slotBookmarkChanged(KFileBookmark *) ) );
  
  connect(htmlview , SIGNAL( setTitle(const QString& ) ),
		   this, SLOT( slotSetTitle(const QString& ) ) );

  // connect tabview
  connect(tabview , SIGNAL(itemSelected(QString) ),
		this, SLOT( slotSetURL(QString) ) );
 
  // restore geometry settings
  KConfig *config = KApplication::getKApplication()->getConfig();
  config->setGroup( "Appearance" );
  QString geom = config->readEntry( "Geometry" );
  if (!geom.isEmpty())
	{
	  int width, height;
	  sscanf( geom, "%dx%d", &width, &height );
	  resize( width, height );
	}
  
  // put bookmarks into boormark menu
  htmlview->slotBookmarkChanged();

  // append current window to helpWindoList
  helpWindowList.setAutoDelete(false);
  helpWindowList.append(this);
  listIndex = helpWindowList.at();
}

HelpCenter::~HelpCenter()
{
  helpWindowList.removeRef(this);
  delete htmlview;
  delete tabview;
  if (splitter)
	delete splitter;
}

void HelpCenter::setupView()
{
  splitter = new QSplitter(QSplitter::Horizontal, this);
  CHECK_PTR(splitter);

  tabview = new HTabView(this);
  htmlview = new KHelpView(this);
  
  // I have to use resize and recreate here because QSplitter is messing
  // up the initial sizes otherwise.
  tabview->resize(200, 550);
  htmlview->resize(600, 550);
  tabview->recreate(splitter, 0, QPoint(0,0),true);
  htmlview->recreate(splitter, 0, QPoint(0,0),true);

  CHECK_PTR(tabview);
  CHECK_PTR(htmlview);
  splitter->show();
    
  setView(splitter, TRUE);	
  updateRects();
}

void HelpCenter::setupMenubar()
{
  KStdAccel stdAccel;
  
  fileMenu = new QPopupMenu;
  CHECK_PTR( fileMenu );
  fileMenu->insertItem(i18n("&New Help Window"),
					   this,SLOT(slotCloneWindow()), stdAccel.openNew());
  fileMenu->insertSeparator();
  fileMenu->insertItem(i18n("&Open File..."), htmlview,
						SLOT(slotOpenFile()), stdAccel.open() );
  fileMenu->insertItem(i18n("&Reload"), htmlview,
					   SLOT(slotReload()), Key_F5);
  fileMenu->insertItem( i18n("&Print..."),
						htmlview, SLOT(slotPrint()), stdAccel.print() );
  fileMenu->insertSeparator();
  fileMenu->insertItem( i18n("&Quit"), this,
						SLOT(slotQuit()), stdAccel.quit() );
  
  editMenu = new QPopupMenu;
  CHECK_PTR( editMenu );
  idCopy = editMenu->insertItem(i18n("&Copy"), htmlview, SLOT(slotCopy()), stdAccel.copy());
  editMenu->insertSeparator();
  editMenu->insertItem(i18n("&Find..."), htmlview, SLOT(slotFind()), stdAccel.find());
  editMenu->insertItem(i18n("Find &next"), htmlview, SLOT(slotFindNext()), Key_F3);
  
  gotoMenu = new QPopupMenu;
  CHECK_PTR(gotoMenu);
  idBack = gotoMenu->insertItem(i18n("&Back"), htmlview, SLOT(slotBack()));
  idForward = gotoMenu->insertItem(i18n("&Forward"), htmlview, SLOT(slotForward()));
  gotoMenu->insertSeparator();
  gotoMenu->insertItem(i18n("&Introduction"), htmlview, SLOT(slotDir()));
  
  viewMenu = new QPopupMenu;
  CHECK_PTR(viewMenu);
  idMagPlus = viewMenu->insertItem(i18n("Zoom &in"), this, SLOT(slotMagPlus()));
  idMagMinus = viewMenu->insertItem(i18n("Zoom &out"), this, SLOT(slotMagMinus()));
  viewMenu->insertSeparator();
  viewMenu->insertItem(i18n("&Reload Tree"), tabview, SLOT(slotReloadTree()));
  viewMenu->insertSeparator();
  viewMenu->insertItem(i18n("Reload &Document"), htmlview, SLOT(slotReload()), Key_F5);
  
  bookmarkMenu = new QPopupMenu;
  CHECK_PTR( bookmarkMenu );
  connect( bookmarkMenu, SIGNAL( activated( int ) ),
		   htmlview, SLOT( slotBookmarkSelected( int ) ) );
  connect( bookmarkMenu, SIGNAL( highlighted( int ) ),
		   htmlview, SLOT( slotBookmarkHighlighted( int ) ) );
   
  optionsMenu = new QPopupMenu;
  CHECK_PTR(optionsMenu);
  optionsMenu->setCheckable( true );

  optionsMenu->insertItem(i18n("&General Preferences..."),
						  this, SLOT(slotOptionsGeneral()));
  optionsMenu->insertSeparator();
  idTree = optionsMenu->insertItem(i18n("Show T&ree"),
								   this, SLOT(slotOptionsTree()));
  idToolbar = optionsMenu->insertItem(i18n( "Show &Toolbar"),
									  this, SLOT(slotOptionsToolbar()));
  idLocationbar = optionsMenu->insertItem(i18n("Show &Location"),
										  this, SLOT(slotOptionsLocationbar()) );
  idStatusbar = optionsMenu->insertItem(i18n("Show Status&bar"),
										this, SLOT(slotOptionsStatusbar()) );
  optionsMenu->insertSeparator();
  optionsMenu->insertItem(i18n("&Save Options"),
						  this, SLOT(slotOptionsSave()) );
  
  helpMenu = kapp->getHelpMenu(true,i18n("The KDE Help Center\n\n"
										 "(c) 1998,99\n"
										 "Matthias Elter <me@main-echo.net> \n"));
  
  menuBar()->insertItem(i18n("&File"), fileMenu);
  menuBar()->insertItem(i18n("&Edit"), editMenu);
  menuBar()->insertItem(i18n("&View"), viewMenu);
  menuBar()->insertItem(i18n("&Go"), gotoMenu);
  menuBar()->insertItem(i18n("&Options"), optionsMenu);
  menuBar()->insertItem(i18n("&Bookmarks"), bookmarkMenu);
  menuBar()->insertSeparator();
  menuBar()->insertItem(i18n("&Help"), helpMenu);
}

void HelpCenter::setupToolbar()
{
  historyBackMenu = new QPopupMenu(0L);
  CHECK_PTR(historyBackMenu);
  connect(historyBackMenu, SIGNAL(aboutToShow()), this, SLOT(slotHistoryFillBack()));

  historyForwardMenu = new QPopupMenu(0L);
  CHECK_PTR(historyForwardMenu);
  connect(historyForwardMenu, SIGNAL(aboutToShow()), this, SLOT(slotHistoryFillForward()));

  connect(historyForwardMenu, SIGNAL(activated(int)), this, SLOT(slotHistoryForwardActivated(int)));
  connect(historyBackMenu, SIGNAL(activated(int)), this, SLOT(slotHistoryBackActivated(int)));

  KToolBar *bar = new KToolBar(this, 0, 40);
  addToolBar(bar, 0);

  toolBar(0)->setIconText(3);
  toolBar(0)->insertButton(Icon("hidetabview.xpm"), TB_TREE,
						   true, i18n("Tree off"));

  toolBar(0)->insertSeparator();
  
  toolBar(0)->insertButton(Icon("back.xpm"), TB_BACK,
						   true, i18n("Back"));
  toolBar(0)->setDelayedPopup(TB_BACK, historyBackMenu);

  toolBar(0)->insertButton(Icon("forward.xpm"), TB_FORWARD,
						   true, i18n("Forward"));
  toolBar(0)->setDelayedPopup(TB_FORWARD, historyForwardMenu);
  
  toolBar(0)->insertButton(Icon("reload.xpm"), TB_RELOAD,
						   true, i18n("Reload"));
  toolBar(0)->insertButton(Icon("stop.xpm"), TB_STOP,
						   true, i18n("Stop"));
  
  toolBar(0)->insertSeparator();
  
  toolBar(0)->insertButton(Icon("viewmag+.xpm"), TB_ZOOMIN,
						   true, i18n("Zoom in"));
  toolBar(0)->insertButton(Icon("viewmag-.xpm"), TB_ZOOMOUT,
						   true, i18n("Zoom out"));

  toolBar(0)->insertSeparator();

  toolBar(0)->insertButton(Icon("flag.xpm"), TB_BOOKMARK,
						   true, i18n("Bookmark"));
  toolBar(0)->insertButton(Icon("search.xpm"), TB_FIND,
						   true, i18n("Find"));
  toolBar(0)->insertButton(Icon("fileprint.xpm"), TB_PRINT,
						   true, i18n("Print"));

  // animated wheel
  toolBar(0)->insertButton(Icon("kde1.xpm"), TB_WHEEL, true);
  toolBar(0)->alignItemRight(TB_WHEEL, true);

  if (animatedWheel.count() == 0)
    {
	  animatedWheel.setAutoDelete(true);
	  
	  for ( int i = 1; i <= 9; i++ )
		{
		  QString n;
		  n.sprintf("/kde%i.xpm", i);
		  QPixmap *p = new QPixmap();
		  p->load(kapp->kde_toolbardir() + n);
		  if (p->isNull())
			{
			  QString e;
			  e.sprintf (i18n( "Could not load icon\n%s" ), n.data());
			  QMessageBox::warning( this, i18n("Error"), e.data());
			}
		  animatedWheel.append(p);
		}
    }

  connect(toolBar(0), SIGNAL(clicked(int)), SLOT(slotToolbarClicked(int)));
}

void HelpCenter::setupLocationbar()
{
  toolBar(1)->insertLined(" ", 1, SIGNAL(returnPressed()),
						  this, SLOT( slotLocationEntered()));
  toolBar(1)->setFullWidth(TRUE);
  toolBar(1)->setItemAutoSized(1, TRUE);
  toolBar(1)->enable(KToolBar::Show);
}

void HelpCenter::setupStatusbar()
{
  statusBar()->insertItem(" ", 1);
  enableStatusBar(KStatusBar::Show);
}

void HelpCenter::resizeEvent(QResizeEvent *)
{
	KConfig *config = KApplication::getKApplication()->getConfig();
	config->setGroup("Appearance");
	QString geom;
	geom.sprintf("%dx%d", geometry().width(), geometry().height());
	config->writeEntry("Geometry", geom);

	updateRects();
}

void HelpCenter::saveProperties( KConfig *config )
{
    config->writeEntry( "URL", htmlview->getCurrentURL() );
}

void HelpCenter::readProperties( KConfig *config )
{
    QString url = config->readEntry( "URL" );

    if (!url.isEmpty())
	openURL(url, true);
}

void HelpCenter::slotReadConfig()
{
  KConfig *config = KApplication::getKApplication()->getConfig();
  config->setGroup( "Appearance" );

  // show tool-, location-, statusbar and tree?
  QString o = config->readEntry("ShowTabView");
  if (!o.isEmpty() && o.find("No", 0, false ) == 0)
	showTree = false;
  else
	showTree = true;
  
  o = config->readEntry("ShowToolBar");
  if (!o.isEmpty() && o.find( "No", 0, false) == 0 )
	showToolbar = false;
  else
	showToolbar = true;

  o = config->readEntry("ShowStatusBar");
  if ( !o.isEmpty() && o.find("No", 0, false ) == 0)
	showStatusbar = false;
  else
	showStatusbar = true;

  o = config->readEntry("ShowLocationBar");
  if (!o.isEmpty() && o.find("No", 0, false ) == 0)
	showLocationbar = false;
  else
	showLocationbar = true;

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
  if (!showTree)
	enableTree(false);

  if (showStatusbar)
	enableStatusBar(KStatusBar::Show);
  else
	enableStatusBar(KStatusBar::Hide);
  
  if (showToolbar) 
	enableToolBar(KToolBar::Show, 0);
  else
	enableToolBar(KToolBar::Hide, 0);

  if (showLocationbar)
	  enableToolBar(KToolBar::Show, 1);
  else
	  enableToolBar(KToolBar::Hide, 1);
}

void HelpCenter::enableMenuItems()
{
  bool val;
  
  val = htmlview->canCurrentlyDo(KHelpView::Copy);
  editMenu->setItemEnabled( idCopy, val );
  
  val = htmlview->canCurrentlyDo(KHelpView::GoBack); // history.isback
  gotoMenu->setItemEnabled( idBack, val );
  toolBar(0)->setItemEnabled( TB_BACK, val );
  
  val = htmlview->canCurrentlyDo(KHelpView::GoForward); // history.IsForward
  gotoMenu->setItemEnabled( idForward, val );
  toolBar(0)->setItemEnabled( TB_FORWARD, val );
  
 	
  val = htmlview->canCurrentlyDo(KHelpView::Stop); // busy
  toolBar(0)->setItemEnabled( TB_STOP, val);
  emit setBusy(val);
}

int HelpCenter::openURL( const char *URL, bool withHistory)
{
  htmlview->slotStopProcessing();
  return htmlview->openURL(URL, withHistory);
}

void HelpCenter::fillBookmarkMenu(KFileBookmark *parent, QPopupMenu *menu, int &id)
{
  KFileBookmark *bm;
  for ( bm = parent->getChildren().first(); bm != NULL;
		bm = parent->getChildren().next() )
	{
	  if ( bm->getType() == KFileBookmark::URL )
		{
		  menu->insertItem( bm->getText(), id );
		  id++;
		}
	  else
		{
		  QPopupMenu *subMenu = new QPopupMenu;
		  menu->insertItem( bm->getText(), subMenu );
		  fillBookmarkMenu( bm, subMenu, id );
		}
	}
}

void HelpCenter::enableTree(bool enable)
{
  if (enable)
  	{
	  splitter = new QSplitter(QSplitter::Horizontal, this);
	  setView(splitter, true); 
	  tabview->resize(200, 550);
	  htmlview->resize(600, 550);
	  tabview->recreate(splitter, 0, QPoint(0,0),true);
	  htmlview->recreate(splitter, 0, QPoint(0,0),true);
	  tabview->show();
	  splitter->show();
	  resize(width()+ 205, height());
	}
  else
	{ 
	  tabview->recreate(this, 0, QPoint(0,0),false);
	  htmlview->recreate(this, 0, QPoint(0,0),true);
	  setView(htmlview, true);
	  delete splitter;
	  splitter = 0L;
	  tabview->hide();
	  resize(width() - 205, height());
	}
	updateRects();
}

void HelpCenter::slotSetLocation(const QString& _url)
{
  toolBar(1)->setLinedText(1, _url);
}

void HelpCenter::slotLocationEntered()
{
  htmlview->openURL(toolBar(1)->getLinedText(1), true );
}

void HelpCenter::slotURLSelected(const QString& _url, int _button)
{
  htmlview->openURL( _url, true );
}

void HelpCenter::slotToolbarClicked(int item)
{
  switch (item)
	{
	case TB_TREE:
	  slotOptionsTree();
	  break;
	case TB_WHEEL:
	  slotCloneWindow();
	  break;
	case TB_BOOKMARK:
	  htmlview->slotAddBookmark();
	  break;
	case TB_PRINT:
	  htmlview->slotPrint();
	  break;
	case TB_ZOOMIN:
	  slotMagPlus();
	  break;
	case TB_FIND:
	  htmlview->slotFind();
	  break;
	case TB_ZOOMOUT:
	  slotMagMinus();
	  break;
	case TB_BACK:
	  htmlview->slotBack();
	  break;
	case TB_FORWARD:
	  htmlview->slotForward();
	  break;
	case TB_RELOAD:
	  htmlview->slotReload();
	  break;
	case TB_STOP:
	  htmlview->slotStopProcessing();
	  break;
	}
}

void HelpCenter::slotQuit()
{
  close();
}

void HelpCenter::slotEnableMenuItems()
{
  enableMenuItems();
}

void HelpCenter::slotNewWindow(const QString& url)
{
	HelpCenter *win;

	win = new HelpCenter;
	win->resize(size());
	win->openURL(url);
	win->show();
	enableMenuItems();
}

void HelpCenter::slotCloneWindow()
{
	QString url = htmlview->getCurrentURL();
	HelpCenter *win = new HelpCenter;

	win->resize( size() );

	if (htmlview->canCurrentlyDo(KHelpView::Stop) )
	{
		// in that case, select the title page...
		url = "file:";
		url += kapp->kde_htmldir();
		url += "/default/khelpcenter/main.html";
	}
	win->openURL( url );

	if (!htmlview->canCurrentlyDo(KHelpView::Stop) )
    {
        win->htmlView()->setHistory(htmlview->getHistory() );
		win->htmlView()->setHistCurrent(htmlview->getHistCurrent());
        win->enableMenuItems();
    }

	win->show();
	enableMenuItems();
}

void HelpCenter::slotSetStatusText(const QString& text)
{
  statusBar()->changeItem(text, 1);
}

void HelpCenter::slotBookmarkChanged(KFileBookmark *parent)
{
  bookmarkMenu->clear();
  bookmarkMenu->insertItem(i18n("&Add Bookmark"),
							htmlview, SLOT(slotAddBookmark()) );
  bookmarkMenu->insertSeparator();
  int idStart = BOOKMARK_ID_BASE;
  fillBookmarkMenu( parent, bookmarkMenu, idStart );
}

void HelpCenter::slotSetTitle( const QString& _title )
{
  QString appCaption = "KDE Help Center - ";
  appCaption += _title;
  
  setCaption( appCaption );
}

void HelpCenter::slotSetURL(QString url)
{
  openURL(url, true);
}

void HelpCenter::slotOptionsGeneral()
{
  
}

void HelpCenter::slotOptionsTree()
{
  if (showTree)
  	{
	  enableTree(false);
	  showTree = false;
	  toolBar(0)->setButtonPixmap(TB_TREE, Icon("showtabview.xpm"));
	  toolBar(0)->getButton(TB_TREE)->setText("Tree on");
	}
  else
	{
	  enableTree(true);
	  showTree = true;
	  toolBar(0)->setButtonPixmap(TB_TREE, Icon("hidetabview.xpm"));
	  toolBar(0)->getButton(TB_TREE)->setText("Tree off");
	}
  optionsMenu->setItemChecked(idTree, showTree);
  updateRects();
}

void HelpCenter::slotOptionsToolbar()
{
  if (showToolbar)
	{
	  enableToolBar(KToolBar::Hide, 0);
	  showToolbar = false;
	}
  else
	{
	  enableToolBar(KToolBar::Show, 0);
	  showToolbar = true;
	}
  
  optionsMenu->setItemChecked(idToolbar, showToolbar);
  updateRects();
}

void HelpCenter::slotOptionsLocationbar()
{ 
  if (showLocationbar)
	{
	  enableToolBar(KToolBar::Hide, 1);
	  showLocationbar = false;
	}
  else
	{
	  enableToolBar(KToolBar::Show, 1);
	  showLocationbar = true;
	}
  optionsMenu->setItemChecked(idLocationbar, showLocationbar); 
  updateRects();
}


void HelpCenter::slotOptionsStatusbar()
{
  showStatusbar = !showStatusbar;
  optionsMenu->setItemChecked(idStatusbar, showStatusbar); 
  enableStatusBar(KStatusBar::Toggle);
}

void HelpCenter::slotOptionsSave()
{
  KConfig *config = KApplication::getKApplication()->getConfig();

  config->setGroup( "Appearance" );
  config->writeEntry( "ShowTabView", showTree ? "Yes" : "No" ); 
  config->writeEntry( "ShowToolBar", showToolbar ? "Yes" : "No" );
  config->writeEntry( "ShowStatusBar", showStatusbar ? "Yes" : "No" );  
  config->writeEntry( "ShowLocationBar", showLocationbar ? "Yes" : "No" );

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
	  warning("HelpCenter::slotOptionsSave: illegal default in case reached\n");
	  break;
	}
}

void HelpCenter::slotAnimatedWheelTimeout()
{
  animatedWheelCounter++;
  if (animatedWheelCounter == animatedWheel.count())
	animatedWheelCounter = 0;

  toolBar(0)->setButtonPixmap(TB_WHEEL, *(animatedWheel.at(animatedWheelCounter)));
}

void HelpCenter::slotSetBusy(bool busy)
{
  if (busy) 
    {
	  if(!animatedWheelTimer->isActive())
		{
		  animatedWheelTimer->start(0);
		  printf("--- animatedWheelTimer start ---\n");fflush(stdout);
		}
    }
  else
	{
	  animatedWheelTimer->stop();
	  printf("--- animatedWheelTimer stop ---\n");fflush(stdout);
	}
}

void HelpCenter::slotMagMinus()
{
  if(fontBase > 2)
	{
	  if(fontBase == 5)
		{
		  viewMenu->setItemEnabled(idMagPlus, true);
		  toolBar(0)->setItemEnabled(TB_ZOOMIN, true);
		}
	  fontBase--;
	  htmlview->setDefaultFontBase( fontBase );
	  htmlview->slotReload();
	  if(fontBase == 2)
		{
		  viewMenu->setItemEnabled(idMagMinus, false);
		  toolBar(0)->setItemEnabled(TB_ZOOMOUT, false);
		}		
	}
}

void HelpCenter::slotMagPlus()
{
  if(fontBase < 5)
	{
	  if (fontBase == 2)
		{
		  viewMenu->setItemEnabled(idMagMinus, true);
		  toolBar(0)->setItemEnabled(TB_ZOOMOUT, true);
		}
	  fontBase++;
	  htmlview->setDefaultFontBase(fontBase);
	  htmlview->slotReload();
	  if (fontBase == 5)
		{
		  viewMenu->setItemEnabled(idMagPlus, false);
		  toolBar(0)->setItemEnabled(TB_ZOOMIN, false);
		}
	}
	
}

void HelpCenter::slotHistoryFillBack()
{
  historyBackMenu->clear();
  QList<KPageInfo> history = htmlview->getHistory();
  KPageInfo *current = htmlview->getHistCurrent();
  history.find(current);
  
  KPageInfo *p = history.prev();
  
  while (p != 0)
  	{
	  QString url = p->getUrl();
	  historyBackMenu->insertItem(url, this, 0);
	  p = history.prev();
	}
}

void HelpCenter::slotHistoryFillForward()
{
  historyForwardMenu->clear();
  QList<KPageInfo> history = htmlview->getHistory();
  KPageInfo *current = htmlview->getHistCurrent();
  history.find(current);
  
  KPageInfo *p = history.next();
  
  while (p != 0)
  	{
	  QString url = p->getUrl();
	  historyForwardMenu->insertItem(url, this,0);
	  p = history.next();
	}
}

void HelpCenter::slotHistoryBackActivated(int id)
{
  QList<KPageInfo> history = htmlview->getHistory();
  KPageInfo *current = htmlview->getHistCurrent();
  history.find(current);

  int index = historyBackMenu->indexOf(id);

  KPageInfo *p = 0; 
  
  for (int i = 0; i <= index; i++)
	p = history.prev();

  if (p)
	htmlview->setHistCurrent(p);

  QString url = historyBackMenu->text(id);
  openURL(url, false);
}

void HelpCenter::slotHistoryForwardActivated(int id)
{
  QList<KPageInfo> history = htmlview->getHistory();
  KPageInfo *current = htmlview->getHistCurrent();
  history.find(current);

  int index = historyForwardMenu->indexOf(id);

  KPageInfo *p = 0; 
  
  for (int i = 0; i <= index; i++)
	p = history.next();

  if (p)
	htmlview->setHistCurrent(p);

  QString url = historyForwardMenu->text(id);
  openURL(url, false);
}
