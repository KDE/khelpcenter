/*
 *  khc_htmlview.cc - part of the KDE Help Center
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

#include "khc_htmlview.h"

#include <kcursor.h>

#include <opUIUtils.h>

khcHTMLView::khcHTMLView()
{
  ADD_INTERFACE("IDL:KHelpCenter/HTMLView:1.0");
  ADD_INTERFACE("IDL:Browser/PrintingExtension:1.0");

  setWidget(this);

  fontBase = 3;
  
  QWidget::setFocusPolicy(StrongFocus);

  QObject::connect(this, SIGNAL(setTitle(QString)), this, SLOT(slotSetTitle(QString)));
  QObject::connect(this, SIGNAL(completed()), this, SLOT(slotCompleted()));

  KHTMLWidget* htmlWidget = getKHTMLWidget();

  htmlWidget->setDefaultBGColor(white);
  htmlWidget->setDefaultTextColors(black, blue, blue);
  //htmlWidget->setStandardFont();
  //htmlWidget->setFixedFont();

  htmlWidget->setUnderlineLinks(true);
  
  htmlWidget->setURLCursor(KCursor().handCursor());
}

khcHTMLView::~khcHTMLView()
{
}

bool khcHTMLView::mappingOpenURL( Browser::EventOpenURL eventURL )
{
  khcBaseView::mappingOpenURL(eventURL);
  KBrowser::openURL(QString(eventURL.url), (bool)eventURL.reload ); // implemented by kbrowser
  SIGNAL_CALL2("started", id(), CORBA::Any::from_string((char *)eventURL.url, 0));
  return true;
}

void khcHTMLView::slotURLClicked( QString url )
{
  SIGNAL_CALL2("started", id(), CORBA::Any::from_string((char *)url.latin1(), 0));
}

void khcHTMLView::slotShowURL( KHTMLView *, QString _url )
{
  if (_url.isEmpty())
    {
      SIGNAL_CALL1("setStatusBarText", CORBA::Any::from_wstring(0L, 0));
      return;
    }
  CORBA::WString_var wurl = Q2C(_url);
  SIGNAL_CALL1("setStatusBarText", CORBA::Any::from_wstring( wurl.out(), 0));
}

void khcHTMLView::slotSetTitle( QString /*title*/ )
{
  //CORBA::WString_var ctitle = Q2C( title );
}

void khcHTMLView::slotStarted( const char *url )
{
  SIGNAL_CALL2("started", id(), CORBA::Any::from_string( (char *)url, 0 ) );
}

void khcHTMLView::slotCompleted()
{
  SIGNAL_CALL1("completed", id());
}

void khcHTMLView::slotCanceled()
{
  SIGNAL_CALL1("canceled", id());
}

void khcHTMLView::stop()
{
  KBrowser::slotStop();
}

void khcHTMLView::setDefaultFontBase(int fSize)
{
    getKHTMLWidget()->resetFontSizes();
    if (fSize != 3)
    {
        int fontSizes[7];
        getKHTMLWidget()->getFontSizes(fontSizes);

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
        getKHTMLWidget()->setFontSizes(fontSizes);
    }
}

void khcHTMLView::slotZoomIn()
{
  if(fontBase < 5)
    {
      fontBase++;
      setDefaultFontBase(fontBase);
      KBrowser::slotReload();
    }
}

void khcHTMLView::slotZoomOut()
{
  if(fontBase > 2)
    {
      fontBase--;
      setDefaultFontBase(fontBase);
      KBrowser::slotReload();
    }
}

CORBA::Long khcHTMLView::xOffset()
{
  return (CORBA::Long)KBrowser::xOffset();
}

CORBA::Long khcHTMLView::yOffset()
{
  return (CORBA::Long)KBrowser::yOffset();
}

void khcHTMLView::openURL(QString _url, bool _reload, int _xoffset, int _yoffset, const char *_post_data)
{
  Browser::EventOpenURL eventURL;
  eventURL.url = CORBA::string_dup(_url);
  eventURL.reload = _reload;
  eventURL.xOffset = _xoffset;
  eventURL.yOffset = _yoffset;
  SIGNAL_CALL1("openURL", eventURL);
}

char *khcHTMLView::url()
{
  QString u = KBrowser::m_strURL;
  return CORBA::string_dup(u.ascii());
}

void khcHTMLView::print()
{
  KHTMLView::print();
}

#include "khc_htmlview.moc"
