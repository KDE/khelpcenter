/*
    This file is part of the KDE Help Center

    Copyright (C) 1998 René Beutler (rbeutler@g26.ethz.ch)
*/


#include <komApplication.h>
#include "helpcom.h"


void usage()
{
	printf(""\
	"kassistantcom is part of the KDE Help Center\n"\
	"(c) 1999, René Beutler\n\n"\
	"purpose: remote control kassistant\n\n"\
	"syntax:\n"\
	"   kassistantcom browse 'filename'\n"\
	"                 # open the file in the browser, where filename is\n"\
	"                 # file:/path/name.html\n\n"\
	"   kassistantcom configure\n"\
	"                 # re-read kassistant's configuration\n");
}


int main(int argc, char *argv[])
{
	KOMApplication app(argc, argv, "kassistantcom" );
	int command = 0;

	if( argc == 1 )
	{
		usage();
		return 0;
	}

	if( (argc == 2) && (strcasecmp( argv[1], "configure" ) == 0) )
		command = 1;
	else if( (argc == 3) && (strcasecmp( argv[1], "browse" ) == 0) )
		command = 2;	

	if( command == 0 ) {
		usage();
		return 0;
	}

	// init CORBA stuff
	CORBA::Object_var obj = app.orb()->bind( "IDL:helpcom:1.0", "inet:localhost:8888" );
	if( CORBA::is_nil( obj ) )
	{
		printf("it seems that kassistant isn't running!\n");
		return 0;
	}	
	helpcom_var client = helpcom::_narrow( obj );


	switch( command )
	{
		case 1:	client->configure(); break;
		case 2: client->showDocument( 0, argv[2] );
	}

	return 0;
}
