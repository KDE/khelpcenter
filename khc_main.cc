/*
 *  khc_main.cpp - part of the KDE Help Center
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

#include "khc_application.h"
#include "khc_mainwindow.h"

#include <opApplication.h>
#include <komBase.h>

#include <kglobal.h>
#include <klocale.h>

#include <qdir.h>


int main(int argc, char *argv[])
{
  OPApplication app(argc, argv, "khelpcenter");
  bool server = false;
  
  for (int i=0; i< argc; i++)
    {
      if (strcmp(argv[i], "--server") == 0 || strcmp(argv[i], "-s") == 0)
	server = true;
    }

  KOMBoot<khcApplicationIf> appLoader("IDL:KHelpcenter/Application:1.0", "App");
  KOMBoot<khcBrowserFactory> browserFactoryLoader("IDL:Browser/BrowserFactory:1.0", "KHelpcenter");
  
  KGlobal::locale()->insertCatalogue("libkhc");

  if (server)
   {
     app.exec();
     return 0;
    }
  else
    {
      khcMainWindow *win = new khcMainWindow;
      win->show();
      
      app.exec();
      if (win)
	delete win;
      return 0;
    }
}
