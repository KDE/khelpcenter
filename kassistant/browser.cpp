/*
    This file is part of the KDE Help Center

    Copyright (C) 1998 René Beutler (rbeutler@g26.ethz.ch)
*/

#include <qdir.h>
#include <qmessagebox.h>
#include <qwhatsthis.h>
#include <qcursor.h>
#include <qkeycode.h>
#include <qclipboard.h>
#include <qfileinfo.h>
#include <qtoolbutton.h>
#include <qcolor.h>
#include <qpalette.h>
#include <qwindowdefs.h>
#include <qmovie.h>

#include <kiconloader.h>
#include <kwm.h>
#include <kcursor.h>
#include <kquickhelp.h>
#include <kconfig.h>
#include <kprocess.h>

#include "browser.h"
#include "assistant.h"
#include "config-kassistant.h"
#include "mypushbutton.h"

#undef DEBUG

extern KConfig *pConfig;


int Browser::fontBase(3);


Browser::Browser() : KTMainWindow()
{
	setCaption( i18n("Help Assistant") );

	setupToolBar();
	setupAccelerators();

	// variables init


	// set up main widget
	view = new KHelpView( this );
	CHECK_PTR( view );
	connect( view, SIGNAL(URLSelected( const char* )), this, SLOT(slotURLSelected( const char* )) );
	connect( view, SIGNAL(documentDone(KDocumentServer::requestType)),  this, SLOT(slotDocumentDone(KDocumentServer::requestType)) );
	connect( view, SIGNAL(setTitle(const char *)),     this, SLOT(slotSetTitle(const char*)) );
	connect( view, SIGNAL(textSelected(KHTMLView*, bool)), this, SLOT(slotTextSelected(KHTMLView*, bool)) );
	connect( view, SIGNAL(popupMenu(KHTMLView *, const char *, const QPoint &)), this, SLOT(slotPopupMenu(KHTMLView *, const char *, const QPoint &)) );
	connect( view->getKHTMLWidget(), SIGNAL(documentDone()),  this, SLOT(slotHTMLWidgetDone()) );

	slotConfigure();

	// get our help file
//	QString html = "/tutorial.html"; 	QString app  = "/kassistant";
	QString html = "/question.html";	QString app  = "/khelpcenter";
//	QString html = "/lockscreen.html";	QString app = "/khelpcenter";
	QString path = KApplication::kde_htmldir().copy() + "/";
	QString file = path + klocale->language() + app + html;
	if( QFileInfo( file ).exists() )
		path += klocale->language() + app;
	else
		path += "default" + app;
	homeURL = QString("file:") + path + html;
	view->setBaseDir( path );
	setURL( homeURL );

	setView( view );
	setMinimumSize( 100, 0 );

	resize( 400, 300 );	// --> should be calculated, approx. 1/3 of height?
	recreate( 0L, WType_TopLevel | WStyle_Customize | WStyle_DialogBorder, pos() );


	KWM::setDecoration( winId(), KWM::normalDecoration | KWM::staysOnTop );
}

void Browser::setupToolBar()
{
	KIconLoader iconLoader;

	historyPopup = new QPopupMenu( 0L );
	CHECK_PTR( historyPopup );
	connect( historyPopup, SIGNAL(activated(int)), this, SLOT(slotHistoryActivated(int)) );

	// Toolbar 0
	KToolBar *toolbar = toolBar( 0 );
	toolbar->setBarPos( KToolBar::Top );
	toolbar->setFullWidth( TRUE );
//	toolbar->insertButton(iconLoader.loadIcon("exit.xpm"     ), ID_CLOSE,      TRUE, i18n("Close") );
//	toolbar->insertSeparator();
	toolbar->insertButton(iconLoader.loadIcon("back.xpm"     ), ID_BACK,      FALSE, i18n("Back") );	
	toolbar->setDelayedPopup( ID_BACK, historyPopup );
//	toolbar->insertSeparator();
//	toolbar->insertButton(iconLoader.loadIcon("idea.xpm" ),    ID_QUESTION,  TRUE, i18n("Ask") );
//	toolbar->insertSeparator();
//	toolbar->insertButton(iconLoader.loadIcon("openbook.xpm" ), ID_INDEX,     TRUE, i18n("Index") );

//	toolbar->insertButton(iconLoader.loadIcon("stop.xpm" ),     ID_STOP,   TRUE,   i18n("Freeze") );
//	toolbar->setToggle4( ID_STOP, TRUE);

//	toolbar->insertSeparator();
	cmdPopup = new QPopupMenu();
	CHECK_PTR( cmdPopup );
	cmdPopup->insertItem( iconLoader.loadIcon("viewmag+.xpm"), i18n("Zoom in") ,  this, SLOT(slotMagPlus()),  0, ID_MAGPLUS );
	cmdPopup->insertItem( iconLoader.loadIcon("viewmag-.xpm"), i18n("Zoom out"),  this, SLOT(slotMagMinus()), 0, ID_MAGMINUS );
	cmdPopup->insertItem( iconLoader.loadIcon("reload.xpm"),   i18n("Reload"),    this, SLOT(slotReload()),   0, ID_RELOAD );
	cmdPopup->insertSeparator();
	cmdPopup->insertItem( iconLoader.loadIcon("attach.xpm"),   i18n("Annotate"), this, SLOT(slotAttach()),   0, ID_ATTACH );
	cmdPopup->insertItem( iconLoader.loadIcon("editcopy.xpm"), i18n("Copy"),      this, SLOT(slotCopy()),     0, ID_COPY );
	cmdPopup->setItemEnabled( ID_COPY, FALSE );		
	cmdPopup->insertSeparator();
//	cmdPopup->insertItem( iconLoader.loadIcon("flag.xpm"),     i18n("Add to Bookmarks") );
//	cmdPopup->insertSeparator();	
	cmdPopup->insertItem( iconLoader.loadIcon("fileprint.xpm"),i18n("Print"),     this, SLOT(slotPrint()),    0, ID_PRINT );

	toolbar->insertButton( iconLoader.loadIcon("empty.xpm"),  ID_CMDPOPUP, cmdPopup, TRUE, i18n("Commands") );
//	toolbar->alignItemRight( ID_CMDPOPUP );
	toolbar->setIconText(1);

	// signal slot stuff for toolbar 1
	toolbar->addConnection( ID_BACK,      SIGNAL(clicked()), this, SLOT(slotBack()) );
	toolbar->addConnection( ID_STOP,      SIGNAL(clicked()), this, SLOT(slotFreeze()) );
}

void Browser::setupAccelerators()
{
	accel = new QAccel( this );
	accel->insertItem( Key_Escape,          ID_CLOSE );
	accel->connectItem( ID_CLOSE, this, SLOT(slotClose()) );

	kAccel = new KAccel( this );
	kAccel->connectItem( KAccel::Copy,  this, SLOT( slotCopy() ) );
	kAccel->connectItem( KAccel::Print, this, SLOT( slotPrint() ) );
	kAccel->connectItem( KAccel::Quit,  this, SLOT( slotExit() ) );
	kAccel->connectItem( KAccel::Close, this, SLOT( slotClose() ) );
	kAccel->connectItem( KAccel::Help,  this, SLOT( slotHelp() ) );
	kAccel->readSettings();
}

void Browser::setupStatusBar()
{
}

void Browser::closeEvent( QCloseEvent *e )
{
//	e->ignore();
	hide();
}

void Browser::resizeEvent( QResizeEvent *e )
{
	updateRects();
}

void Browser::setURL( QString url, bool reload )
{
#ifdef DEBUG
	printf("\nBrowser::setURL(%s, %d)", (const char *)url, reload); fflush(stdout);
#endif

	if( (reload == TRUE) || (currentURL != url) )
	{
		currentURL = selectedURL = url;
		view->openURL( (const char *) url );
	}
}

void Browser::setBaseDir( QString dir )
{
	view->setBaseDir( dir );
}

void Browser::adjustBrowserSize()
{
#ifdef DEBUG
	printf("Browser::adjustBrowserSize\n"); fflush(stdout);
#endif

	// adjust the size of the main window accordingly to fit the document
	show();
	int height = view->getKHTMLWidget()->docHeight() + view->y();

	// don't get too big
	int h;
	if( height >= (h = QApplication::desktop()->height()/2) )
		height = h;

	resize( width(), height );

	// maybe the window has to be repositioned...
	QRect geom = frameGeometry();		
	QRect region = KWM::getWindowRegion( KWM::currentDesktop() );

	if( region.contains( geom ) == FALSE )
	{
		// actually, to check against bottom would be enough, but
		// let's help the user a bit...
		int d, dx = 0, dy = 0;
		if( (d = region.top() - geom.top() ) > 0 )	
			dy = d;		
		if( (d = region.bottom() - geom.bottom() ) < 0 )
			dy = d;
		if( (d = region.left() - geom.left() ) > 0 )
			dx = d;
		if( (d = region.right() - geom.right() ) < 0 )
			dx = d;

		move( x()+dx, y()+dy );
	}
}

// ---------------------------- Application related SLOTS -------------------------------------

void Browser::slotConfigure()
{
	QColor bgColor, textColor, linkColor, vLinkColor;
	bool changeCursor, underlineLinks, forceDefaults;

	pConfig->reparseConfiguration();
	pConfig->setGroup("Browser");
	bgColor        = pConfig->readColorEntry( "BgColor",           &HTML_DEFAULT_BG_COLOR );
	textColor      = pConfig->readColorEntry( "TextColor",         &HTML_DEFAULT_TXT_COLOR );
	linkColor      = pConfig->readColorEntry( "LinkColor",         &HTML_DEFAULT_LNK_COLOR );
	vLinkColor     = pConfig->readColorEntry( "VLinkColor",        &HTML_DEFAULT_VLNK_COLOR);
	changeCursor   = pConfig->readBoolEntry(  "ChangeCursor",       HTML_DEFAULT_CHANGE_CURSOR);
	underlineLinks = pConfig->readBoolEntry(  "UnderlineLinks",     HTML_DEFAULT_UNDERLINE_LINKS);
	forceDefaults  = pConfig->readBoolEntry(  "ForceDefaultColors", HTML_DEFAULT_FORCE_DFLT_COLORS);

	KHTMLWidget *htmlWidget = view->getKHTMLWidget();
	htmlWidget->setDefaultBGColor( bgColor );
	htmlWidget->setDefaultTextColors( textColor, linkColor, vLinkColor );
	htmlWidget->setForceDefault( forceDefaults );
	htmlWidget->setUnderlineLinks( underlineLinks );
	htmlWidget->setGranularity(10000);

	if( changeCursor )
		htmlWidget->setURLCursor( KCursor::handCursor() );
	else
		htmlWidget->setURLCursor( KCursor::arrowCursor() );

	view->setDefaultFontBase( fontBase );

	slotReload();
}

void Browser::slotExit()
{
	kapp->quit();
}

void Browser::slotClose()
{
	hide();
}

void Browser::slotSettings()
{
        KProcess proc;
        proc << "kcmhelpcenter";
        proc << "{" << "browser" << "," << "widget" << "}";
        proc.start( KProcess::DontCare, KProcess::NoCommunication );
}

void Browser::slotAbout()
{
	Assistant::about();
}

void Browser::slotPopupMenu( KHTMLView *_view, const char *_url, const QPoint &_point )
{
#ifdef DEBUG
	printf("\nBrowser::slotPopupMenu()"); fflush(stdout);
#endif DEBUG

	QPopupMenu *popup = new QPopupMenu();
	popup->insertItem( i18n("Settings..."), this, SLOT(slotSettings()) );
	popup->insertSeparator();
	popup->insertItem( i18n("About kassistant..."), this, SLOT(slotAbout()) );
	popup->exec(QCursor::pos());
}

void Browser::slotBack()
{
	ASSERT( URLHistory.count() > 0 );

	URLHistory.remove();	// remove current
	historyPopup->removeItemAt( historyPopup->count() - 1 );

	QString url = URLHistory.current();	
	URLHistory.remove();	// remove this too, it will be re-inserted in slotDocumentDone
	historyPopup->removeItemAt( historyPopup->count() - 1 );

	setURL( url );

	if( URLHistory.count() < 2 )
		toolBar( 0 )->setItemEnabled( ID_BACK, FALSE );
}

void Browser::slotPrint()
{
	view->print();
}

void Browser::slotHelp()
{
	setURL( homeURL );
}

void Browser::slotMagMinus()
{
	if( fontBase > 2 )
	{
		if( fontBase == 5 )
			cmdPopup->setItemEnabled( ID_MAGPLUS, TRUE );
		fontBase--;
		view->setDefaultFontBase( fontBase );
		slotReload();
		if( fontBase == 2 )
			cmdPopup->setItemEnabled( ID_MAGMINUS, FALSE );		
	}
}

void Browser::slotMagPlus()
{
	if( fontBase < 5 )
	{
		if( fontBase == 2 )
			cmdPopup->setItemEnabled( ID_MAGMINUS, TRUE );
		fontBase++;
		view->setDefaultFontBase( fontBase );
		slotReload();
		if( fontBase == 5 )
			cmdPopup->setItemEnabled( ID_MAGPLUS, FALSE );
	}
	
}

void Browser::slotAttach()
{
	KNoteEditor *edit = new KNoteEditor( this, i18n("Note Editor") );
	edit->setFile( "kfm", "index.html" );
	int r = edit->exec();

	delete edit;

	if( r == QDialog::Accepted )
		slotReload();
}

void Browser::slotReload()
{
	// remove current, it will be re-inserted in slotDocumentDone
	if( !URLHistory.isEmpty() )
	{
		URLHistory.remove();	
		historyPopup->removeItemAt( historyPopup->count() - 1 );

		setURL( currentURL, TRUE );
	}
}

void Browser::slotEmptyQuestion()
{
//	toolBar( 1 )->setLinedText( ID_QUESTIONLINE, "" );
//	toolBar( 1 )->getWidget( ID_QUESTIONLINE )->setFocus();	
}

void Browser::slotQuestionAsked()
{
	printf("Question was: %s\n", (const char *)(toolBar( 1 )->getLinedText( ID_QUESTIONLINE )) );
}

void Browser::slotQuestion()
{
#ifdef DEBUG
	printf("Browser::slotQuestion() ");
#endif
/*
	// if there is no dialog, create one, else raise it
	if( questDialog == 0L )
	{	
		printf("create new dialog\n"); fflush(stdout);
		questDialog = new KQuestionDialog( parentWidget(), i18n("Ask a Question") );	
		questDialog->show();
	} else
	{
		printf("raise existing dialog\n"); fflush(stdout);
		questDialog->show();
		questDialog->raise();
	}
*/
}

void Browser::slotHistoryActivated( int id )
{
#ifdef DEBUG
	printf("Browser::slotHistoryActivated(%d)\n", id); fflush(stdout);
#endif

	// did the user select the current entry?
	if( (uint)(id+1) == URLHistory.count() )
		return;

	QString url = QString( URLHistory.at( (uint)id ) );

	// remove 
	for( signed int i=URLHistory.count()-1; i >= (signed int)id; i-- )
	{
		historyPopup->removeItemAt( i );
		URLHistory.remove();
	}

	if( URLHistory.count() < 2 )
		toolBar( 0 )->setItemEnabled( ID_BACK, FALSE );

	setURL( url );
}

void Browser::slotCopy()
{
#ifdef DEBUG
	printf("Browser::slotCopy()\n"); fflush(stdout);
#endif

	QString text;

	view->getSelectedText( text );
	QClipboard *cb = kapp->clipboard();
	cb->setText( text );
}

void Browser::slotFreeze()
{
}

// ------------------------ HTMLWidget related SLOTS ------------------------------

void Browser::slotTextSelected( KHTMLView *_view, bool _selected )
{
	cmdPopup->setItemEnabled( ID_COPY, _selected );			
	slotCopy();
}

void Browser::slotURLSelected( const char *_url )
{
#ifdef DEBUG
	printf("Browser::slotURLSelected(%s)\n", _url); fflush(stdout);
#endif

	selectedURL = QString( _url );
}

void Browser::slotDocumentDone( KDocumentServer::requestType type )
{
#ifdef DEBUG
	printf("Browser::slotDocumentDone(%d)\n", type); fflush(stdout);
#endif

	if( type == KDocumentServer::FILE )
	{
		currentURL = selectedURL;
		URLHistory.append( currentURL );
		if( URLHistory.count() > 1 )
			toolBar( 0 )->setItemEnabled( ID_BACK, TRUE );

//		toolBar( 0 )->setItemEnabled( ID_FORWARD, FALSE );

		historyPopup->insertItem( (const char*) currentURL );
	}	
}

void Browser::slotHTMLWidgetDone()
{
#ifdef DEBUG
	printf("Browser::slotHTMLWidgetDone()\n"); fflush(stdout);
#endif

	historyPopup->changeItem( documentTitle, URLHistory.count()-1 );
	adjustBrowserSize();
}

void Browser::slotSetTitle( const char *_title )
{
#ifdef DEBUG
//	printf("Browser::slotSetTitle( %s )\n", _title ); fflush(stdout);
#endif
	documentTitle = _title;

}

#include "browser.moc"
