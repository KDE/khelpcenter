/*
 *  helpcentercom_impl.cpp - part of the KDE Help Center
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
#include "helpcentercom_impl.h"

HelpCenterCom::HelpCenterCom()
{
}

CORBA::ULong HelpCenterCom::openHelpView (const char *url, CORBA::Boolean )
{
    HelpCenter *hc = new HelpCenter;
  
    QString _url(url);

    if (_url.isEmpty())
    {
	_url = "file:";
	_url += kapp->kde_htmldir().copy();
	_url += "/default/khelpcenter/main.html";
    }
    hc->openURL(_url, true);
    hc->show();
    return hc->getListIndex();
}

void HelpCenterCom::openURL(const char *url, CORBA::ULong id)
{
    QString _url(url);

    if (_url.isEmpty())
    {
	_url = "file:";
	_url += kapp->kde_htmldir().copy();
	_url += "/default/khelpcenter/main.html";
    }
    HelpCenter *hc;
    hc = HelpCenter::getHelpWindowAt(id);
    if (hc)
	hc->openURL(_url, true);
}

void HelpCenterCom::configure()
{
    // call slotConfigure for all HelpCenter Windows
}
