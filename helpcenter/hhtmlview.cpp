/*
 *  hhtmlview.cpp - part of the KDE Help Center
 *
 *  Copyright (C) 1999 Matthias Elter (me@kde.org)
 *
 *  based on code from Martin R. Jones's kdehelp
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

#include <qmessagebox.h>
#include <qclipboard.h>

#include <kapp.h>
#include <kurl.h>
#include <kfm.h>
#include <kfiledialog.h>
#include <kmsgbox.h>
#include <kcursor.h>

#include "hhtmlview.h"

#ifdef HAVE_PATHS_H
#include <paths.h>
#endif

#ifndef _PATH_TMP
#define _PATH_TMP "/tmp"
#endif

// for selection
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>


//-----------------------------------------------------------------------------

KOpenURLDialog::KOpenURLDialog( QWidget *parent, const char *name )
	: QDialog( parent, name )
{
	setCaption( i18n("KDE Help Center: Open URL") );

	QLabel *label = new QLabel( i18n("Open URL"), this );
	label->setGeometry( 20, 20, 60, 20 );
	
	lineEdit = new QLineEdit( this );
	lineEdit->setGeometry( 30+label->width(), 20, 200, 20 );
	lineEdit->setFocus();

	QPushButton *openButton = new QPushButton( i18n("Open"), this );
	openButton->setGeometry( 20, 60, 50, 30 );
	openButton->setDefault( TRUE );
	connect( openButton, SIGNAL(clicked()), SLOT(openPressed()) );

	QPushButton *cancelButton = new QPushButton( i18n("Cancel"), this );
	cancelButton->setGeometry( 290-50, 60, 50, 30 );
	connect( cancelButton, SIGNAL(clicked()), SLOT(reject()) );
}

void KOpenURLDialog::openPressed()
{
	emit openURL( lineEdit->text(), LeftButton );
	accept();
}

//-----------------------------------------------------------------------------

KHistory *KHelpView::urlHistory = 0;

KHelpView::KHelpView( QWidget *parent, const char *name )
    : KHTMLWidget( parent, name )
{
    QString histFile = KApplication::localkdedir() + "/share/apps/khelpcenter/history";

    if ( !urlHistory )
	urlHistory = new KHistory( histFile );
}

KHelpView::~KHelpView()
{
    urlHistory->saveHistory();
}

void
KHelpView::setDefaultFontBase(int fSize)
{
  resetFontSizes();
  if (fSize != 3)
  {
    int fontSizes[7];
    getFontSizes(fontSizes);

    if (fSize > 3)
    {
      for(int i = 0; i < 7; i++)
      {
        int j = i + fSize - 3;
        if ( j > 6)
           j = 6;
    	fontSizes[i] = fontSizes[j]; 
      }
    }
    else
    {
      for(int i = 7; i--;)
      {
        int j = i + fSize - 3;
        if ( j < 0)
           j = 0;
    	fontSizes[i] = fontSizes[j]; 
      }
    }
    setFontSizes(fontSizes);
  }
}

bool KHelpView::URLVisited( const char *_url )
{
    return urlHistory->inHistory( _url );
}

//-----------------------------------------------------------------------------

// statics
QString KHelpWindow::newURL;
KFileBookmarkManager KHelpWindow::bookmarkManager;
int  KHelpWindow::fontBase = 3;
QString KHelpWindow::standardFont;
QString KHelpWindow::fixedFont;
QColor KHelpWindow::bgColor;
QColor KHelpWindow::textColor;
QColor KHelpWindow::linkColor;
QColor KHelpWindow::vLinkColor;
bool KHelpWindow::underlineLinks;
bool KHelpWindow::forceDefaults;

KHelpWindow::KHelpWindow( QWidget *parent, const char *name )
	: QWidget( parent, name ), history(50), format(&html)
{
	openURLDialog = 0;
	CGIServer = 0;
	busy = false;
	scrollTo = 0;
	rmbPopup = 0;
	findDialog = 0;

	readOptions();
	man = new cMan;

	view = new KHelpView( this );
	CHECK_PTR( view );
	view->setDefaultFontBase( fontBase );
	view->setStandardFont( standardFont );
	view->setFixedFont( fixedFont );
	view->setURLCursor( KCursor::handCursor() );
	view->setUnderlineLinks( underlineLinks );
	view->setFocusPolicy( QWidget::StrongFocus );
	view->setFocus();
	view->installEventFilter( this );
	view->setUpdatesEnabled( true );
	view->setDefaultBGColor( bgColor );
	view->setDefaultTextColors( textColor, linkColor, vLinkColor );

	vert = new QScrollBar( 0, 0, 12, view->height(), 0,
			QScrollBar::Vertical, this, "vert" );

	horz = new QScrollBar( 0, 0, 24, view->width(), 0,
			QScrollBar::Horizontal, this, "horz" );

	connect( &bookmarkManager, SIGNAL( changed() ), 
			SLOT( slotBookmarkChanged() ) );

	connect( view, SIGNAL( scrollVert( int ) ), SLOT( slotScrollVert( int ) ) );
	connect( view, SIGNAL( scrollHorz( int ) ), SLOT( slotScrollHorz( int ) ) );
	connect( view, SIGNAL( setTitle(const char *) ),
		        SLOT( slotSetTitle(const char *) ) );
	connect( view, SIGNAL( URLSelected( const char *, int ) ),
			SLOT( slotURLSelected(const char *, int ) ) );
	connect( view, SIGNAL( onURL( const char * ) ),
			SLOT( slotOnURL(const char * ) ) );
	connect( view, SIGNAL( popupMenu( const char *, const QPoint & ) ),
			SLOT( slotPopupMenu(const char *, const QPoint & ) ) );
	connect( view, SIGNAL( formSubmitted( const char *, const char *, const char *, const char * ) ),
			SLOT( slotFormSubmitted(const char *, const char *, const char *, const char * ) ) );
	connect( view, SIGNAL( resized( const QSize & ) ),
			SLOT( slotViewResized( const QSize & ) ) );
	connect( view, SIGNAL( textSelected( bool ) ), 
			SLOT( slotTextSelected( bool ) ) );

	connect( vert, SIGNAL(valueChanged(int)), view, SLOT(slotScrollVert(int)) );
	connect( horz, SIGNAL(valueChanged(int)), view, SLOT(slotScrollHorz(int)) );

	connect( view, SIGNAL( documentChanged() ),
			SLOT( slotDocumentChanged() ) );

	connect( view, SIGNAL( documentDone() ),
			SLOT( slotDocumentDone() ) );

	// load bookmarks
	QString p = KApplication::localkdedir();
	QString bmFile = p + "/share/apps/khelpcenter/bookmarks.html";
	bookmarkManager.read( bmFile );

	QString histFile = p + "/share/apps/khelpcenter/history";

	layout();
}


KHelpWindow::~KHelpWindow()
{
	if (openURLDialog)
		delete openURLDialog;

	if ( findDialog )
        delete findDialog;

	delete man;
}


void KHelpWindow::readOptions()
{
	KConfig *config = KApplication::getKApplication()->getConfig();
	config->setGroup( "Appearance" );

	QString qs = config->readEntry( "BaseFontSize" );
	if ( !qs.isEmpty() )
		fontBase = qs.toInt();

	qs = "times";
	standardFont = config->readEntry( "StandardFont", qs );
	qs = "courier";
	fixedFont = config->readEntry( "FixedFont", qs );

	bgColor = config->readColorEntry( "BgColor", &white );
	textColor = config->readColorEntry( "TextColor", &black );
	linkColor = config->readColorEntry( "LinkColor", &blue );
	vLinkColor = config->readColorEntry( "VLinkColor", &darkMagenta );
	underlineLinks = config->readBoolEntry( "UnderlineLinks", TRUE );
	forceDefaults = config->readBoolEntry( "ForceDefaultColors", TRUE );
}


// Open a URL.  Initiate remote transfer if necessary
int KHelpWindow::openURL( const char *URL, bool withHistory )
{
	char location[256];
	int pos, rv = 1, viewPos = view->yOffset();
	const char *colon;
	bool isRemote = false;

	scrollTo = 0;
	ref = "";

	if ( busy )
		view->setCursor( oldCursor );

	if ( CGIServer )
	{
		delete CGIServer;
		CGIServer = 0;
	}

	if ( !URL )
		return 1;

	if ( URL[0] == '\"')	// skip leading "
		URL++;

	fullURL = URL;

	if ( ( pos = fullURL.findRev( '\"' ) ) > 0)
		fullURL.truncate( pos );

	// is this actually a file:
	if ( (fullURL[0] == '/' && fullURL[1] != '/') || fullURL[0] == '~' )
		fullURL = "file:" + fullURL;

	// extract reference
	int prevPos = currentURL.findRev( '#' );
	prevPos = prevPos >= 0 ? prevPos : currentURL.length();

	int refPos = fullURL.findRev( '#' );
	if ( refPos < 0 )
	{
	    ref = QString::null;
	    refPos = fullURL.length();
	}
	else
	    ref = fullURL.right( fullURL.length() - refPos - 1 );
	fullURL.truncate( refPos );

	if ( fullURL == currentURL.left( prevPos ) )
	{
		if ( !ref.isEmpty() )
		{
		    if ( !view->gotoAnchor( ref ) )
			    return 1;
		    currentURL.truncate( prevPos );
		    currentURL += "#";
		    currentURL += ref;
		}
		else
		    currentURL.truncate( prevPos );

		if ( withHistory )
		{
			if ( history.Current() )
				history.Current()->setOffset( viewPos );
			history.Add( new KPageInfo( currentURL, 0 ) );
			view->urlHistory->addURL( currentURL );
		}
		emit enableMenuItems();
		emit setURL( currentURL );
		emit setLocation( currentURL );
		return 0;
	}

	// if this is a relative location, then use path of current URL
	colon = strchr( fullURL, ':' );
	if ( !colon )
	{
		int i = currentURL.findRev( '/' );
		if ( i >= 0 )
		{
			fullURL = currentURL.left( i + 1 ) + fullURL;
			colon = strchr( fullURL, ':' );
		}
		else
			return 1;
	}

	strcpy( location, colon+1 );

	oldCursor = view->cursor();
	view->setCursor( waitCursor );

	if ( fullURL.find( "file:" ) >= 0 )
	{
		// use internal CGI sever for local CGI stuff
		if ( fullURL.find( "cgi-bin" ) > 0 )
		{
			rv = runCGI( fullURL );
			isRemote = true;
		}
		else
			rv = openFile( location );
	}
	else if ( fullURL.find( "man:" ) >= 0 )
	{
		if ( ( rv = man->ReadLocation( location ) ) == 0 )
			formatMan();
	}
	else if ( fullURL.find( "http:" ) >= 0 )
	{
		KFM *kfm = new KFM;
		kfm->openURL( fullURL );
		delete kfm;
	}
	else if ( fullURL.find( "ftp:" ) >= 0 )
	{
		KFM *kfm = new KFM;
		kfm->openURL( fullURL );
		delete kfm;
	}
	else if ( fullURL.find( "mailto:" ) >= 0 )
	{
		KFM *kfm = new KFM;
		kfm->openURL( fullURL );
		delete kfm;
	}

	if ( !rv )
	{
		busy = true;

		currentURL = fullURL.copy();
		if ( ref.length() > 0 )
		{
			currentURL += '#';
			currentURL += ref;
		}
		if ( withHistory )
		{
			if ( history.Current() )
				history.Current()->setOffset( viewPos );
			history.Add( new KPageInfo( currentURL, 0 ) );
			view->urlHistory->addURL( currentURL );
		}
		emit enableMenuItems();

		if ( !isRemote )
		{
			view->parse();
			horz->setValue( 0 );
			emit setURL( currentURL );
			emit setLocation( currentURL ); 
			view->setCursor( oldCursor );
		}
	}
	else
	    view->setCursor( oldCursor );

	return rv;
}


// actually read the file (must be on local disk)
int KHelpWindow::openFile( const QString &location )
{
	int rv = 1;
	QString fileName;

	if ( location[0] == '~' )
	{
		fileName = getenv( "HOME" );
		fileName += '/';
		fileName += location.data() + 1;
	}
	else
		fileName = location.copy();

	switch ( detectFileType( fileName ) )
	{
		case HTMLFile:
			rv = openHTML( fileName );
			break;
			
		case ManFile:
			if ( ( rv = man->ReadLocation( fileName ) ) == 0 )
				formatMan();
			break;

		case CannotOpenFile:
			{
				QMessageBox mb;
				mb.setText( i18n("Cannot open: ") + fileName );
				mb.setButtonText( QMessageBox::Ok, i18n("Oops!") );
				mb.show();
			}
			break;

		default:
			{
				QMessageBox mb;
				mb.setText( i18n("Unknown format: ") + fileName );
				mb.setButtonText( QMessageBox::Ok, i18n("Error") );
				mb.show();
			}
	}

	return rv;
}

int KHelpWindow::formatMan( int bodyOnly )
{
	if ( !bodyOnly )
	{
		view->setGranularity( 600 );
		view->begin(fullURL);
		view->write( "<html><head><title>" );
		view->write( man->GetTitle() );
		view->write( "</title></head><body>" );
	}

	view->write( man->page() );

	if ( !bodyOnly )
	{
		view->write("</body></html>");
		view->end();
	}

	format = man;

	return 0;
}

int KHelpWindow::openHTML( const char *location )
{
	if ( html.ReadLocation( location ) )
		return 1;

	char buffer[256];
	int val;
	QFile file( location) ;

	if ( !file.open(IO_ReadOnly) )
		return 1;

	view->setGranularity( 600 );
	view->begin( fullURL );

	do
	{
		buffer[0] = '\0';
		val = file.readLine( buffer, 256 );
		if ( strncmp( buffer, "Content-type", 12 ) )
		    view->write(buffer);
	}
	while ( !file.atEnd() );

	view->end();

	format = &html;

	return 0;
}

int KHelpWindow::runCGI( const char *_url )
{
	if ( CGIServer )
	{
		delete CGIServer;
		KURL u( localFile );
		unlink( u.path() );
	}

	CGIServer = new KCGI;

	connect( CGIServer, SIGNAL( finished() ), this, SLOT( slotCGIDone() ) );

	char filename[256];
	sprintf( filename, "%s/khelpcenterXXXXXX", _PATH_TMP );
	mktemp( filename );
	localFile.sprintf( "file:%s", filename );

	CGIServer->get( _url, localFile, "Get" );

	return 0;
}

KHelpWindow::FileType KHelpWindow::detectFileType( const QString &fileName )
{
	FileType type = UnknownFile;

	// attempt to identify file type
	// This is all pretty dodgey at the moment...
	if ( fileName.find( ".htm" ) > 0 && !access( fileName, 0 ) )
	{
		return HTMLFile;
	}
	else
	{
		// open file and attempt to identify
		char fname[256];

		strcpy( fname, fileName );

		if ( strstr( fileName, ".gz" ) )
		{
			char sysCmd[256];
			sprintf( fname, "%s/khelpcenterXXXXXX", _PATH_TMP );
			mktemp( fname );
			sprintf(sysCmd, "gzip -cd %s > %s", (const char *)fileName, fname);
            if ( safeCommand( fileName ) )
                system( sysCmd );
		}
		QFile file( fname );
		char buf[256];
		if ( file.open(IO_ReadOnly) )
		{
			for ( int i = 0; !file.atEnd(), i < 80; i++ )
			{
				file.readLine( buf, 256 );
				QString buffer = buf;
				if ( buffer.find( "<HTML>", 0, FALSE ) >= 0 ||
					buffer.find( "<BODY", 0, FALSE ) >= 0 )
				{
					type = HTMLFile;
					break;
				}
				else if ( buffer.find( ".TH" ) >= 0 ||
						buffer.find( ".so" ) >= 0 )
				{
					type = ManFile;
					break;
				}
			}
		}
		else
			type = CannotOpenFile;
		if ( strstr( fileName, ".gz" ) )
			remove( fname );
	}

	return type;
}


// turn special characters into their HTML equivalents
//
void KHelpWindow::convertSpecial( const char *buffer, QString &converted )
{
	QString special = "<>&\"";
	const char *ptr, *replace[] = { "&lt;", "&gt;", "&amp;", "&quot;", 0 };
	int pos;

	converted = "";

	if ( !buffer )
		return;
	ptr = buffer;

	while ( *ptr )
	{
		if ( (pos = special.find( *ptr )) >= 0 )
			converted += replace[pos];
		else
			converted += *ptr;
		ptr++;
	}
}


QString KHelpWindow::getPrefix()
{
	QString prefix = "";

	int pos = currentURL.find( ':' );

	if ( pos > 0 )
		prefix = currentURL.left( pos+1 );

	return prefix;
}


QString KHelpWindow::getLocation()
{
	QString loc = "";

	int pos1 = currentURL.find( ':' ) + 1;
	int pos2 = currentURL.find( '#' );

	if ( pos2 < 0 )
		pos2 = currentURL.length();
	
	loc = currentURL.mid( pos1, pos2-pos1 );

	return loc;
}


bool KHelpWindow::canCurrentlyDo(AllowedActions action)
{
    switch (action)
	{
	case Copy:        return view->isTextSelected();
	case GoBack:      return history.IsBack();
	case GoForward:   return history.IsForward();
	case GoUp:        return false;
	case Stop:        return busy;
	default: 
	      warning("KHelpWindow::canCurrentlyDo: missing case in \"switch\" statement\n");
	      return FALSE;
	}
	return FALSE; // just to make the compiler happy...
}


const char *KHelpWindow::getCurrentURL()
{
  return (const char *) currentURL;
}


void KHelpWindow::addBookmark( const char *_title, const char *url )
{
	QString bmFile = KApplication::localkdedir() + "/share/apps/khelpcenter/bookmarks.html";
	bookmarkManager.add( _title, url );
	bookmarkManager.write( bmFile );
}


void KHelpWindow::layout()
{
	int top = 0;
	int bottom = height();

	bottom -= SCROLLBAR_WIDTH;

	// ME: added dynamic scrollbars:
	if (view->docWidth() > view->width())
	  horz->show();
	else
	  horz->hide();

	if (view->docHeight() > view->height())
	  vert->show();
	else
		vert->hide();

	if(vert->isVisible() && horz->isVisible())
	  {
		view->setGeometry( 0, top, width() - SCROLLBAR_WIDTH, bottom-top );
		vert->setGeometry( width()-SCROLLBAR_WIDTH, top,
						   SCROLLBAR_WIDTH, bottom-top );
		horz->setGeometry( 0, bottom,
						   width() - SCROLLBAR_WIDTH, SCROLLBAR_WIDTH );
	  }
	else if (vert->isVisible())
	  {
		view->setGeometry( 0, top, width() - SCROLLBAR_WIDTH, height()-top );
		vert->setGeometry( width()-SCROLLBAR_WIDTH, top,
						   SCROLLBAR_WIDTH, height()-top );
	  }
	else if (horz->isVisible())
	  {
		view->setGeometry( 0, top, width(), bottom-top );
		horz->setGeometry( 0, bottom,
						   width(), SCROLLBAR_WIDTH );
	  }
	else
	  view->setGeometry( 0, top, width(), height()-top );
}


void KHelpWindow::slotOpenFile()
{
	QString fileName = KFileDialog::getOpenFileName();

	if ( !fileName.isNull() )
	{
		QString url = "file:";
		url += fileName;
		openURL( url );
	}
}


void KHelpWindow::slotOpenURL()
{
	if (!openURLDialog)
	{
		openURLDialog = new KOpenURLDialog();
		connect( openURLDialog, SIGNAL(openURL( const char *, int )),
								SLOT(slotURLSelected( const char *, int )) );
	}

	openURLDialog->show();
}


void KHelpWindow::slotSearch()
{
  	QMessageBox mb;
	mb.setText( i18n("Sorry not implemented!"));
	mb.setButtonText( QMessageBox::Ok, i18n("Oops!") );
	mb.show();
}

void KHelpWindow::slotReload()
{
  QString url = currentURL;
  currentURL = "";
  openURL(url);
}


void KHelpWindow::slotPrint()
{
	view->print();
}


void KHelpWindow::slotCopy()
{
	QString text;

	view->getSelectedText( text );
	QClipboard *cb = kapp->clipboard();
	cb->setText( text );
}

void KHelpWindow::slotFind()
{
    // if we haven't already created a find dialog then do it now
    if ( !findDialog )
    {
	findDialog = new KFindTextDialog();
	connect( findDialog, SIGNAL( find( const QRegExp & ) ),
			     SLOT( slotFindNext( const QRegExp & ) ) );
    }

    // reset the find iterator
    view->findTextBegin();

    findDialog->show();
}

void KHelpWindow::slotFindNext()
{
    if ( findDialog && !findDialog->regExp().isEmpty() )
    {
	// We have a find dialog, so use the reg exp it maintains.
	slotFindNext( findDialog->regExp() );
    }
    else
    {
	// no find has been attempted yet - open the find dialog.
	slotFind();
    }
}

void KHelpWindow::slotFindNext( const QRegExp &regExp )
{
    if ( !view->findTextNext( regExp ) )
    {
	// We've reached the end of the document.
	// Can either stop searching and close the find dialog,
	// or start again from the top.
	if ( KMsgBox::yesNo( this, i18n( "Find Complete" ),
	    i18n( "Continue search from the top of the page?" ),
	    KMsgBox::DB_SECOND | KMsgBox::QUESTION ) == 1 )
	{
	    view->findTextBegin();
	    slotFindNext( regExp );
	}
	else
	{
	    view->findTextEnd();
	    findDialog->hide();
	}
    }
}

void KHelpWindow::slotBack()
{
	if ( history.Current() )
		history.Current()->setOffset( view->yOffset() );

	KPageInfo *p = history.Back();

	if ( p )
	{
		if ( !openURL( p->getUrl(), false ) )
		{
		    scrollTo = p->getOffset();
		    if ( !busy )
		    {
			view->slotScrollVert( scrollTo );
			vert->setValue( scrollTo );
		    }
		}
	}
}


void KHelpWindow::slotForward()
{
	if ( history.Current() )
		history.Current()->setOffset( view->yOffset() );

	KPageInfo *p = history.Forward();

	if ( p )
	{
		if ( !openURL( p->getUrl(), false ) )
		{
		    scrollTo = p->getOffset();
		    if ( !busy )
		    {
			view->slotScrollVert( scrollTo );
			vert->setValue( scrollTo );
		    }
		}
	}
}


void KHelpWindow::slotDir()
{
    QString initDoc = "file:";
    initDoc += kapp->findFile( "/share/doc/HTML/default/khelpcenter/main.html" );

    openURL( initDoc );
}

void KHelpWindow::slotUp()
{

}

void KHelpWindow::slotTextSelected( bool )
{
	emit enableMenuItems();

	XSetSelectionOwner( qt_xdisplay(), XA_PRIMARY, handle(), CurrentTime );
}

void KHelpWindow::slotAddBookmark()
{
	addBookmark( title, currentURL );
}


void KHelpWindow::slotBookmarkSelected( int id )
{
	id -= BOOKMARK_ID_BASE;

	KFileBookmark *bm = bookmarkManager.getBookmark( id );

	if ( bm )
		openURL( bm->getURL() );
}


void KHelpWindow::slotBookmarkHighlighted( int id )
{
	id -= BOOKMARK_ID_BASE;

	KFileBookmark *bm = bookmarkManager.getBookmark( id );

	if ( bm )
	{
		emit setURL( bm->getURL() );
	}
	else
	{
		emit setURL( currentURL );
	}
}


void KHelpWindow::slotBookmarkChanged()
{
	emit bookmarkChanged(bookmarkManager.getRoot());
}


void KHelpWindow::slotStopProcessing()
{
	if ( !busy )
		return;

	if ( CGIServer )
	{
		delete CGIServer;
		KURL u( localFile );
		unlink( u.path() );
		CGIServer = NULL;
		view->setCursor( oldCursor );
	}

	view->stopParser();

	busy = false;
	emit enableMenuItems();
}

void KHelpWindow::slotSetTitle( const char *_title )
{
	title = _title;
	emit setTitle(_title);
}


void KHelpWindow::slotURLSelected( const char *URL, int button )
{
	if ( button == LeftButton )
	{
		openURL( URL );
		view->setFocus();
	}
	else if ( button == MidButton )
	{
        emit openNewWindow( URL );
	}
}


void KHelpWindow::slotOnURL( const char *url )
{
  if ( url )
	emit setURL( url );
  else
	emit setURL( currentURL );
}


void KHelpWindow::slotFormSubmitted( const char *_method, const char *_url, const char* _data, const char * _target )
{
    if (strcasecmp(_method, "GET")==0)
    {
      QString url( _url );
      url += "?";
      url += _data; 
      openURL( url );
    }
    else
      openURL( _url );
}


void KHelpWindow::slotPopupMenu( const char *url, const QPoint &p )
{
	int id;

	if ( rmbPopup )
		delete rmbPopup;

	rmbPopup = new QPopupMenu;
	rmbPopup->installEventFilter( this );

	id = rmbPopup->insertItem( i18n("Back"), this, SLOT( slotBack() ) );
	rmbPopup->setItemEnabled( id, history.IsBack() );
	id = rmbPopup->insertItem( i18n("Forward"), this, SLOT( slotForward() ) );
	rmbPopup->setItemEnabled( id, history.IsForward() );
	
	if ( url )
	{
		rmbPopup->insertSeparator();
		int pos;
		if ( url[0] == '\"' )
		  url++;
		newURL = url;
		if ( ( pos  = newURL.findRev( '\"' ) ) > 0)
		  newURL.truncate( pos );
		rmbPopup->insertItem(i18n("Open this Link"),this,SLOT(slotPopupOpenURL()));
		rmbPopup->insertItem(i18n("Add Bookmark"),this,SLOT(slotPopupAddBookmark()));
		rmbPopup->insertItem(i18n("Open in new Window"),this,SLOT(slotPopupOpenNew()));
	}

	rmbPopup->popup( p );
}

void KHelpWindow::slotCGIDone()
{
	view->setCursor( oldCursor );
	KURL u( localFile );
	if ( !openFile( u.path() ) )
		view->parse();
	emit setURL( currentURL );
	emit setLocation( currentURL ); 

	delete CGIServer;
	CGIServer = 0;

	unlink( u.path() );
}


void KHelpWindow::slotScrollVert( int _y )
{
	vert->setValue( _y );
}


void KHelpWindow::slotScrollHorz( int _x )
{
	horz->setValue( _x );
}


void KHelpWindow::slotBackgroundColor( const QColor &col )
{
	view->setBackgroundColor( col );
}


void KHelpWindow::slotFontSize( int size )
{
	fontBase = size;
	view->setDefaultFontBase( size );
	slotReload();
	busy = true;
	emit enableMenuItems();
}


void KHelpWindow::slotStandardFont( const QString& n )
{
	standardFont = n;
	view->setStandardFont( n );
	//view->parse();
	slotReload();
	busy = true;
	emit enableMenuItems();
}


void KHelpWindow::slotFixedFont( const QString& n )
{
	fixedFont = n;
	view->setFixedFont( n );
	slotReload();
	busy = true;
	emit enableMenuItems();
}


void KHelpWindow::slotColorsChanged( const QColor &bg, const QColor &text,
	const QColor &link, const QColor &vlink, const bool uline, const bool force)
{
	view->setDefaultBGColor( bg );
	view->setDefaultTextColors( text, link, vlink );
	view->setUnderlineLinks(uline);
	slotReload();
	busy = true;
	emit enableMenuItems();
}


void KHelpWindow::slotPopupOpenURL()
{
	openURL( newURL );
}


void KHelpWindow::slotPopupAddBookmark()
{
	addBookmark( newURL, newURL );
}


void KHelpWindow::slotPopupOpenNew()
{
	emit openNewWindow( newURL );
}


void KHelpWindow::slotViewResized( const QSize & )
{
	QApplication::setOverrideCursor( waitCursor );

	vert->setSteps( 12, view->height() - 20 ); 
	horz->setSteps( 24, view->width() );

	if ( view->docHeight() > view->height() )
		vert->setRange( 0, view->docHeight() - view->height() );
	else
		vert->setRange( 0, 0 );

	QApplication::restoreOverrideCursor();
}


void KHelpWindow::resizeEvent( QResizeEvent * )
{
	layout();
}

// all this just so that the rmb popup menu doesn't disappear
bool KHelpWindow::eventFilter( QObject *obj, QEvent *ev )
{
	static QPoint tmpPoint;

	if ( obj == rmbPopup )
	{
		if ( ev->type() == QEvent::MouseButtonRelease )
		{
			if ( QCursor::pos() == tmpPoint )
			{
				tmpPoint = QPoint( -10, -10 );
				return true;
			}
		}
	}

	if ( obj == view )
	{
		switch ( ev->type() )
		{
			case QEvent::MouseButtonPress:
			case QEvent::MouseButtonDblClick:
				tmpPoint = QCursor::pos();
				break;

			case QEvent::MouseButtonRelease:
				tmpPoint = QPoint(-10,-10);
				break;
		        default:
			  break;
		}

	}

	return false;
}

bool KHelpWindow::x11Event( XEvent *xevent )
{
    switch ( xevent->type )
    {
	case SelectionRequest:
	    {
		if ( view->isTextSelected() )
		{
		    QString text;
		    view->getSelectedText( text );
		    XSelectionRequestEvent *req = &xevent->xselectionrequest;
		    XEvent evt;
		    evt.xselection.type = SelectionNotify;
		    evt.xselection.display  = req->display;
		    evt.xselection.requestor = req->requestor;
		    evt.xselection.selection = req->selection;
		    evt.xselection.target = req->target;
		    evt.xselection.property = None;
		    evt.xselection.time = req->time;
		    if ( req->target == XA_STRING )
		    {
			XChangeProperty ( qt_xdisplay(), req->requestor,
				req->property, XA_STRING, 8, PropModeReplace,
				(uchar *)text.data(), text.length() );
			evt.xselection.property = req->property;
		    }
		    XSendEvent( qt_xdisplay(), req->requestor, False, 0, &evt );
		}

		return true;
	    }
	    break;

	case SelectionClear:
	    // Do we want to clear the selection???
	    view->selectText( 0, 0, 0, 0 );
	    break;
    }

    return false;
}

// called as html is parsed
void KHelpWindow::slotDocumentChanged()
{
  if ( view->docHeight() > view->height() )
	vert->setRange( 0, view->docHeight() - view->height() );
  else
	vert->setRange( 0, 0 );
  
  if ( view->docWidth() > view->width() )
	horz->setRange( 0, view->docWidth() - view->width() );
  else
	horz->setRange( 0, 0 );

  layout();
}


// called when all html has been parsed
void KHelpWindow::slotDocumentDone()
{
	if ( scrollTo )
	{
		if ( scrollTo > view->docHeight() - view->height() )
			scrollTo = view->docHeight() - view->height();
		view->slotScrollVert( scrollTo );
		vert->setValue( scrollTo );
	}
	else if ( !ref.isEmpty() )
	{
		if ( !view->gotoAnchor( ref ) )
			vert->setValue( 0 );
	}

	layout();
	
	busy = false;
	emit enableMenuItems();
}

//-----------------------------------------------------------------------------

