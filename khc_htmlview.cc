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
#include <kapp.h>
#include <klocale.h>
#include <kpixmapcache.h>
#include <kmessagebox.h>

#include <qclipboard.h>

#include <opUIUtils.h>

#define TB_SEARCH Browser::View::TOOLBAR_ITEM_ID_BEGIN

khcHTMLView::khcHTMLView()
{
  ADD_INTERFACE("IDL:KHelpCenter/HTMLView:1.0");
  ADD_INTERFACE("IDL:Browser/PrintingExtension:1.0");
  ADD_INTERFACE("IDL:Browser/MagnifyingExtension:1.0");

  setWidget(this);
  
  fontBase = 3;

  m_pFindDlg = 0L;
  
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
  QWidget::show();
}

khcHTMLView::~khcHTMLView()
{
  delete m_pFindDlg;
}

bool khcHTMLView::event(const char *event, const CORBA::Any &value)
{
  EVENT_MAPPER(event, value);
  
  MAPPING(Browser::View::eventFillMenuEdit, Browser::View::EventFillMenu_ptr, mappingFillMenuEdit);
  MAPPING(Browser::View::eventFillMenuView, Browser::View::EventFillMenu_ptr, mappingFillMenuView);
  MAPPING(Browser::View::eventFillToolBar, Browser::View::EventFillToolBar, mappingFillToolBar);
  MAPPING(Browser::eventOpenURL, Browser::EventOpenURL, mappingOpenURL);
  
  END_EVENT_MAPPER;
  return false;
}

bool khcHTMLView::mappingOpenURL(Browser::EventOpenURL eventURL)
{
  khcBaseView::mappingOpenURL(eventURL);
  KBrowser::openURL(QString(eventURL.url), (bool)eventURL.reload ); // implemented by kbrowser
  SIGNAL_CALL2("started", id(), CORBA::Any::from_string((char *)eventURL.url, 0));
  return true;
}

bool khcHTMLView::mappingFillMenuView(Browser::View::EventFillMenu_ptr viewMenu)
{
  m_vViewMenu = OpenPartsUI::Menu::_duplicate(viewMenu);
  if (!CORBA::is_nil(viewMenu))
    {
      CORBA::WString_var text;
	  viewMenu->insertItem4((text = Q2C(i18n("&Find in page..."))), this, "slotFind", 0, -1, -1);
	  viewMenu->insertItem4((text = Q2C(i18n("&Find next in page..."))), this, "slotFindNext", 0, -1, -1);
    }
  
  return true;
}

bool khcHTMLView::mappingFillMenuEdit( Browser::View::EventFillMenu_ptr editMenu)
{
  m_vEditMenu = OpenPartsUI::Menu::_duplicate(editMenu);
  if (!CORBA::is_nil(editMenu))
    {
      CORBA::WString_var text;
      editMenu->insertItem4((text = Q2C(i18n("&Copy"))), this, "slotCopy", 0, -1, -1);
	  
    }
  
  return true;
}

bool khcHTMLView::mappingFillToolBar(Browser::View::EventFillToolBar viewToolBar)
{
  if (CORBA::is_nil(viewToolBar.toolBar))
    return true;
  
  if (viewToolBar.create)
    {
      CORBA::WString_var toolTip = Q2C(i18n("Find"));
      OpenPartsUI::Pixmap_var pix = OPUIUtils::convertPixmap(*KPixmapCache::toolbarPixmap("search.xpm"));
      viewToolBar.toolBar->insertButton2(pix, TB_SEARCH,SIGNAL(clicked()), this, "slotFind", 
					 true, toolTip, viewToolBar.startIndex++);
    }
  else
    viewToolBar.toolBar->removeItem(TB_SEARCH);
  
  return true;
}


void khcHTMLView::slotURLClicked(QString url)
{
  SIGNAL_CALL2("started", id(), CORBA::Any::from_string((char *)url.latin1(), 0));
}

void khcHTMLView::slotShowURL(KHTMLView *, QString _url)
{
  if (_url.isEmpty())
    {
      SIGNAL_CALL1("setStatusBarText", CORBA::Any::from_wstring(0L, 0));
      return;
    }
  CORBA::WString_var wurl = Q2C(_url);
  SIGNAL_CALL1("setStatusBarText", CORBA::Any::from_wstring(wurl.out(), 0));
}

void khcHTMLView::slotSetTitle(QString title)
{
  CORBA::WString_var ctitle = Q2C(title);
  SIGNAL_CALL1("setStatusBarText", CORBA::Any::from_wstring(ctitle.out(), 0));
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

void khcHTMLView::slotCopy()
{
  QString text;
  
  getKHTMLWidget()->getSelectedText(text);
  QClipboard *cb = kapp->clipboard();
  cb->setText(text);
}

void khcHTMLView::slotFind()
{
  if (!m_pFindDlg)
    {
	  m_pFindDlg = new KFindTextDialog();
	  QObject::connect(m_pFindDlg, SIGNAL(find(const QRegExp &)), this, SLOT(slotFindNext(const QRegExp &)));
    }
  
  // reset the find iterator
  getKHTMLWidget()->findTextBegin();
  m_pFindDlg->show();
}

void khcHTMLView::slotFindNext()
{
  if (m_pFindDlg && !m_pFindDlg->regExp().isEmpty())
    {
	  slotFindNext(m_pFindDlg->regExp());
    }
  else
    {
	  // no find has been attempted yet - open the find dialog.
	  slotFind();
    }
}

void khcHTMLView::slotFindNext(const QRegExp &regExp)
{
  if (!getKHTMLWidget()->findTextNext(regExp))
    {
	  if(!KMessageBox::questionYesNo(this, i18n("Continue search from the top of the page?")))
		{
		  getKHTMLWidget()->findTextBegin();
		  slotFindNext(regExp);
		}
	  else
		{
		  getKHTMLWidget()->findTextEnd();
		  m_pFindDlg->hide();
		}
	}
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

void khcHTMLView::zoomIn()
{
  if(fontBase < 5)
    {
      fontBase++;
      setDefaultFontBase(fontBase);
      KBrowser::openURL(url(), true );
      SIGNAL_CALL2("started", id(), CORBA::Any::from_string((char *)url(), 0));
    }
}

void khcHTMLView::zoomOut()
{
  if(fontBase > 2)
    {
      fontBase--;
      setDefaultFontBase(fontBase);
      KBrowser::openURL(url(), true );
      SIGNAL_CALL2("started", id(), CORBA::Any::from_string((char *)url(), 0));
    }
}

CORBA::Boolean khcHTMLView::canZoomIn()
{
  return (CORBA::Boolean)(fontBase < 5);
}

CORBA::Boolean khcHTMLView::canZoomOut()
{
  return (CORBA::Boolean)(fontBase > 2);
}

CORBA::Long khcHTMLView::xOffset()
{
  return (CORBA::Long)KBrowser::xOffset();
}

CORBA::Long khcHTMLView::yOffset()
{
  return (CORBA::Long)KBrowser::yOffset();
}

void khcHTMLView::openURL(QString _url, bool _reload, int _xoffset, int _yoffset, const char */*_post_data*/)
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
