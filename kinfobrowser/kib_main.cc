/*
 *  kib_main.cc - part of KInfoBrowser
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
#include "kib_application.h"
#include "kib_mainwindow.h"

#include <opApplication.h>
#include <komBase.h>

#include <kglobal.h>
#include <klocale.h>

int main(int argc, char *argv[])
{
  OPApplication app(argc, argv, "kinfobrowser");

  bool server = false;
  
  for (int i=0; i< argc; i++)
    {
      if (strcmp(argv[i], "--server") == 0 || strcmp(argv[i], "-s") == 0)
		server = true;
    }

  KOMBoot<kibApplicationIf> appLoader("IDL:KInfoBrowser/Application:1.0", "App");
  KGlobal::locale()->insertCatalogue("libkhc");

  if (server)
	{
	  app.exec();
	  return 0;
    }
  else
    {
	  kibMainWindow *win = new kibMainWindow;
	  win->show();
	  
	  app.exec();
	  if (win)
	    delete win;
	  return 0;
    }
}
