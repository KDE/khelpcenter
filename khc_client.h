/*
 *  khc_client.h - part of the KDE Help Center
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

#ifndef __khc_client_h__
#define __khc_client_h__

#include <komApplication.h>
#include <kded_instance.h>
#include "khelpcenter.h"

class khcClientApp : public KOMApplication
{
public:

  khcClientApp(int &argc, char **argv, const QString& rAppName = QString::null)
    : KOMApplication (argc, argv, rAppName)
    {
      kded = new KdedInstance(argc, argv, komapp_orb);
      trader = kded->ktrader();
      activator = kded->kactivator();
    };

  ~khcClientApp() { /* delete kded */ } ;
  
  openURL(const char* _url);

protected:
  
  KdedInstance *kded;
  KTrader *trader;
  KActivator *activator;

  Browser::BrowserFactory_var m_vkhc;
};

#endif
