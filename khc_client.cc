/*
 *  khc_client.cc - part of the KDE Help Center
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

#include "khc_client.h"

#include <kactivator.h>
#include <ktrader.h>
#include <kdebug.h>

#include <qstring.h>

int main(int argc, char *argv[])
{
  khcClientApp app(argc, argv);

  // Do we have a url argument?
  QString _url(QString::null);

  if(argc == 2) 
    {
      _url = argv[1];
    }

  return app.openURL(_url);
}

khcClientApp::openURL(const char* _url)
{
  // Query the trader for the KHelpcenter service
  KTrader::OfferList offers = trader->query("Helpbrowser", "Name == 'KHelpcenter'");

  if (offers.count() != 1)
    {
      kdebug(KDEBUG_INFO,1401,"Found %i offers for Helpbrowser service called KHelpcenter.\n", offers.count());
      kdebug(KDEBUG_INFO,1401,"Error: Can't find KHelpcenter service.\n");
      return false;
    }
  
  // Activate the BrowserFactory.
  CORBA::Object_var obj = activator->activateService("KHelpcenter", "IDL:Browser/BrowserFactory:1.0", "KHelpcenter");

  if (CORBA::is_nil(obj))
    {
      kdebug(KDEBUG_INFO,1401,"Error: Can't connect to KHelpcenter.\n");
      return false;
    }
  
  m_vkhc = Browser::BrowserFactory::_narrow(obj);
  
  if (CORBA::is_nil(m_vkhc))
    {
      kdebug(KDEBUG_INFO,1401,"Error: Can't connect to KHelpcenter.\n");
      return false;
    }
   
  // Create a khcMainWindow.
  kdebug(KDEBUG_INFO,1401,"Incarnating khcMainWindow!\n");
  OpenParts::MainWindow_var m_vMainWindow = m_vkhc->createBrowserWindow(_url);
  
  return 0; 
}
