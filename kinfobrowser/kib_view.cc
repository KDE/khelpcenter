/*
 *  kib_view.cc - part of KInfoBrowser
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

#include <qcstring.h>
#include <qclipboard.h>

#include <kcursor.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <klocale.h>
#include <kprocess.h>
#include <kmessagebox.h>
#include <kapp.h>

#include <opUIUtils.h>

kibView::kibView()
{
  ADD_INTERFACE("IDL:KInfoBrowser/View:1.0");
  ADD_INTERFACE("IDL:Browser/PrintingExtension:1.0");
  ADD_INTERFACE("IDL:Browser/MagnifyingExtension:1.0");  

  setWidget(this);

  m_pProc = new KProcess();
  QObject::connect(m_pProc, SIGNAL(processExited(KProcess *)),
				   this ,SLOT(slotProcessExited(KProcess *)));
  QObject::connect(m_pProc, SIGNAL(receivedStdout(KProcess *, char *, int)),
				   this, SLOT(slotReceivedStdout(KProcess *, char *, int)));
  
  m_fontBase = 3;
  
  m_pView = new KHTMLWidget(this);
  CHECK_PTR(m_pView);
  
  m_pView->setURLCursor(KCursor::handCursor());
  m_pView->setUnderlineLinks(true);
  m_pView->setFocusPolicy(QWidget::StrongFocus);
  m_pView->setFocus();
  m_pView->setUpdatesEnabled(true);
  m_pView->setDefaultBGColor(white);
  m_pView->setDefaultTextColors(black, blue, blue);
  m_pView->setGranularity(600);
  //m_pView->setStandardFont();
  //m_pView->setFixedFont();

  m_pVert = new QScrollBar(0, 0, 12, m_pView->height(), 0, QScrollBar::Vertical, this, "vert");
  m_pHorz = new QScrollBar(0, 0, 24, m_pView->width(), 0, QScrollBar::Horizontal, this, "horz");
  
  QObject::connect(m_pView, SIGNAL(setTitle(QString)), this, SLOT(slotSetTitle(QString)));
  QObject::connect(m_pView, SIGNAL(documentDone()), this, SLOT(slotCompleted()));
  QObject::connect(m_pView, SIGNAL(scrollVert(int)), this, SLOT(slotScrollVert(int)));
  QObject::connect(m_pView, SIGNAL(scrollHorz(int)), this, SLOT(slotScrollHorz(int)));
  QObject::connect(m_pVert, SIGNAL(valueChanged(int)), m_pView, SLOT(slotScrollVert(int)));
  QObject::connect(m_pHorz, SIGNAL(valueChanged(int)), m_pView, SLOT(slotScrollHorz(int)));
  QObject::connect(m_pView, SIGNAL(resized(const QSize &)), SLOT(slotM_PViewResized(const QSize &)));
  QObject::connect(m_pView, SIGNAL(URLSelected(QString, int)), this, SLOT(slotURLSelected(QString, int)));

  m_Info2html = locate("data", "kinfobrowser/kde-info2html");

  if (m_Info2html.isEmpty())
	{
	  KMessageBox::error(this, i18n("Critical error: Info2html perl script not found.\n") + i18n("Quit!"));
	  kapp->quit();
	}
}

kibView::~kibView()
{
  delete m_pProc;
  delete m_pVert;
  delete m_pHorz;
  delete m_pView;
}

void kibView::layout()
{
  // do we need scrollbars?
  if (m_pView->docWidth() > width() - SCROLLBAR_WIDTH)
    m_pHorz->show();
  else
    m_pHorz->hide();
  
  if (m_pView->docHeight() > height() - SCROLLBAR_WIDTH)
    m_pVert->show();
  else
    m_pVert->hide();
  
  // calc geometry
  if(m_pVert->isVisible() && m_pHorz->isVisible())
    {
      m_pView->setGeometry(0, 0, width() - SCROLLBAR_WIDTH, height() -  SCROLLBAR_WIDTH);
      m_pVert->setGeometry(width()-SCROLLBAR_WIDTH, 0, SCROLLBAR_WIDTH, height() -  SCROLLBAR_WIDTH);
      m_pHorz->setGeometry(0, height() -  SCROLLBAR_WIDTH, width() - SCROLLBAR_WIDTH, SCROLLBAR_WIDTH);
    }
  else if (m_pVert->isVisible())
    {
      m_pView->setGeometry(0, 0, width() - SCROLLBAR_WIDTH, height());
      m_pVert->setGeometry(width() - SCROLLBAR_WIDTH, 0, SCROLLBAR_WIDTH, height());
    }
  else if (m_pHorz->isVisible())
    {
      m_pView->setGeometry(0, 0, width(), height() -  SCROLLBAR_WIDTH);
      m_pHorz->setGeometry(0, height() -  SCROLLBAR_WIDTH, width(), SCROLLBAR_WIDTH);
    }
  else
	m_pView->setGeometry(0, 0, width(), height());
  
  // calc ranges
  if (m_pView->docHeight() > m_pView->height())
    m_pVert->setRange(0, m_pView->docHeight() - m_pView->height());
  else
    m_pVert->setRange(0, 0);
  
  if (m_pView->docWidth() > m_pView->width())
    m_pHorz->setRange(0, m_pView->docWidth() - m_pView->width());
  else
    m_pHorz->setRange(0, 0);
}

void kibView::slotScrollVert(int _y)
{
  m_pVert->setValue(_y);
}

void kibView::slotScrollHorz(int _x)
{
  m_pHorz->setValue(_x);
}

void kibView::slotViewResized(const QSize &)
{
  QApplication::setOverrideCursor(waitCursor);
  
  m_pVert->setSteps(12, m_pView->height() - 20); 
  m_pHorz->setSteps(24, m_pView->width());
  
  if (m_pView->docHeight() > m_pView->height())
    m_pVert->setRange(0, m_pView->docHeight() - m_pView->height());
  else
    m_pVert->setRange(0, 0);
  
  QApplication::restoreOverrideCursor();
}

void kibView::resizeEvent(QResizeEvent *e)
{
  layout();
}

void kibView::open(QString _url, bool /*_reload*/, int _xoffset, int _yoffset)
{
  m_strURL = _url;
  
  int pos1 = _url.find('(');
  int pos2 = _url.find(')');
  
  if (pos2 == -1 || pos1 == -1 || pos2 < pos1)
	{
	  KMessageBox::error(this,"Invalid URL: \n\n" + _url +
						 "\n\nKInfoBrowser can only handle URL's in the format: info:(<infofile>)<node>.\n"
						 "For example: info:(bash)Top"
						 , "KDE InfoBrowser");
	  return;
	}
  
  QString file = _url.mid(pos1+1, pos2 - pos1 - 1);
  QString node = _url.right(_url.length() - pos2 - 1);
  
  kdebug(KDEBUG_INFO,1400,"file: %s, node: %s", file.latin1(), node.latin1());
  
  m_pProc->kill();
  m_pProc->clearArguments();
  
  *m_pProc << "perl" << m_Info2html.latin1() << file.latin1() << node.latin1();
  
  m_pView->end();
  m_pView->begin("file:/tmp", _xoffset, _yoffset);
  m_pView->parse();
  m_pProc->start(KProcess::NotifyOnExit, KProcess::Stdout);
}

bool kibView::mappingOpenURL( Browser::EventOpenURL eventURL )
{
  SIGNAL_CALL2("started", id(), CORBA::Any::from_string((char *)eventURL.url, 0));
  SIGNAL_CALL2( "setLocationBarURL", id(), (char*)eventURL.url );
  open(QString(eventURL.url), (bool)eventURL.reload );
  return true;
}

void kibView::slotURLSelected(QString url, int button)
{
  // replace "%20" with " "
  int pos = url.find("%20");
  while(pos > -1)
	{
	  url.replace(pos, 3, " "); 
	  pos = url.find("%20");
	}
  
  // ask the mainview to open this url, as it might not be suited for this view.
  openURL(url, false, 0, 0);
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
  layout();
  SIGNAL_CALL1("completed", id());
}

void kibView::slotCanceled()
{
  layout();
  SIGNAL_CALL1("canceled", id());
}

void kibView::stop()
{
  m_pView->stopParser();
}

void kibView::setDefaultFontBase(int fSize)
{
  m_pView->resetFontSizes();
  if (fSize != 3)
    {
	  int fontSizes[7];
	  m_pView->getFontSizes(fontSizes);
	  
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
	  m_pView->setFontSizes(fontSizes);
    }
}

void kibView::zoomIn()
{
  if(m_fontBase < 5)
    {
      m_fontBase++;
      setDefaultFontBase(m_fontBase);
	  SIGNAL_CALL2("started", id(), CORBA::Any::from_string((char *)url(), 0));
      open(url(), true);
    }
}

void kibView::zoomOut()
{
  if(m_fontBase > 2)
    {
      m_fontBase--;
      setDefaultFontBase(m_fontBase);
	  SIGNAL_CALL2("started", id(), CORBA::Any::from_string((char *)url(), 0));
      open(url(), true);
    }
}

CORBA::Boolean kibView::canZoomIn()
{
  return (CORBA::Boolean)(m_fontBase < 5);
}

CORBA::Boolean kibView::canZoomOut()
{
  return (CORBA::Boolean)(m_fontBase > 2);
}

CORBA::Long kibView::xOffset()
{
  return (CORBA::Long) m_pView->xOffset();
}

CORBA::Long kibView::yOffset()
{
  return (CORBA::Long) m_pView->yOffset();
}

void kibView::openURL(QString _url, bool _reload, int _xoffset, int _yoffset, const char */*_post_data*/)
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
  m_pView->print();
}

void kibView::slotProcessExited(KProcess *)
{
  m_pView->end();
}

void kibView::slotReceivedStdout(KProcess *, char *buffer, int buflen)
{
  QCString in(buffer, buflen);
  m_pView->write(QCString(buffer, buflen));
}

void kibView::slotCopy()
{
  QString text;
  
  m_pView->getSelectedText(text);
  QClipboard *cb = kapp->clipboard();
  cb->setText(text);
}

void kibView::slotSearch()
{
}

#include "kib_view.moc"
