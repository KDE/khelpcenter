/*
 *  kib_view.cc - part of the KDE Help Center
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

#include "kib_view.h"

#include <qapp.h>

#include <kcursor.h>
#include <kdebug.h>

#include <opUIUtils.h>

kibView::kibView()
{
  ADD_INTERFACE("IDL:KInfoBrowser/View:1.0");
  ADD_INTERFACE("IDL:Browser/PrintingExtension:1.0");
  ADD_INTERFACE("IDL:Browser/MagnifyingExtension:1.0");  

  setWidget(this);

  fontBase = 3;

  view = new KHTMLWidget( this );
  CHECK_PTR( view );

  view->setURLCursor(KCursor::handCursor());
  view->setUnderlineLinks(true);
  view->setFocusPolicy(QWidget::StrongFocus);
  view->setFocus();
  view->setUpdatesEnabled(true);
  view->setDefaultBGColor(white);
  view->setDefaultTextColors(black, blue, blue);
  view->setGranularity(600);
  //view->setStandardFont();
  //view->setFixedFont();

  vert = new QScrollBar(0, 0, 12, view->height(), 0, QScrollBar::Vertical, this, "vert");
  horz = new QScrollBar(0, 0, 24, view->width(), 0, QScrollBar::Horizontal, this, "horz");
  
  QObject::connect(view, SIGNAL(setTitle(QString)), this, SLOT(slotSetTitle(QString)));
  QObject::connect(view, SIGNAL(documentDone()), this, SLOT(slotCompleted()));
  QObject::connect(view, SIGNAL(scrollVert(int)), this, SLOT(slotScrollVert(int)));
  QObject::connect(view, SIGNAL(scrollHorz(int)), this, SLOT(slotScrollHorz(int)));
  QObject::connect(vert, SIGNAL(valueChanged(int)), view, SLOT(slotScrollVert(int)));
  QObject::connect(horz, SIGNAL(valueChanged(int)), view, SLOT(slotScrollHorz(int)));
  QObject::connect(view, SIGNAL(resized(const QSize &)), SLOT(slotViewResized(const QSize &)));
}

kibView::~kibView()
{
}

void kibView::layout()
{
  int top = 0;
  int bottom = height();
  
  bottom -= SCROLLBAR_WIDTH;

  if (view->docWidth() > view->width())
    horz->show();
  else
    horz->hide();
  
  if (view->docHeight() > view->height())
    vert->show();
  else
    vert->hide();
  
  if(vert->isVisible() && horz->isVisible())
    {
      view->setGeometry(0, top, width() - SCROLLBAR_WIDTH, bottom-top);
      vert->setGeometry(width()-SCROLLBAR_WIDTH, top, SCROLLBAR_WIDTH, bottom-top);
      horz->setGeometry(0, bottom, width() - SCROLLBAR_WIDTH, SCROLLBAR_WIDTH);
    }
  else if (vert->isVisible())
    {
      view->setGeometry(0, top, width() - SCROLLBAR_WIDTH, height()-top);
      vert->setGeometry(width()-SCROLLBAR_WIDTH, top, SCROLLBAR_WIDTH, height()-top);
    }
  else if (horz->isVisible())
    {
      view->setGeometry(0, top, width(), bottom-top);
      horz->setGeometry(0, bottom, width(), SCROLLBAR_WIDTH);
    }
  else
    view->setGeometry(0, top, width(), height()-top);
}

void kibView::slotScrollVert(int _y)
{
    vert->setValue(_y);
}

void kibView::slotScrollHorz(int _x)
{
    horz->setValue(_x);
}

void kibView::slotViewResized(const QSize &)
{
  QApplication::setOverrideCursor(waitCursor);
  
  vert->setSteps(12, view->height() - 20); 
  horz->setSteps(24, view->width());
  
  if (view->docHeight() > view->height())
    vert->setRange(0, view->docHeight() - view->height());
  else
    vert->setRange(0, 0);
  
  QApplication::restoreOverrideCursor();
}

void kibView::resizeEvent(QResizeEvent *)
{
  layout();
}

void kibView::open(QString _url, bool _reload, int _xoffset, int _yoffset)
{
  m_strURL = _url;

  view->begin(_url);
  view->parse();
  view->write("<html><head><title>Test!</title></head><body>");
  view->write("<H2>Test!</H2>");
  view->write("<br>Viewing: " + _url + " not implemented, yet.<br>");
  view->end();

  view->gotoXY(_xoffset, _yoffset);
}

bool kibView::mappingOpenURL( Browser::EventOpenURL eventURL )
{
  open(QString(eventURL.url), (bool)eventURL.reload );
  SIGNAL_CALL2("started", id(), CORBA::Any::from_string((char *)eventURL.url, 0));
  SIGNAL_CALL2( "setLocationBarURL", id(), (char*)eventURL.url );
  return true;
}

void kibView::slotURLClicked( QString url )
{
  SIGNAL_CALL2("started", id(), CORBA::Any::from_string((char *)url.latin1(), 0));
}

void kibView::slotSetTitle( QString /*title*/ )
{
  //CORBA::WString_var ctitle = Q2C( title );
}

void kibView::slotStarted( const char *url )
{
  SIGNAL_CALL2("started", id(), CORBA::Any::from_string( (char *)url, 0 ) );
}

void kibView::slotCompleted()
{
  SIGNAL_CALL1("completed", id());
}

void kibView::slotCanceled()
{
  SIGNAL_CALL1("canceled", id());
}

void kibView::stop()
{
  view->stopParser();
}

void kibView::setDefaultFontBase(int fSize)
{
    view->resetFontSizes();
    if (fSize != 3)
    {
        int fontSizes[7];
        view->getFontSizes(fontSizes);

        if (fSize > 3)
        {
            for(int i = 0; i < 7; i++)
            {
                int j = i + fSize - 3;
                if ( j > 6)
                    j = 6;
                fontSizes[i] = fontSizes[j]; 
            }
        }
        else
        {
            for(int i = 7; i--;)
            {
                int j = i + fSize - 3;
                if ( j < 0)
                    j = 0;
                fontSizes[i] = fontSizes[j]; 
            }
        }
        view->setFontSizes(fontSizes);
    }
}

void kibView::zoomIn()
{
  if(fontBase < 5)
    {
      fontBase++;
      setDefaultFontBase(fontBase);
      open(url(), true);
      SIGNAL_CALL2("started", id(), CORBA::Any::from_string((char *)url(), 0));
    }
}

void kibView::zoomOut()
{
  if(fontBase > 2)
    {
      fontBase--;
      setDefaultFontBase(fontBase);
      open(url(), true);
      SIGNAL_CALL2("started", id(), CORBA::Any::from_string((char *)url(), 0));
    }
}

CORBA::Boolean kibView::canZoomIn()
{
  return (CORBA::Boolean)(fontBase < 5);
}

CORBA::Boolean kibView::canZoomOut()
{
  return (CORBA::Boolean)(fontBase > 2);
}

CORBA::Long kibView::xOffset()
{
  return (CORBA::Long) view->xOffset();
}

CORBA::Long kibView::yOffset()
{
  return (CORBA::Long) view->yOffset();
}

void kibView::openURL(QString _url, bool _reload, int _xoffset, int _yoffset, const char *_post_data)
{
  Browser::EventOpenURL eventURL;
  eventURL.url = CORBA::string_dup(_url);
  eventURL.reload = _reload;
  eventURL.xOffset = _xoffset;
  eventURL.yOffset = _yoffset;
  SIGNAL_CALL1("openURL", eventURL);
}

char *kibView::url()
{
  QString u = m_strURL;
  return CORBA::string_dup(u.ascii());
}

void kibView::print()
{
  view->print();
}

#include "kib_view.moc"
