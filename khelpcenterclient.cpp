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
#include <kded_utils.h>
#include <kded_instance.h> 
#include <kactivator.h>
#include <ktrader.h>

#include <iostream.h>
#include <qstringlist.h>

#include "khelpcenter.h"
#include <kdebug.h>

int main(int argc, char *argv[])
{
  KOMApplication app( argc, argv );

  // create a KdedInstance object, which takes care about connecting to kded
  // and providing a trader and an activator
  KdedInstance kded(argc, argv, komapp_orb);

  // get a pointer to the activator
  KActivator *activator = kded.kactivator();
  
  // now we register our service
  // usually we query the trader for a certain service and use the data
  // of the kservice offer from the trader to register the service, but since 
  // our service is not registered with the trader we do the registration at
  // the activator "manually"
  QStringList repoIds;
  repoIds.append( "IDL:KHelpCenter/HelpWindowFactory:1.0#KHelpCenter" );

  // now let's really register the service
  // this will make the activator register the service at the IMR
  activator->registerService( "KHelpCenter", "shared", repoIds, "khelpcenterd" );

  // let's activate a service in the server
  CORBA::Object_var obj = activator->activateService( "KHelpCenter", "IDL:KHelpCenter/HelpWindowFactory:1.0", "KHelpCenter" );
  assert( !CORBA::is_nil( obj ) );

  KHelpCenter::HelpWindowFactory_var factory = KHelpCenter::HelpWindowFactory::_narrow( obj );
  assert( !CORBA::is_nil( factory ) );
  
  KHelpCenter::HelpWindow_var view = factory->create(); 
  assert(!CORBA::is_nil(view));
  
  if(argc == 2) 
    {
      QString url = argv[1];
      view->open(url);
    }
  
  return 0;
}
