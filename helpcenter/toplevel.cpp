/*
 *  toplevel.cpp - part of the KDE Help Center
 *
 *  Copyright (C) 1999 Matthias Elter (me@kde.org)
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

#include <qkeycode.h>

#include <kiconloader.h>
#include <kstdaccel.h>
#include <kcursor.h>

KHelpOptionsDialog *HelpCenter::optionsDialog = NULL;

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
  
  // read configuration
  readConfig();

  // toggle menu items
  optionsMenu->setItemChecked(idTree, showTree);
  optionsMenu->setItemChecked(idToolbar, showToolbar);
  optionsMenu->setItemChecked(idLocationbar, showLocationbar);
  optionsMenu->setItemChecked(idStatusbar, showStatusbar);

  // connect options dialog
  if ( optionsDialog )
	{
	  connect( optionsDialog->fontOptions, SIGNAL(fontSize( int )),
			   htmlview, SLOT(slotFontSize( int )) );
	  connect( optionsDialog->fontOptions,
			   SIGNAL(standardFont( const QString& )),
			   htmlview, SLOT(slotStandardFont( const QString& )) );
	  connect( optionsDialog->fontOptions,
			   SIGNAL(fixedFont( const QString& )),
			   htmlview, SLOT(slotFixedFont( const QString& )) );
	  connect( optionsDialog->colorOptions,
			   SIGNAL(colorsChanged(const QColor&, const QColor&,
									const QColor&, const QColor&,
									const bool, const bool )),
			   htmlview, SLOT(slotColorsChanged(const QColor&, const QColor&,
											   const QColor&, const QColor&,
											   const bool, const bool)) );
	}
  
  // connect misc. signals
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

  // connect treeview
  connect(treeview , SIGNAL(itemSelected(QString) ),
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
}

HelpCenter::~HelpCenter()
{
  delete htmlview;
  delete treeview;
  if (splitter)
	delete splitter;
}

void HelpCenter::setupView()
{
  splitter = new QSplitter(QSplitter::Horizontal, this);
  CHECK_PTR(splitter);

  treeview = new HTreeView(this);
  htmlview = new KHelpWindow(this);
  
  // I have to use resize and recreate here because QSplitter is messing
  // up the initial sizes otherwise.
  treeview->resize(200, 550);
  htmlview->resize(600, 550);
  treeview->recreate(splitter, 0, QPoint(0,0),true);
  htmlview->recreate(splitter, 0, QPoint(0,0),true);

  CHECK_PTR(treeview);
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
  fileMenu->insertSeparator();
  fileMenu->insertItem( i18n("&Print..."),
						htmlview, SLOT(slotPrint()), stdAccel.print() );
  fileMenu->insertSeparator();
  fileMenu->insertItem( i18n("&Quit"), this,
						SLOT(slotQuit()), stdAccel.quit() );
  
  editMenu = new QPopupMenu;
  CHECK_PTR( editMenu );
  idCopy = editMenu->insertItem(i18n("&Copy"), htmlview, SLOT(slotCopy()), stdAccel.copy());
  editMenu->insertItem(i18n("&Find..."), htmlview, SLOT(slotFind()), stdAccel.find());
  editMenu->insertItem(i18n("Find &next"), htmlview, SLOT(slotFindNext()), Key_F3);
  
  gotoMenu = new QPopupMenu;
  CHECK_PTR(gotoMenu);
  idUp = gotoMenu->insertItem(i18n("&Up"), htmlview, SLOT(slotUp()));
  idBack = gotoMenu->insertItem(i18n("&Back"), htmlview, SLOT(slotBack()));
  idForward = gotoMenu->insertItem(i18n("&Forward"), htmlview, SLOT(slotForward()));
  gotoMenu->insertSeparator();
  gotoMenu->insertItem(i18n("&Contents"), htmlview, SLOT(slotDir()));
  
  viewMenu = new QPopupMenu;
  CHECK_PTR(viewMenu);
  viewMenu->insertItem(i18n("&Reload Tree"), treeview, SLOT(slotReloadTree()));
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
  toolBar(0)->insertButton(Icon("hidetree.xpm"), TB_TREE,
						   true, i18n("Toogle Treeview"));
  toolBar(0)->insertButton(Icon("up.xpm"), TB_UP,
						   true, i18n("Up"));
  toolBar(0)->insertButton(Icon("back.xpm"), TB_BACK,
						   true, i18n("Previous document in history"));
  toolBar(0)->insertButton(Icon("forward.xpm"), TB_FORWARD,
						   true, i18n("Next document in history"));
  toolBar(0)->insertSeparator();
  toolBar(0)->insertButton(Icon("contents.xpm"), TB_INTRO,
						   true, i18n("Introduction"));
  toolBar(0)->insertButton(Icon("reload.xpm"), TB_RELOAD,
						   true, i18n("Reload"));
  toolBar(0)->insertButton(Icon("stop.xpm"), TB_STOP,
						   true, i18n("Stop"));

  connect(toolBar(0), SIGNAL(clicked(int)), SLOT(slotToolbarClicked(int)));
}

void HelpCenter::setupLocationbar()
{
  QString intropage = "file:" + kapp->kde_datadir() + "/khelpcenter/html/intro.html";
  toolBar(1)->insertLined(intropage.data(), 1, SIGNAL(returnPressed()),
						  this, SLOT( slotLocationEntered()));
  toolBar(1)->setFullWidth(TRUE);
  toolBar(1)->setItemAutoSized(1, TRUE);
  toolBar(1)->enable(KToolBar::Show);
}

void HelpCenter::setupStatusbar()
{
  statusBar()->insertItem("Test", 1);
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

void HelpCenter::readConfig()
{
  KConfig *config = KApplication::getKApplication()->getConfig();
  config->setGroup( "Appearance" );

  // show tool-, location-, statusbar and tree?
  QString o = config->readEntry("ShowTreeView");
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
  
  val = htmlview->canCurrentlyDo(KHelpWindow::Copy);
  editMenu->setItemEnabled( idCopy, val );
  
  val = htmlview->canCurrentlyDo(KHelpWindow::GoBack); // history.isback
  gotoMenu->setItemEnabled( idBack, val );
  toolBar(0)->setItemEnabled( TB_BACK, val );
  
  val = htmlview->canCurrentlyDo(KHelpWindow::GoForward); // history.IsForward
  gotoMenu->setItemEnabled( idForward, val );
  toolBar(0)->setItemEnabled( TB_FORWARD, val );
  
 	
  val = htmlview->canCurrentlyDo(KHelpWindow::GoUp); 
  gotoMenu->setItemEnabled( idUp, val );
  toolBar(0)->setItemEnabled( TB_UP, val );
  
  val = htmlview->canCurrentlyDo(KHelpWindow::Stop); // busy
  toolBar(0)->setItemEnabled( TB_STOP, val );
}

int HelpCenter::openURL( const char *URL, bool withHistory)
{
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
	  treeview->resize(200, 550);
	  htmlview->resize(600, 550);
	  treeview->recreate(splitter, 0, QPoint(0,0),true);
	  htmlview->recreate(splitter, 0, QPoint(0,0),true);
	  treeview->show();
	  splitter->show();
	  resize(width()+ 205, height());
	}
  else
	{ 
	  treeview->recreate(this, 0, QPoint(0,0),false);
	  htmlview->recreate(this, 0, QPoint(0,0),true);
	  setView(htmlview, true);
	  delete splitter;
	  treeview->hide();
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
	case TB_BACK:
	  htmlview->slotBack();
	  break;
	case TB_FORWARD:
	  htmlview->slotForward();
	  break;
	case TB_UP:
	  htmlview->slotUp();
	  break;
	case TB_INTRO:
	  htmlview->slotDir();
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

	if (htmlview->canCurrentlyDo(KHelpWindow::Stop) )
	{
		// in that case, select the title page...
		url = "file:";
		url += kapp->kde_htmldir();
		url += "/default/khelpcenter/main.html";
	}
	win->openURL( url );

	if (!htmlview->canCurrentlyDo(KHelpWindow::Stop) )
    {
        win->htmlView()->setHistory(htmlview->getHistory() );
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
  if ( !optionsDialog )
	{
	  optionsDialog = new KHelpOptionsDialog();
	  connect( optionsDialog->fontOptions, SIGNAL(fontSize(int)),
			   htmlview, SLOT(slotFontSize( int )) );
	  connect( optionsDialog->fontOptions,
			   SIGNAL(standardFont( const QString& )),
			   htmlview, SLOT(slotStandardFont( const QString& )) );
	  connect( optionsDialog->fontOptions,
			   SIGNAL(fixedFont( const QString& )),
			   htmlview, SLOT(slotFixedFont( const QString&)) );
	  connect( optionsDialog->colorOptions,
			   SIGNAL(colorsChanged(const QColor&, const QColor&,
									const QColor&, const QColor&,
									const bool, const bool)),
			   htmlview, SLOT(slotColorsChanged(const QColor&, const QColor&,
												const QColor&, const QColor&,
												const bool, const bool)) );
	}
  optionsDialog->show();
}

void HelpCenter::slotOptionsTree()
{
  if (showTree)
  	{
	  enableTree(false);
	  showTree = false;
	  toolBar(0)->setButtonPixmap(TB_TREE, Icon("showtree.xpm"));
	}
  else
	{
	  enableTree(true);
	  showTree = true;
	  toolBar(0)->setButtonPixmap(TB_TREE, Icon("hidetree.xpm"));
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
  config->writeEntry( "ShowTreeView", showTree ? "Yes" : "No" ); 
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
