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
#include "khc_finddlg.h"

#include <khtml.h>
#include <khtml.h>

#define SCROLLBAR_WIDTH		16
#define BOOKMARK_ID_BASE	200
#define MAX_HISTORY_LENGHT  15

class khcHTMLView : public KHTMLWidget,
		    public khcBaseView,
		    virtual public KHelpCenter::HTMLView_skel
{
    Q_OBJECT

 public:
    khcHTMLView();
    virtual ~khcHTMLView();

    virtual bool event(const QCString &event, const CORBA::Any &value);
    virtual bool mappingOpenURL(Browser::EventOpenURL eventURL);
    virtual bool mappingFillMenuView( Browser::View::EventFillMenu_ptr viewMenu );
    virtual bool mappingFillMenuEdit( Browser::View::EventFillMenu_ptr editMenu );
    virtual bool mappingFillToolBar( Browser::View::EventFillToolBar viewToolBar );

    virtual void stop();
    virtual QCString url();
    virtual long xOffset();
    virtual long yOffset();
    virtual void print();
    virtual void zoomIn();
    virtual void zoomOut();
    virtual bool canZoomIn();
    virtual bool canZoomOut();
    virtual void openURL( QString _url, bool _reload, int _xoffset = 0, int _yoffset = 0, const char *_post_data = 0L);

public slots:
  void slotURLClicked( QString url );
  virtual void slotCopy();
  virtual void slotFind();
  virtual void slotFindNext();

protected slots:
  void slotShowURL(const QString &_url);
  void slotSetTitle(QString title);
  void slotStarted(const char *url);
  void slotCompleted();
  void slotCanceled();
  void slotFindNext(const QRegExp &regExp);

protected:
  void setDefaultFontBase(int fSize);

  int fontBase;
  KFindTextDialog *m_pFindDlg;
  OpenPartsUI::Menu_var m_vViewMenu, m_vEditMenu;
};

#endif

