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
#include "khelpcenter_impl.h"

#include <klocale.h>

#include <komApplication.h>
#include <komAutoLoader.h>

#include <qdir.h>
#include <qfileinfo.h>

int main(int argc, char *argv[])
{
    // create local data directory if necessary
    QDir dir;
    dir.setPath(KApplication::localkdedir() + "/share/apps");

    if (!dir.exists())
	dir.mkdir(KApplication::localkdedir() + "/share/apps");
  
    dir.setPath(KApplication::localkdedir() + "/share/apps/khelpcenter");
    if (!dir.exists())
	dir.mkdir(KApplication::localkdedir() + "/share/apps/khelpcenter");

    // init app
    KOMApplication app(argc, argv, "KDE HelpCenter");

    KOMAutoLoader<HelpWindowFactory_Impl> HelpWindowFactoryLoader("IDL:KHelpCenter/HelpWindowFactory:1.0" , "KHelpCenter");
 
    app.exec();
    return 0;
}
