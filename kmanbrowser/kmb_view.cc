/*
 *  kmb_view.cc - part of the KDE Help Center
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

#include "kmb_view.h"

#include <qapp.h>

#include <kcursor.h>
#include <kdebug.h>

#include <opUIUtils.h>

kmbView::kmbView()
{
  ADD_INTERFACE("IDL:KManBrowser/View:1.0");
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
  //view->setStandardFont();
  //view->setFixedFont();

  QObject::connect(view, SIGNAL(setTitle(const QString &)), this, SLOT(slotSetTitle(const QString &)));
  QObject::connect(view, SIGNAL(documentDone()), this, SLOT(slotCompleted()));
  // ### fix cursor scrolling
  //QObject::connect(view, SIGNAL(scrollVert(int)), this, SLOT(slotScrollVert(int)));
  //QObject::connect(view, SIGNAL(scrollHorz(int)), this, SLOT(slotScrollHorz(int)));
  QObject::connect(view, SIGNAL(resized(const QSize &)), SLOT(slotViewResized(const QSize &)));
}

kmbView::~kmbView()
{
}

void kmbView::layout()
{
}

void kmbView::slotScrollVert(int _y)
{
    view->scrollBy(0, _y);
}

void kmbView::slotScrollHorz(int _x)
{
    view->scrollBy(_x, 0);
}

void kmbView::slotViewResized(const QSize &)
{
  QApplication::setOverrideCursor(waitCursor);
  QApplication::restoreOverrideCursor();
}

void kmbView::resizeEvent(QResizeEvent *)
{
  layout();
}

void kmbView::open(QString _url, bool /*_reload*/, int _xoffset, int _yoffset)
{
  m_strURL = _url;

  view->begin(_url);
  view->write("<html><head><title>Test!</title></head><body>");
  view->write("<H2>Test!</H2>");
  view->write("<br>Viewing: " + _url + " not implemented, yet.<br>");
  view->end();

  view->setContentsPos(_xoffset, _yoffset);
}

bool kmbView::mappingOpenURL( Browser::EventOpenURL eventURL )
{
  open(QString(eventURL.url), eventURL.reload );
  SIGNAL_CALL2("started", id(), eventURL.url);
  SIGNAL_CALL2( "setLocationBarURL", id(), eventURL.url );
  return true;
}

void kmbView::slotURLClicked( QString url )
{
  SIGNAL_CALL2("started", id(), QCString(url.latin1()) );
}

void kmbView::slotSetTitle( const QString &/*title*/ )
{
  //CORBA::WString_var ctitle = Q2C( title );
}

void kmbView::slotStarted( const char *url )
{
  SIGNAL_CALL2("started", id(), QCString(url) );
}

void kmbView::slotCompleted()
{
  SIGNAL_CALL1("completed", id());
}

void kmbView::slotCanceled()
{
  SIGNAL_CALL1("canceled", id());
}

void kmbView::stop()
{
    // ### FIXME
    //view->stopParser();
}

void kmbView::setDefaultFontBase(int fSize)
{
    view->resetFontSizes();
    if (fSize != 3)
    {
        int fontSizes[7];
        view->fontSizes(fontSizes);

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

void kmbView::zoomIn()
{
  if(fontBase < 5)
    {
      fontBase++;
      setDefaultFontBase(fontBase);
      open(url(), true);
      SIGNAL_CALL2("started", id(), url() );
    }
}

void kmbView::zoomOut()
{
  if(fontBase > 2)
    {
      fontBase--;
      setDefaultFontBase(fontBase);
      open(url(), true);
      SIGNAL_CALL2("started", id(), url() );
    }
}

bool kmbView::canZoomIn()
{
  return (fontBase < 5);
}

bool kmbView::canZoomOut()
{
  return (fontBase > 2);
}

long kmbView::xOffset()
{
  return view->contentsX();
}

long kmbView::yOffset()
{
  return view->contentsY();
}

void kmbView::openURL(QString _url, bool _reload, int _xoffset, int _yoffset, const char */*_post_data*/)
{
  Browser::EventOpenURL eventURL;
  eventURL.url = _url.ascii();
  eventURL.reload = _reload;
  eventURL.xOffset = _xoffset;
  eventURL.yOffset = _yoffset;
  SIGNAL_CALL1("openURL", eventURL);
}

QCString kmbView::url()
{
  return m_strURL.ascii();
}

void kmbView::print()
{
    // ### FIXME
    //view->print();
}

#include "kmb_view.moc"
