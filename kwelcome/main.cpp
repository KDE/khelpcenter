/*
 *  main.cpp - part of KWelcome
 *
 *  Copyright (C) 1998,99 Matthias Elter (me@kde.org)
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

#include "kwelcome.h"
#include <kapp.h>

int main(int argc, char *argv[])
{
  KApplication app(argc, argv, "kwelcome");
  
  // check for --kdestartup
  KConfig *conf = kapp->getConfig();
  conf->setGroup("General Settings");
  QString tmp = conf->readEntry("AutostartOnKDEStartup", "0");
  if (tmp == "0" && (strcmp(argv[1], "--kdestartup") == 0))
	return 0;
  
  KWelcome *widget = new KWelcome();
  app.setMainWidget(widget);
  app.setTopWidget(widget);
  widget->show();
  
  return app.exec();
}
