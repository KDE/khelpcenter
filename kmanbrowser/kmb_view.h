/*
 *  kmb_view.h - part of the KDE Help Center
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

#ifndef __kmb_view_h__
#define __kmb_view_h__

#include "kmanbrowser.h"
#include "khc_baseview.h"

#include <khtml.h>

#include <opPart.h>
#include <qstring.h>

#define SCROLLBAR_WIDTH		16
#define BOOKMARK_ID_BASE	200
#define MAX_HISTORY_LENGHT  15

class kmbView : public KHTMLWidget,
		public khcBaseView,
		virtual public KManBrowser::View_skel
{
    Q_OBJECT

 public:
    kmbView();
    virtual ~kmbView();

    virtual bool mappingOpenURL( Browser::EventOpenURL eventURL );

    virtual void stop();
    virtual char *viewName() { return CORBA::string_dup("kmbView"); }
    virtual char *url();
    virtual CORBA::Long xOffset();
    virtual CORBA::Long yOffset();
    virtual void print();
    virtual void zoomIn();
    virtual void zoomOut();
    virtual CORBA::Boolean canZoomIn();
    virtual CORBA::Boolean canZoomOut();
    virtual void openURL( QString _url, bool _reload, int _xoffset = 0, int _yoffset = 0, const char *_post_data = 0L);

public slots:
  void slotURLClicked( QString url );

protected slots:
  void slotSetTitle(QString title);
  void slotStarted(const char *url);
  void slotCompleted();
  void slotCanceled();

protected:
  void setDefaultFontBase(int fSize);
  int fontBase;
};

#endif

