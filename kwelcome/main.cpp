/*
 *  main.cpp - part of the KDE Help Center
 *
 *  Copyright (C) 1999 Matthias Elter (me@kde.org)
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

#include <kwelcome.h>
#include <kapp.h>
#include <kconfig.h>

int main(int argc, char *argv[])
{
  KApplication app(argc, argv, "kwelcome");
  
  // check for -kdestartup
  KConfig *conf = kapp->getConfig();
  conf->setGroup("General Settings");
  QString tmp = conf->readEntry("AutostartOnKDEStartup", "true");

  for (int i = 1; i < argc; i++)
	{
	  QString arg = argv[i];

	  if ((arg == "-kdestartup") && (tmp != "true"))
		return 0;
	}
  
  KWelcome *toplevel = new KWelcome();
  app.setMainWidget(toplevel);
  app.setTopWidget(toplevel);
  toplevel->show();
  
  int rv = app.exec();

  if (toplevel)
    delete toplevel;
  return rv;
}
