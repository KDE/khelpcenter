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

#include <qstring.h>

#include <khtml.h>

#include "khc_baseview.h"

#define SCROLLBAR_WIDTH		16
#define BOOKMARK_ID_BASE	200
#define MAX_HISTORY_LENGHT      15

class kmbView : public khcBaseView
{
  Q_OBJECT

public:

  kmbView( QWidget *parent = 0, char *name = 0 );
  virtual ~kmbView();

  virtual bool mappingOpenURL( const QString& eventURL );

  virtual void stop();
  virtual QString url();
  virtual long xOffset();
  virtual long yOffset();
  virtual void print();
  virtual void zoomIn();
  virtual void zoomOut();
  virtual bool canZoomIn();
  virtual bool canZoomOut();
  virtual void openURL( const QString url, bool reload, int xoffset = 0, int yoffset = 0, const char* post_data = 0L);

public slots:

  void slotURLClicked( QString url );

protected slots:

  void slotSetTitle(const QString &title);
  void slotStarted(const QString& url);
  void slotCompleted();
  void slotCanceled();
  void slotScrollVert( int _y );
  void slotScrollHorz( int _y );
  void slotViewResized( const QSize & );

protected:

  virtual void resizeEvent( QResizeEvent * );
  void setDefaultFontBase(int fSize);
  void open( const QString url, bool reload, int xoffset = 0, int yoffset = 0);

private:

  int          m_fontBase;
  KHTMLWidget *m_pView;
};

#endif
