/*
 *  main.cc - part of the KDE Help Center
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

#include <kglobal.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <kconfig.h>
#include <klocale.h>
#include <kuniqueapp.h>

static const char *description = I18N_NOOP("KDE welcome dialog.");
static const char *version     = "v1.0alpha";

static KCmdLineOptions option[] =
{
   { "kdestartup", I18N_NOOP("Show this dialog on KDE startup."), 0 },
   { 0, 0, 0 }
};

int main(int argc, char *argv[])
{
  KAboutData aboutData( "kwelcome", I18N_NOOP("KDE Welcome Dialog"),
                        version, description, KAboutData::License_GPL,
						"(c) 1999-2000, Matthias Elter" );
  aboutData.addAuthor( "Matthias Elter", 0, "me@kde.org" );
  KCmdLineArgs::init( argc, argv, &aboutData );
  KCmdLineArgs::addCmdLineOptions(option);

  if ( KUniqueApplication::start() == false )
    return 0;

  KUniqueApplication app;
  
  // check for -kdestartup
  KConfig *conf = KGlobal::config();
  conf->setGroup("General Settings");
  bool autostart = conf->readBoolEntry( "AutostartOnKDEStartup", true);
  
  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
  if ( args->isSet( "kdestartup" ) && !autostart )
    return 0;
  
  KWelcome *toplevel = new KWelcome();
  app.setMainWidget(toplevel);
  app.setTopWidget(toplevel);
  toplevel->show();
  
  int rv = app.exec();
  
  if (toplevel)
    delete toplevel;
  return rv;
}
