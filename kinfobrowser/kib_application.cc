/*
 *  kib_application.cc - part of KInfoBrowser
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

#include "kib_application.h"
#include "kib_mainwindow.h"
#include "kib_view.h"

#include <opApplication.h>
#include <komShutdownManager.h>

kibApplicationIf::kibApplicationIf(){}
kibApplicationIf::kibApplicationIf(const CORBA::BOA::ReferenceData &refdata) : KInfoBrowser::Application_skel(refdata){}
kibApplicationIf::kibApplicationIf(CORBA::Object_ptr _obj) : KInfoBrowser::Application_skel(_obj){}

OpenParts::MainWindow_ptr kibApplicationIf::createWindow()
{
  kibMainWindow * mw = new kibMainWindow;
  mw->show();
  KOMShutdownManager::self()->watchObject(mw->interface());
  return OpenParts::MainWindow::_duplicate(mw->interface());
}

KInfoBrowser::View_ptr kibApplicationIf::createView()
{
  kibView *view = new kibView;
  KOMShutdownManager::self()->watchObject(view);
  return KInfoBrowser::View::_duplicate(view);
}
