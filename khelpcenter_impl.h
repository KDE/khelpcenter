/*
 *  khelpcenter_impl.h - part of the KDE Help Center
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

#ifndef _HelpWindow_h_
#define _HelpWindow_h_

#include "khelpcenter.h"

class HelpWindow_Impl : virtual public KHelpCenter::HelpWindow_skel
{

 public:
    HelpWindow_Impl();
    virtual ~HelpWindow_Impl();

    void open(const char *url);

 private:
    HelpCenter *hc;
};

class HelpWindowFactory_Impl : virtual public KHelpCenter::HelpWindowFactory_skel
{
public:
  HelpWindowFactory_Impl() {}

  HelpWindowFactory_Impl(const CORBA::ORB::ObjectTag &tag)
   : KHelpCenter::HelpWindowFactory_skel(tag) {}
  
  HelpWindowFactory_Impl(CORBA::Object_ptr obj)
   : KHelpCenter::HelpWindowFactory_skel(obj) {}

  virtual KHelpCenter::HelpWindow_ptr create();  
};

#endif

