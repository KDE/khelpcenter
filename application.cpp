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
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include "application.h"
#include "mainwindow.h"

#include <kcmdlineargs.h>

using namespace KHC;

Application::Application() : KUniqueApplication()
{
}

int Application::newInstance()
{
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

	KURL url;
	if ( args->count() )
		url = args->url( 0 );

	if ( isRestored() ) {
		RESTORE( MainWindow );
		return 0;
	}
        else
        {
            if( ! mainWidget() )
            {
                MainWindow *mainWindow = new MainWindow;
                setMainWidget( mainWindow );
                mainWindow->show();
            }
            static_cast<MainWindow *>( mainWidget() )->slotOpenURL( url.url() );
        }

  return KUniqueApplication::newInstance();
}

#include "version.h"

#include <kaboutdata.h>

static KCmdLineOptions options[] =
{
	{ "+[url]", I18N_NOOP("A URL to display"), "" },
        KCmdLineLastOption
};

int kdemain( int argc, char **argv )
{
	KAboutData aboutData( "khelpcenter", I18N_NOOP("KDE Help Center"),
	                      HELPCENTER_VERSION,
	                      I18N_NOOP("The KDE Help Center"),
	                      KAboutData::License_GPL,
	                      I18N_NOOP("(c) 1999-2002, The KHelpCenter developers") );

	aboutData.addAuthor( "Cornelius Schumacher", 0, "schumacher@kde.org" );
	aboutData.addAuthor( "Frerich Raabe", 0, "raabe@kde.org" );
	aboutData.addAuthor( "Matthias Elter", I18N_NOOP("Original Author"),
	                     "me@kde.org" );
	aboutData.addAuthor( "Wojciech Smigaj", I18N_NOOP("Info page support"),
	                     "achu@klub.chip.pl" );

	KCmdLineArgs::init( argc, argv, &aboutData );
	KCmdLineArgs::addCmdLineOptions( options );
	KApplication::addCmdLineOptions();

	KHC::Application app;

	return app.exec();
}

// vim:ts=2:sw=2:et
