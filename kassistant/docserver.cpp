/*
    This file is part of the KDE Help Center

    Copyright (C) 1998 René Beutler (rbeutler@g26.ethz.ch)

    The class KDocumentServer serves as document server
*/


#include <kprocess.h>
#include <kurl.h>
#include <qfile.h>
#include <kmessagebox.h>
#include "docserver.h"

#include <unistd.h> // you need this for mktemp()

#ifdef HAVE_PATHS_H
#include <paths.h>
#endif

#ifndef _PATH_TMP
#define _PATH_TMP "/tmp"
#endif

#undef DEBUG

KDocumentServer::KDocumentServer()
{
	CGIServer = 0L;
}

void KDocumentServer::request( KHTMLView *_view, const char *_url )
{
#ifdef DEBUG
	printf("KDocumentServer::request(%s)\n", _url); fflush(stdout);
#endif

	KURL url( _url );
	
	if( strcasecmp(url.protocol(), "file" ) == 0 )
	{				
		if( QString( _url ).find( "cgi-bin" ) > 0 )
		{
			CGI( _view, _url );
		}
		else
		{
			file( _view, _url );
		}
	}
	else if( strcasecmp(url.protocol(), "command") == 0 )
	{
		command( url.path() + 1 );	// cut leading '/'
	}
	else if( strcasecmp(url.protocol(), "widget") == 0 )
	{
		widget( url.path() + 1 );	// cut leading '/'
	}
}


void KDocumentServer::file( KHTMLView *_view, const char *_url )
{
	KURL url( _url );
	QFile file( url.path() );

	if ( !file.open(IO_ReadOnly) )
	{
		KMessageBox::sorry( _view, i18n("unable to find URL\n") + _url);
		return;
	}

	char buffer[1024];	
	_view->begin(url.directory()); // changed this from directoryURL() to
	// directory(), seems like this has been changed in KURL2

	// is there an attachement?
	// my luck that KHWMLWidget does not care about <HTML></HTML> tags...
	
//	_view->write( "<A HREF=\"file:/install/kdenonbeta2/khelpcenter/HTML/terms/root window\" TARGET=\"_popup\"><IMG SRC=\"file:/install/kdenonbeta2/khelpcenter/HTML/attach.gif\" BORDER=0 WIDTH=22 HEIGHT=22></A>" );	

	do
	{
		buffer[0] = '\0';
		file.readLine( buffer, 1024 );
		_view->write(buffer);
	}
	while ( !file.atEnd() );

	_view->end();
	_view->parse();	
	
	emit requestDone( FILE );

	return;
}

void KDocumentServer::command( const char *cmd )
{
	KProcess proc;
	proc << cmd;
	bool success = proc.start( KProcess::DontCare );	

	emit requestDone( (success == TRUE) ? COMMAND : ERROR );
}

void KDocumentServer::widget( const char *identifier )
{
	KProcess proc;
	proc << "highlightWidget";
	proc << identifier;
	bool success = proc.start( KProcess::DontCare );	

	emit requestDone( (success == TRUE) ? WIDGET : ERROR );

	return;
}

void KDocumentServer::CGI( KHTMLView *_view, const char *_url )
{
#ifdef DEBUG
	printf("KDocumentServer::CGI(%s)\n", _url); fflush(stdout);
#endif

	if( CGIServer != 0L )
	{
		delete( CGIServer );
		CGIServer = 0L;
	}

	CGIServer = new KCGI;

	sprintf( filename, "%s/kassistant_XXXXXX", _PATH_TMP );
	mktemp( filename );
	localFile.sprintf("file:%s" , filename);
	view = _view;

	connect( CGIServer, SIGNAL( finished() ), this, SLOT( slotCGIDone() ) );
	CGIServer->get( _url, localFile, "Get" );
}

void KDocumentServer::slotCGIDone()
{
#ifdef DEBUG
	printf("KDocumentServer::slotCGIDone()\n"); fflush(stdout);
#endif

	if( CGIServer != 0L )
	{
		delete( CGIServer );
		CGIServer = 0L;
	}

	file( view, localFile );
	emit requestDone( CGIBIN );
	
	unlink( filename );
}

#include "docserver.moc"
