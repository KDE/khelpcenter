/*
 *  khelpcenter_impl.cpp - part of the KDE Help Center
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

#include "toplevel.h"
#include "khelpcenter_impl.h"

#include <iostream.h>


KHelpCenter::HelpWindow_ptr HelpWindowFactory_Impl::create()
{
  return KHelpCenter::HelpWindow::_duplicate(new HelpWindow_Impl);
}


HelpWindow_Impl::HelpWindow_Impl()
{
  hc = new HelpCenter;

  QString _url = "file:" + locate("html", "default/khelpcenter/main.html");

  hc->openURL(_url, true);
  hc->show();

  cout << "+ HelpWindow_Impl" << endl;
}
  
HelpWindow_Impl::~HelpWindow_Impl()
{
  if (hc)
    delete hc;
  cout << "- HelpWindow_Impl" << endl;
}

void HelpWindow_Impl::open(const char *url)
{
    QString _url(url);

    if (_url.isEmpty())
      _url = "file:" + locate("html", "default/khelpcenter/main.html");
    
    if (hc)
	hc->openURL(_url, true);
}
