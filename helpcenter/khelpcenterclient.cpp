/*
 *  khelpcenterclient.cpp - part of the KDE Help Center
 *
 *  Copyright (c) 1999 Matthias Elter (me@kde.org)
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

#include <komApplication.h>
#include <kprocess.h>

#include "khelpcenter.h"
#include <kdebug.h>

void usage()
{
    printf(""\
	   "\nkhelpcenterclient is part of the KDE Help Center\n"\
	   "(c) 1999, Matthias Elter\n\n"\
	   "purpose: remote control khelpcenter\n\n"\
	   "syntax:\n"\
	   "   khelpcenterclient open url\n"\
	   "                 # open a new browser window and load url 'url'\n"\
	   "                 # load introduction url if no url parameter spezified\n"\
	   "                 # example url: file:/home/dummy/index.html\n\n"\
	   "   khelpcenterclient configure\n"\
	   "                 # re-read khelpcenter's configuration\n"\
	   "options:\n"\
	   "   -enable_tree\n"\
	   "                 # open a new browser window with enabled treeview\n");
}

int main(int argc, char *argv[])
{
    bool enableTree = false;
    int command = 0;
    QString url;
  
    // no parameters -> print usage
    if(argc == 1) 
    {
	usage();
	return 0;
    }

    // parse command line parameters
    for (int i = 1; i < argc; i++)
    {
	if (strcasecmp(argv[i], "configure") == 0)
	{
	    command = 1;
	    break;
	}
	else if (strcasecmp(argv[i], "-enable_tree") == 0)
	{
	    enableTree = true;
	}
	else if (command == 2) // already found 'open'...this must be a url
	{
	    url = argv[i];
	}
	else if (strcasecmp( argv[i], "open" ) == 0)
	{
	    command = 2;
	}
    }

    if (command == 0)
    {
	usage();
	return 0;
    }
  
    KOMApplication app(argc, argv, "khelpcenterclient");

    // init CORBA stuff
    CORBA::Object_var obj = app.orb()->bind("IDL:KHelpCenter/HelpCenterCom:1.0", "inet:localhost:8887");
    if(CORBA::is_nil(obj))
    {
	kdebug(KDEBUG_FATAL,1401,"It seems that the khelpcenter server isn't running!");
	return 0;
    }	
    KHelpCenter::HelpCenterCom_var server = KHelpCenter::HelpCenterCom::_narrow(obj);
    kdebug(KDEBUG_INFO,1401,"binding to KOM interface: IDL:KHelpCenter/HelpCenterCom:1.0");
  
    if (command == 1)
    {
	kdebug(KDEBUG_INFO,1401,"khelpcenterclient -configure");
	server->configure();
    }
    else if (command == 2)
    {
	if (url.isEmpty())
	    url = "file:" + locate("html", "default/khelpcenter/main.html");
	kdebug(KDEBUG_INFO,1401,"khelpcenterclient open %s", url.ascii());
	server->openHelpView (url, enableTree);
    }
  
    return 0;
}
