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
  
  QWidget::setFocusPolicy(StrongFocus);

  QObject::connect(this, SIGNAL(setTitle(QString)), this, SLOT(slotSetTitle(QString)));
  QObject::connect(this, SIGNAL(completed()), this, SLOT(slotCompleted()));

  setDefaultBGColor(white);
  setDefaultTextColors(black, blue, blue);
  setUnderlineLinks(true);
  setURLCursor(KCursor().handCursor());
  //setStandardFont();
  //setFixedFont();
}

kmbView::~kmbView()
{
}

bool kmbView::mappingOpenURL( Browser::EventOpenURL eventURL )
{
  //KBrowser::openURL(QString(eventURL.url), (bool)eventURL.reload ); // implemented by kbrowser
  SIGNAL_CALL2("started", id(), CORBA::Any::from_string((char *)eventURL.url, 0));
  SIGNAL_CALL2( "setLocationBarURL", id(), (char*)eventURL.url );
  return true;
}

void kmbView::slotURLClicked( QString url )
{
  SIGNAL_CALL2("started", id(), CORBA::Any::from_string((char *)url.latin1(), 0));
}

void kmbView::slotSetTitle( QString /*title*/ )
{
  //CORBA::WString_var ctitle = Q2C( title );
}

void kmbView::slotStarted( const char *url )
{
  SIGNAL_CALL2("started", id(), CORBA::Any::from_string( (char *)url, 0 ) );
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
  //KBrowser::slotStop();
}

void kmbView::setDefaultFontBase(int fSize)
{
    resetFontSizes();
    if (fSize != 3)
    {
        int fontSizes[7];
        getFontSizes(fontSizes);

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
        setFontSizes(fontSizes);
    }
}

void kmbView::zoomIn()
{
  if(fontBase < 5)
    {
      fontBase++;
      setDefaultFontBase(fontBase);
      //KBrowser::openURL(url(), true );
      SIGNAL_CALL2("started", id(), CORBA::Any::from_string((char *)url(), 0));
    }
}

void kmbView::zoomOut()
{
  if(fontBase > 2)
    {
      fontBase--;
      setDefaultFontBase(fontBase);
      //KBrowser::openURL(url(), true );
      SIGNAL_CALL2("started", id(), CORBA::Any::from_string((char *)url(), 0));
    }
}

CORBA::Boolean kmbView::canZoomIn()
{
  return (CORBA::Boolean)(fontBase < 5);
}

CORBA::Boolean kmbView::canZoomOut()
{
  return (CORBA::Boolean)(fontBase > 2);
}

CORBA::Long kmbView::xOffset()
{
  return (CORBA::Long) 0;//KBrowser::xOffset();
}

CORBA::Long kmbView::yOffset()
{
  return (CORBA::Long) 0;//KBrowser::yOffset();
}

void kmbView::openURL(QString _url, bool _reload, int _xoffset, int _yoffset, const char *_post_data)
{
  Browser::EventOpenURL eventURL;
  eventURL.url = CORBA::string_dup(_url);
  eventURL.reload = _reload;
  eventURL.xOffset = _xoffset;
  eventURL.yOffset = _yoffset;
  SIGNAL_CALL1("openURL", eventURL);
}

char *kmbView::url()
{
  QString u = "man://(index)";//KBrowser::m_strURL;
  return CORBA::string_dup(u.ascii());
}

void kmbView::print()
{
  KHTMLWidget::print();
}

#include "kmb_view.moc"
