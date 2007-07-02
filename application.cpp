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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "application.h"
#include "mainwindow.h"
#include "version.h"

#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>

using namespace KHC;

Application::Application() : KUniqueApplication(), mMainWindow( 0 )
{
}

int Application::newInstance()
{
  if (restoringSession()) return 0;
  
  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

  KUrl url;
  if ( args->count() )
    url = args->url( 0 );

  if( !mMainWindow ) {
    mMainWindow = new MainWindow;
    mMainWindow->show();
  }

  mMainWindow->openUrl( url );

  return KUniqueApplication::newInstance();
}

extern "C" int KDE_EXPORT kdemain( int argc, char **argv )
{
  KAboutData aboutData( "khelpcenter", 0, ki18n("KDE Help Center"),
                        HELPCENTER_VERSION,
                        ki18n("The KDE Help Center"),
                        KAboutData::License_GPL,
                        ki18n("(c) 1999-2003, The KHelpCenter developers") );

  aboutData.addAuthor( ki18n("Cornelius Schumacher"), KLocalizedString(), "schumacher@kde.org" );
  aboutData.addAuthor( ki18n("Frerich Raabe"), KLocalizedString(), "raabe@kde.org" );
  aboutData.addAuthor( ki18n("Matthias Elter"), ki18n("Original Author"),
                       "me@kde.org" );
  aboutData.addAuthor( ki18n("Wojciech Smigaj"), ki18n("Info page support"),
                       "achu@klub.chip.pl" );

  KCmdLineArgs::init( argc, argv, &aboutData );

  KCmdLineOptions options;
  options.add("+[url]", ki18n("URL to display"));
  KCmdLineArgs::addCmdLineOptions( options );
  KCmdLineArgs::addStdCmdLineOptions();

  KHC::Application app;

  if ( app.isSessionRestored() ) {
     RESTORE( MainWindow );
  }

  return app.exec();
}

// vim:ts=2:sw=2:et
