 /*
 *  This file is part of the KDE Help Center
 *
 *  Copyright (c) 2002 Frerich Raabe <raabe@kde.org>
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
#include "application.h"
#include "mainwindow.h"

#include <kcmdlineargs.h>
#include <kuniqueapplication.h>

using namespace KHC;

Application::Application() : KUniqueApplication()
{
}

int Application::newInstance()
{
	if ( isRestored() ) {
		RESTORE( MainWindow )
		return 0;
	}

	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

	KURL url;
	if ( args->count() )
		url = args->url( 0 );

	MainWindow *mainWindow = new MainWindow( url );
	setMainWidget( mainWindow );
	mainWindow->show();

	return 0;
}

// vim:ts=4:sw=4:noet
