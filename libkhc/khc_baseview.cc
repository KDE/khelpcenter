/*
 *  khc_baseview.cc - part of the KDE Help Center
 *
 *  Copyright (c) 1999 Matthias Elter (me@kde.org)
 *  based on konqueror code (c) Toben Weiss
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

#include "khc_baseview.h"
#include <kdebug.h>

khcBaseView::khcBaseView()
{
    ADD_INTERFACE("IDL:Browser/View:1.0");
    
    SIGNAL_IMPL("openURL");
    SIGNAL_IMPL("started");
    SIGNAL_IMPL("completed");
    SIGNAL_IMPL("canceled");
    SIGNAL_IMPL("setStatusBarText");
    SIGNAL_IMPL("setLocationBarURL");
    SIGNAL_IMPL("createNewWindow");
    
    OPPartIf::setFocusPolicy(OpenParts::Part::ClickFocus);
    
    m_strURL = "";
}

khcBaseView::~khcBaseView()
{
    kdebug(0,1400,"khcBaseView::~KHCBaseView()");
    cleanUp();
}

void khcBaseView::init()
{
}

void khcBaseView::cleanUp()
{
    if (m_bIsClean)
	return;
    
    kdebug(0,1400,"void khcBaseView::cleanUp()");
    
    OPPartIf::cleanUp();
}

bool khcBaseView::event( const QCString &event, const CORBA::Any &value )
{
    EVENT_MAPPER(event, value);
    
    MAPPING(Browser::eventOpenURL, Browser::EventOpenURL, mappingOpenURL);
    
    END_EVENT_MAPPER;
    return false;
}

bool khcBaseView::mappingOpenURL( Browser::EventOpenURL eventURL )
{
    SIGNAL_CALL2("setLocationBarURL", id(), eventURL.url);
    return false;
}

QCString khcBaseView::url()
{
    return m_strURL.latin1();
}

void khcBaseView::openURLRequest( const QCString &_url )
{
    // Ask the mainwindow to open this URL, since it might not be suitable
    // for the current type of view.
    Browser::URLRequest urlRequest;
    urlRequest.url = _url;
    urlRequest.reload = false;
    urlRequest.xOffset = 0;
    urlRequest.yOffset = 0;
    SIGNAL_CALL1( "openURL", urlRequest );
}

