/*
    This file is part of the KDE Help Center

    Copyright (C) 1998 René Beutler (rbeutler@g26.ethz.ch)
*/


#include <stdio.h>
#include "help.moc"

onItemHelp::onItemHelp()
{	
	int pos;
	KProcess proc;
	proc << "identifyWidget";
	QApplication::connect(&proc, SIGNAL(receivedStdout(KProcess *, char *, int)), this, SLOT(receivedStdout(KProcess *, char *, int)) );
	proc.start( KProcess::Block, KProcess::Stdout );
	QString path = KApplication::kde_htmldir();

	if( proc.normalExit() == TRUE )
	{
		if( proc.exitStatus() == 0 )
		{
//			highlight( *result );

			// get rid of everything behind an optional '#'			
			pos = result->find( '#' );
			if( pos != -1 )
				result->truncate( pos );
			// compose path to help file
			pos = result->find( '/' );
			if( pos < 0 )
				exit(1);
			result->insert( pos, "/widgets" );
			// ---> should use klocale->language() here
			// ---> or better a invokeOnItemHelp to KApplication
			path += "/default/" + *result + ".html";
			// run help
			openHelp( path );
		}
	}
}

void onItemHelp::highlight( QString widgetIdentifier )
{
	KProcess proc;
	proc << "highlightWidget";
	proc << widgetIdentifier;
	proc.start( KProcess::DontCare );
}

void onItemHelp::openHelp( QString path )
{
	KProcess proc;
	proc << "kdehelp";
	proc << path;
	proc.start( KProcess::DontCare );
}

void onItemHelp::receivedStdout(KProcess *proc, char *buffer, int buflen)
{
	result = new QString( (const char *)buffer );
}

int main()
{
	new onItemHelp;
	return 0;
}
