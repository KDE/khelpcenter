/*
 *  khc_htmlview.h - part of the KDE Help Center
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

#ifndef __khc_htmlview_h__
#define __khc_htmlview_h__

#include "khelpcenter.h"
#include "khc_baseview.h"
#include "kbrowser.h"

#include <khtml.h>

#define SCROLLBAR_WIDTH		16
#define BOOKMARK_ID_BASE	200
#define MAX_HISTORY_LENGHT  15

class khcHTMLView : public KBrowser,
		    public khcBaseView,
		    virtual public KHelpCenter::HTMLView_skel
{
    Q_OBJECT

 public:
    khcHTMLView();
    virtual ~khcHTMLView();

    virtual bool mappingOpenURL( KHelpCenter::EventOpenURL eventURL );

    virtual void stop();
    virtual char *viewName() { return CORBA::string_dup("khcHTMLView"); }
    virtual char *url();
    virtual void print();
    virtual void openURL( QString _url, bool _reload, int _xoffset = 0, int _yoffset = 0, const char *_post_data = 0L);

public slots:
  void slotURLClicked( QString url );

protected slots:
  void slotShowURL( KHTMLView *view, QString _url );
  void slotSetTitle( QString title );
  void slotStarted( const char *url );
  void slotCompleted();
  void slotCanceled();


};

#endif

