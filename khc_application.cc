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

#include "khc_application.h"
#include "khc_mainwindow.h"
#include "khc_mainview.h"
#include "khc_htmlview.h"

#include <opApplication.h>
#include <komShutdownManager.h>

khcApplicationIf::khcApplicationIf(){}
khcApplicationIf::khcApplicationIf(const CORBA::BOA::ReferenceData &refdata) : KHelpCenter::Application_skel(refdata){}
khcApplicationIf::khcApplicationIf(CORBA::Object_ptr _obj) : KHelpCenter::Application_skel(_obj){}

khcMainView *khcApplicationIf::allocMainView(const char *url)
{
  khcMainView *mainView = new khcMainView(url);
  //KOMShutdownManager::self()->watchObject(mainView);
  return mainView;
}

khcMainWindow *khcApplicationIf::allocMainWindow(const char *url)
{
  khcMainWindow * mw = new khcMainWindow(url);
  mw->show();
  //KOMShutdownManager::self()->watchObject(mw->interface());
  return mw;
}

OpenParts::Part_ptr khcApplicationIf::createPart()
{
  return OpenParts::Part::_duplicate(allocMainView());
}

OpenParts::MainWindow_ptr khcApplicationIf::createWindow()
{
  return OpenParts::MainWindow::_duplicate(allocMainWindow()->interface());
}

KHelpCenter::HTMLView_ptr khcApplicationIf::createHTMLView()
{
  khcHTMLView *htmlView = new khcHTMLView;
  // KOMShutdownManager::self()->watchObject(htmlView);
  return KHelpCenter::HTMLView::_duplicate(htmlView);
}


khcBrowserFactory::khcBrowserFactory(const CORBA::BOA::ReferenceData &refData)
: Browser::BrowserFactory_skel(refData){}

khcBrowserFactory::khcBrowserFactory(CORBA::Object_ptr obj)
: Browser::BrowserFactory_skel(obj){}

OpenParts::MainWindow_ptr khcBrowserFactory::createBrowserWindow(const QCString &url)
{
  return OpenParts::MainWindow::_duplicate(khcApplicationIf::allocMainWindow(url)->interface());
}

OpenParts::Part_ptr khcBrowserFactory::createBrowserPart(const QCString &url)
{
  return OpenParts::Part::_duplicate(khcApplicationIf::allocMainView(url));
}
