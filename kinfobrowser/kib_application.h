/*
 *  kib_application.h - part of KInfoBrowser
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

#include "kinfobrowser.h"

class kibMainWindow;
class kibView;

/**
 * Provide factories for MainWindow and View.
 */
class kibApplicationIf : virtual public OPApplicationIf,
						 virtual public KInfoBrowser::Application_skel
{
public:
  kibApplicationIf();
  kibApplicationIf(const CORBA::BOA::ReferenceData &refdata);
  kibApplicationIf(CORBA::Object_ptr _obj);
  
 
  /**
   * Defined in openparts.idl.
   */
  //virtual OpenParts::Part_ptr createPart();
  virtual OpenParts::MainWindow_ptr createWindow();

  KInfoBrowser::View_ptr createView();
};

#endif
