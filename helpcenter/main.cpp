/*
 *  main.cpp - part of the KDE Help Center
 *
 *  Copyright (C) 199 Matthias Elter (me@main-echo.net)
 *  based on code from kdehelp (C) 1996 Martin R. Jones
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

#include "toplevel.h"
#include "error.h"
#include <kapp.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qmessagebox.h>

void errorHandler( int type, char *msg );

int main(int argc, char *argv[])
{
  int i;
  QString url, initDoc;
  
  // pase command line parameters
  for (i = 1; i < argc; i++)
	{
	  if ( argv[i][0] == '-' )
		{
		  // skip -caption, -icon, -miniicon
		  if ( strcasecmp( argv[i], "-caption" ) == 0 )
			i++;
		  if ( strcasecmp( argv[i], "-icon" ) == 0 )
			i++;
		  if ( strcasecmp( argv[i], "-miniicon" ) == 0 )
			i++;
		  continue;
		}
	  initDoc = argv[i];
	}
  
  if ( initDoc.isEmpty() ) // no url parameter...use main.html
	{
	  initDoc = "file:";
	  initDoc += kapp->kde_htmldir().copy();
	  initDoc += "/default/khelpcenter/main.html";
	}
	
  url = initDoc;
	
  if ( !strchr( url, ':' ) )
	{
	  if ( initDoc[0] == '.' || initDoc[0] != '/' )
		{
		  QFileInfo fi( initDoc );
		  initDoc = fi.absFilePath();
		}

	  url = "file:";
	  url += initDoc;
	}

  // error handler for info and man stuff
  Error.SetHandler( (void (*)(int, const char *))errorHandler );

  // create local data directory if necessary
  QDir dir;
  dir.setPath(KApplication::localkdedir() + "/share/apps");

  if (!dir.exists())
	dir.mkdir(KApplication::localkdedir() + "/share/apps");
  
   dir.setPath(KApplication::localkdedir() + "/share/apps/khelpcenter");
  if (!dir.exists())
	dir.mkdir(KApplication::localkdedir() + "/share/apps/khelpcenter");

  // init app
  KApplication app(argc, argv, "khelpcenter");
  
  HelpCenter *toplevel;

  if (app.isRestored())
	{
	  int n = 1;
	  while ( KTMainWindow::canBeRestored( n ) )
	    {
		  toplevel = new HelpCenter;
		  toplevel->restore( n );
		  n++;
	    }
	  
	  return app.exec();
	}
  else
	{
	  toplevel = new HelpCenter;

	  app.setMainWidget(toplevel);
	  app.setTopWidget(toplevel);

	  toplevel->openURL(url);
	  toplevel->show();  
	  return app.exec();
	}
}

void errorHandler( int type, char *msg )
{
  QApplication::setOverrideCursor(Qt::arrowCursor);
  
  QMessageBox::message( i18n("Error"), 
						msg, 
						i18n("OK") );
  
  QApplication::restoreOverrideCursor();

  if ( type == ERR_FATAL )
	{
	  exit(1);
	}
}
