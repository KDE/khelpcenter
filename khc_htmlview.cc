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

  setDefaultBGColor(white);
  setDefaultTextColors(black, blue, blue);
  //setStandardFont();
  //setFixedFont();

  setUnderlineLinks(true);
  setURLCursor(KCursor().handCursor());
  QWidget::show();
}

khcHTMLView::~khcHTMLView()
{
  delete m_pFindDlg;
}

bool khcHTMLView::event(const QCString &event, const CORBA::Any &value)
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
  KHTMLWidget::openURL(QString(eventURL.url), eventURL.reload ); // implemented by khtmlwidget
  SIGNAL_CALL2("started", id(), eventURL.url);
  return true;
}

bool khcHTMLView::mappingFillMenuView(Browser::View::EventFillMenu_ptr viewMenu)
{
  m_vViewMenu = OpenPartsUI::Menu::_duplicate(viewMenu);
  if (!CORBA::is_nil(viewMenu))
    {
      viewMenu->insertItem4( i18n("&Find in page..."), this, "slotFind", 0, -1, -1);
      viewMenu->insertItem4( i18n("&Find next in page..."), this, "slotFindNext", 0, -1, -1);
    }

  return true;
}

bool khcHTMLView::mappingFillMenuEdit( Browser::View::EventFillMenu_ptr editMenu)
{
  m_vEditMenu = OpenPartsUI::Menu::_duplicate(editMenu);
  if (!CORBA::is_nil(editMenu))
    editMenu->insertItem4( i18n("&Copy"), this, "slotCopy", 0, -1, -1);

  return true;
}

bool khcHTMLView::mappingFillToolBar(Browser::View::EventFillToolBar viewToolBar)
{
  if (CORBA::is_nil(viewToolBar.toolBar))
    return true;

  if (viewToolBar.create)
    {
      QString toolTip = i18n("Find");
      OpenPartsUI::Pixmap_var pix = OPUIUtils::convertPixmap(*KPixmapCache::toolbarPixmap("search.png"));
      viewToolBar.toolBar->insertButton2(pix, TB_SEARCH,SIGNAL(clicked()), this, "slotFind",
					 true, toolTip, viewToolBar.startIndex++);
    }
  else
    viewToolBar.toolBar->removeItem(TB_SEARCH);

  return true;
}


void khcHTMLView::slotURLClicked(QString url)
{
  SIGNAL_CALL2( "started", id(), QCString(url.latin1()) );
}

void khcHTMLView::slotShowURL(const QString &_url)
{
  SIGNAL_CALL1("setStatusBarText", _url);
}

void khcHTMLView::slotSetTitle(QString title)
{
  SIGNAL_CALL1("setStatusBarText", title);
}

void khcHTMLView::slotStarted( const char *url )
{
  SIGNAL_CALL2("started", id(), QCString(url) );
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
  QString text = selectedText();
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
  findTextBegin();
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
  if (!findTextNext(regExp))
    {
	  if(!KMessageBox::questionYesNo(this, i18n("Continue search from the top of the page?")))
		{
		  findTextBegin();
		  slotFindNext(regExp);
		}
	  else
		{
		  m_pFindDlg->hide();
		}
	}
}

void khcHTMLView::stop()
{
  KHTMLWidget::slotStop();
}

void khcHTMLView::setDefaultFontBase(int fSize)
{
    resetFontSizes();
    if (fSize != 3)
    {
        int fontSizes[7];
        KHTMLWidget::fontSizes(fontSizes);

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

void khcHTMLView::zoomIn()
{
  if(fontBase < 5)
    {
      fontBase++;
      setDefaultFontBase(fontBase);
      KHTMLWidget::openURL(url(), true );
      SIGNAL_CALL2("started", id(), url());
    }
}

void khcHTMLView::zoomOut()
{
  if(fontBase > 2)
    {
      fontBase--;
      setDefaultFontBase(fontBase);
      KHTMLWidget::openURL(url(), true );
      SIGNAL_CALL2("started", id(), url());
    }
}

bool khcHTMLView::canZoomIn()
{
  return (fontBase < 5);
}

bool khcHTMLView::canZoomOut()
{
  return (fontBase > 2);
}

long khcHTMLView::xOffset()
{
  return contentsX();
}

long khcHTMLView::yOffset()
{
  return contentsY();
}

void khcHTMLView::openURL(QString _url, bool _reload, int _xoffset, int _yoffset, const char */*_post_data*/)
{
  Browser::EventOpenURL eventURL;
  eventURL.url = QCString(_url.ascii());
  eventURL.reload = _reload;
  eventURL.xOffset = _xoffset;
  eventURL.yOffset = _yoffset;
  SIGNAL_CALL1("openURL", eventURL);
}

QCString khcHTMLView::url()
{
  return KHTMLWidget::m_strURL.ascii();
}

void khcHTMLView::print()
{
    // ### FIXME
    //KHTMLWidget::print();
}

#include "khc_htmlview.moc"
