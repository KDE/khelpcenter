/*
 *  khc_application.cc - part of the KDE Helpcenter
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

#ifndef __khc_application_h__
#define __khc_application_h__

#include <opApplication.h>

#include "khelpcenter.h"

class khcMainWindow;
class khcMainView;

/**
 * Provide factories for MainWindow, MainView and HTMLView.
 */
class khcApplicationIf : virtual public OPApplicationIf,
			 virtual public KHelpCenter::Application_skel
{
public:
  khcApplicationIf();
  khcApplicationIf(const CORBA::BOA::ReferenceData &refdata);
  khcApplicationIf(CORBA::Object_ptr _obj);
  
  /**
   * Share allocation code between khcApplicationIf and khcBrowserFactory.
   */
  static khcMainView   *allocMainView(const char *url = 0L);
  static khcMainWindow *allocMainWindow(const char *url = 0L);

  /**
   * Defined in openparts.idl.
   */
  virtual OpenParts::Part_ptr createPart();
  virtual OpenParts::MainWindow_ptr createWindow();

  KHelpCenter::HTMLView_ptr createHTMLView();
};

/**
 * Implementation of Browser::BrowserFactory. See kdelibs/corba/idl/browser.idl!
 */
class khcBrowserFactory : public Browser::BrowserFactory_skel
{
 public:
  khcBrowserFactory(const CORBA::BOA::ReferenceData &refData);
  khcBrowserFactory(CORBA::Object_ptr obj);
  
  OpenParts::MainWindow_ptr createBrowserWindow(const QCString &url);
  OpenParts::Part_ptr createBrowserPart(const QCString &url);
};

#endif




