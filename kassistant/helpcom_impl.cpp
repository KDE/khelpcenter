/*
    This file is part of the KDE Help Center

    Copyright (C) 1998 René Beutler (rbeutler@g26.ethz.ch)
*/


#include "assistant.h"
#include "browser.h"
#include "helpcom_impl.h"


extern Browser *browser;
extern Assistant *assistant;


HelpCom::HelpCom()
{
}

CORBA::ULong HelpCom::registerApp( const char *name )
{
	printf("Application %s registered.\n", name); fflush(stdout);
	return 1;
}

void HelpCom::removeApp( CORBA::ULong id )
{
	// if the last app is removed, and the help assistant
	// was invoked by an application -> exit
	printf("Application %ld removed.\n", id); fflush(stdout);
}

void HelpCom::showDocument( CORBA::ULong id, const char *name )
{
	if( !browser )
		browser = new Browser();


	browser->setURL( name, TRUE );
}

void HelpCom::documentHint( CORBA::ULong id, const char *name )
{
}

void HelpCom::activate( CORBA::ULong id )
{
}


void HelpCom::configure()
{
	if( browser )
		browser->slotConfigure();
	if( assistant )
		assistant->slotConfigure();
}
