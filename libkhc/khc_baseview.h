/*
 *  khc_baseview.h - part of the KDE Help Center
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

#ifndef __khc_baseview_h__
#define __khc_baseview_h__

#include "browser.h"

#include <opPart.h>
#include <qstring.h>

/**
 * The base class for all views in khelpcenter
 */
class khcBaseView : virtual public OPPartIf,
		    virtual public Browser::View_skel
{
public:
    khcBaseView();
    ~khcBaseView();
  
    virtual void init();
    virtual void cleanUp();
  
    virtual bool event( const char *event, const CORBA::Any &value );
    virtual bool mappingOpenURL( Browser::EventOpenURL eventURL );
    
    /**
     * @return the URL shown by this view
     */
    virtual char *url();
    
    /**
     * Ask the mainwindow to open this URL, since it might not be suitable
     * for the current type of view.
     */
    virtual void openURLRequest(const char *_url);
    
protected:
    QString m_strURL;
};

#endif
