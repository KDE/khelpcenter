/*
 *  kib_view.h - part of KInfoBrowser
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

#ifndef __kib_view_h__
#define __kib_view_h__

#include "kinfobrowser.h"
#include "khc_baseview.h"
#include "khc_finddlg.h"

#include <qstring.h>
#include <qscrollbar.h>

#include <khtml.h>

#include <opPart.h>

#define SCROLLBAR_WIDTH		16
#define BOOKMARK_ID_BASE	200

class KProcess;

class kibView : public QWidget,
				public khcBaseView,
				virtual public KInfoBrowser::View_skel
{
  Q_OBJECT
    
public:
  kibView();
  virtual ~kibView();
  
  virtual bool mappingOpenURL( Browser::EventOpenURL eventURL );
  
  virtual void stop();
  virtual QCString url();
  virtual void print();
  virtual void zoomIn();
  virtual void zoomOut();

  virtual long xOffset();
  virtual long yOffset();
  virtual bool canZoomIn();
  virtual bool canZoomOut();

  virtual void openURL( QString _url, bool _reload, int _xoffset = 0, int _yoffset = 0,
			const char *_post_data = 0L);
  
public slots:
  virtual void slotCopy();
  virtual void slotFind();
  virtual void slotFindNext();

  void slotScrollVert( int _y );
  void slotScrollHorz( int _y );

protected slots:
  void slotSetTitle(QString title);
  void slotStarted(const char *url);
  void slotCompleted();
  void slotCanceled();
  void slotViewResized( const QSize & );
  void slotURLSelected(QString url, int button);
  void slotProcessExited(KProcess *proc);
  void slotReceivedStdout(KProcess *proc, char *buffer, int buflen);
  void slotFindNext(const QRegExp &regExp);

protected:
  virtual void resizeEvent(QResizeEvent *);
  void setDefaultFontBase(int fSize);
  void open(QString _url, bool _reload, int _xoffset = 0, int _yoffset = 0);
  void layout();

 protected:
  KHTMLWidget     *m_pView;
  KFindTextDialog *m_pFindDlg;
  KProcess        *m_pProc;
  QScrollBar      *m_pVert;
  QScrollBar      *m_pHorz;

  QString          m_Info2html;
  int              m_fontBase;
};

#endif

