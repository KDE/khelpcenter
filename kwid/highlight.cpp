 /*
  *  This file is part of the KDE Help Center
  * 
  * Copyright (c) 1998 René Beutler (rbeutler@g26.ethz.ch)
  *
  */

#include <stdio.h>
#include <kapp.h>
#include "kwid.h"


int main( int argc, char **argv )
{
	KApplication app( argc, argv, "highlightWidget" );
	KWidgetIdentification *id = new KWidgetIdentification();
	id->showWidget( argv[1], TRUE );
	app.exec();
}
