/*
    This file is part of the KDE Help Center

    Copyright (C) 1998 René Beutler (rbeutler@g26.ethz.ch)
*/


#include <kapp.h>
#include <kprocess.h>
#include <qfile.h>
#include <qmessagebox.h>
#include "helpview.h"

#undef DEBUG

KHelpView::KHelpView(  QWidget *_parent=0, const char *_name=0, int _flags = 0, KHTMLView *_parent_view = 0L, int _defaultFontBase = 3 )
           : KHTMLView( _parent, _name, _flags, _parent_view )
{
	docServer = 0L;

	connectSignals( this );
	defaultFontBase = _defaultFontBase;
#if 0	//not supported by libkhtml
	getKHTMLWidget()->setDefaultFontBase( defaultFontBase );
#endif

#ifdef DEBUG
	printf("KHelpView::KHelpView(%p, %s, %d)\n", _parent, _name, _defaultFontBase); fflush(stdout);
#endif
}

KHelpView::~KHelpView()
{
}

void KHelpView::setBaseDir( QString s )
{
	baseDir = s;
}

void KHelpView::connectSignals( QWidget *w )
{
	connect( w, SIGNAL(documentRequest(KHTMLView*, const char*)),               this, SLOT(slotDocumentRequest(KHTMLView *, const char *)) );
	connect( w, SIGNAL(URLSelected(KHTMLView*, const char*, int, const char*)), this, SLOT(slotURLSelected(KHTMLView*, const char *, int, const char*)) );
//	connect( w, SIGNAL(formSubmitted(const char *, const char *, const char *)), this, SLOT(slotFormSubmitted( const char *, const char *, const char *)) );	
}

KHTMLView* KHelpView::newView( QWidget *_parent = 0L, const char *_name = 0L, int _flags = 0L )
{
#ifdef DEBUG
	printf("KHelpView::newView(%p, %s)\n", _parent, _name); fflush(stdout);
#endif

	KHelpView *v = new KHelpView( _parent, _name, _flags, this, defaultFontBase );
        childViewList.append( v );
		
	return (KHTMLView *) v;
}

void KHelpView::begin( const char *_url, int _x_offset, int _y_offset )
{
	// Delete all frames in this view
	childViewList.clear();
	KHTMLView::begin( _url, _x_offset, _y_offset );
}

void KHelpView::setDefaultFontBase( int size )
{	
	defaultFontBase = size;	
#if 0	//not supported by libkhtml
	getKHTMLWidget()->setDefaultFontBase( defaultFontBase );
#endif
}

void KHelpView::slotDocumentRequest( KHTMLView *_view, const char *_url )
{
#ifdef DEBUG
	printf("KHelpView::slotDocumentRequest(%p, %s)\n", _view, _url); fflush(stdout);
#endif

	if( docServer != 0L )
	{
		delete( docServer );
		docServer = 0L;
	}

	docServer = new KDocumentServer();
	CHECK_PTR( docServer );
	connect( docServer, SIGNAL(requestDone( KDocumentServer::requestType )), this, SLOT(slotDocumentRequestDone( KDocumentServer::requestType )) );
	docServer->request( _view, _url );
}

void KHelpView::slotDocumentRequestDone( KDocumentServer::requestType type )
{
#ifdef DEBUG
	printf("KHelpView::slotDocumentRequestDone()\n"); fflush(stdout);
#endif

	if( docServer != 0L )
	{
		delete( docServer );
		docServer = 0L;
	}

	emit documentDone( type );
}

void KHelpView::slotURLSelected( KHTMLView *_view, const char *_url, int _button, const char *_target )
{
#ifdef DEBUG
	printf("KHelpView::slotURLSelected(%p, %s, %s)\n", _view, _url, _target); fflush(stdout);
#endif

	if( _url == 0L )
		return;

	if( ( _target != 0L) && (strcasecmp( _target, "_popup" ) == 0) )
		return;		// we already handled this in mousePressHook. This is a bit ugly,
				// but I need the mouse position for the pop-up window

//	printf("KHelpView::slotURLSelected.emit URLSelected(%s)\n", _url); fflush(stdout);
	emit URLSelected( _url );

	if( _button == LeftButton )
	{
		if( _target == 0L )
			_view->openURL( _url );
		else {
			KHTMLView *view = findView( _target );
			if( view != 0L )
				view->openURL( _url );
			else {
				QMessageBox::information( this, "Help Assistant",
		                		        QString("tried to display '") + _url + "'\nin nonexistent frame called '" + _target + "'",
		                          		QMessageBox::Ignore | QMessageBox::Default |QMessageBox::Escape);
				return;
			}
		}
	}
}

bool KHelpView::mousePressedHook(const char* _url, const char *_target, QMouseEvent *_ev, bool _isselected )
{
	KHTMLView::mousePressedHook( _url, _target, _ev, _isselected );
	
	if( (_target != 0L) && (strcasecmp( _target, "_popup" ) == 0) && (_url != 0L) )
	{
		quickHelp = new KQuickHelpWindow();
		KURL url( _url );
		QFile file( baseDir + ("/terms") + url.path() + ".txt");
		if( (file.exists() == TRUE) && (file.open( IO_ReadOnly ) == TRUE) )
		{
                	char *buffer = new char [ file.size() + 1 ];
			file.readBlock( buffer, file.size() );		
			buffer[ file.size() ] = '\0';
			quickHelp->popup( buffer, _ev->globalX(), _ev->globalY() );
			file.close();
			return TRUE;
		} else 
		{
			char buffer[] = "<B>Sorry</B>\nCan't find the requested term";
			quickHelp->popup( i18n(buffer), _ev->globalX(), _ev->globalY() );
			return TRUE;
		}
	}

	return FALSE;	// we didn't handle the event
}

bool KHelpView::dndHook( const char *_url, QPoint &_p )
{
	return TRUE;	// disable dnd
}


void KHelpView::slotFormSubmitted( const char *_method, const char *_url, const char *_data )
{
	printf("KHelpView::slotFormSubmitted(%s, %s, %s)\n", _method, _url, _data); fflush(stdout);

	if( strcasecmp(_method, "GET") == 0 )
	{
		QString url( _url );
		url += "?";
		url += _data; 
		emit URLSelected( url );
		openURL( url );
	}
	else
	{
		emit URLSelected( _url );
		openURL( _url );
	}
}

#include "helpview.moc"
