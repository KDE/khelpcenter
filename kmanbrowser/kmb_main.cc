/*
 *  kmb_main.cpp - part of the KDE Help Center
 *
 *  Copyright (C) 199 Matthias Elter (me@kde.org)
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

#include "kmb_mainwindow.h"

#include <komAutoLoader.h>
#include <opApplication.h>

int main(int argc, char *argv[])
{
    bool server = false;

    for (int i=0; i< argc; i++)
    {
	if (strcmp(argv[i], "--server") == 0 || strcmp(argv[i], "-s") == 0)
	    server = true;
    }

    if (server)
    {
	// activate autoloader for interface "ViewFactory"
	OPApplication app(argc, argv, "kmanbrowser");
	//KOMAutoLoader<ViewFactory_Impl> HelpBrowserFactoryLoader("IDL:Browser/ViewFactory:1.0" , "ManBrowser");
	
	app.exec();
	return 0;
    }
    else
    {
	// standalone app
	OPApplication app(argc, argv, "kmanbrowser");
	kmbMainWindow *win = new kmbMainWindow;
	win->show();
	
	app.exec();
	if (win)
	    delete win;
	return 0;
    }
}
