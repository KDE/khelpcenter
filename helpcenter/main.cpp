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
#include "khelpcenter.h"
#include "helpcentercom_impl.h"
#include "error.h"
#include <komApplication.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qmessagebox.h>
#include <klocale.h>

void errorHandler( int type, char *msg );

int main(int argc, char *argv[])
{
  // error handler for man stuff
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
  KOMApplication app(argc, argv, "khelpcenter.bin");
  
  HelpCenterCom *helpcentercom = new HelpCenterCom;
  app.boa()->impl_is_ready(CORBA::ImplementationDef::_nil());
  kdebug(KDEBUG_INFO,1401,"KOM interface ready: IDL:KHelpCenter/HelpCenterCom:1.0");

  app.exec();
  return 0;
}

void errorHandler( int type, char *msg )
{
  QApplication::setOverrideCursor(Qt::arrowCursor);
  QMessageBox::message(i18n("Error"), msg, i18n("OK"));
  
  QApplication::restoreOverrideCursor();

  if (type == ERR_FATAL)
	  exit(1);
}
