 /*
  *  This file is part of the KDE Help Center
  * 
  * Copyright (c) 1998 René Beutler (rbeutler@g26.ethz.ch)
  *
  */

#include <stdio.h>
#include "kwid.h"


int main( int argc, char **argv )
{
	char caption[256];

	KWidgetIdentification *id = new KWidgetIdentification();
	id->getIdentifierByUser( caption );
	printf("%s\n", caption);
}
