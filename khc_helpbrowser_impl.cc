/*
 *  khc_helpbrowser_impl.cc - part of the KDE Help Center
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

#include "khc_mainwindow.h"
#include "khc_helpbrowser_impl.h"

#include <kdebug.h>
#include <kstddirs.h>
#include <kglobal.h>


KHelpCenter::HelpBrowser_ptr HelpBrowserFactory_Impl::create()
{
    HelpBrowser_Impl *hbi = new HelpBrowser_Impl;
    return KHelpCenter::HelpBrowser::_duplicate(hbi);
}


HelpBrowser_Impl::HelpBrowser_Impl()
{
  khb = new khcMainWindow;
  khb->show();

  kdebug(KDEBUG_INFO,1400,"KHelpBrowser::HelpBrowser_Impl()");
}
  
HelpBrowser_Impl::~HelpBrowser_Impl()
{
  if (khb)
    delete khb;
  kdebug(KDEBUG_INFO,1400,"KHelpBrowser::~HelpBrowser_Impl()");
}

void HelpBrowser_Impl::open(const char *url)
{
    QString _url(url);

    if (_url.isEmpty())
    {
	_url =(QString("file:" + locate("html", "default/khelpcenter/main.html")));
    }
    
    //if (khb)
    //	khb->openURL(_url, true);
}
