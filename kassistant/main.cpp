/*
    This file is part of the KDE Help Center

    Copyright (C) 1998 René Beutler (rbeutler@g26.ethz.ch)
*/

#include <komApplication.h>
#include <ktopwidget.h>
#include <kconfig.h>

#include "helpcom.h"
#include "helpcom_impl.h"
#include "assistant.h"
#include "browser.h"

#include <X11/Xlib.h>


KConfig *pConfig = 0L;
Browser *browser = 0L;
Assistant *assistant = 0L;

int main(int argc, char *argv[])
{
	pConfig = new KConfig( KApplication::kde_configdir() +  "/khelpcenterrc",
	                       KApplication::localconfigdir() + "/khelpcenterrc" );

 	// create data directory if necessary
	QString p = KApplication::localkdedir();
	QString dataDir = p + "/share/apps";
	if ( access( dataDir, F_OK ) )
		mkdir( dataDir.data(), 0740 );
	dataDir += "/kassistant";
	if ( access( dataDir, F_OK ) )
		mkdir( dataDir.data(), 0740 );

	KOMApplication app(argc, argv, "kassistant" );

	if ( app.isRestored() )
	{
		printf("app is restored\n"); fflush(stdout);
//		RESTORE(Assistant);
	}
	else 
	{
		// no session management: just create one window
		printf("create window\n"); fflush(stdout);
		assistant = new Assistant();
	}

	XSetTransientForHint( qt_xdisplay(), assistant->winId(), qt_xrootwin() );

	HelpCom *helpcom;
	helpcom = new HelpCom();
	app.boa()->impl_is_ready( CORBA::ImplementationDef::_nil() );

	app.exec();

	delete( pConfig );
	return 0;
}
