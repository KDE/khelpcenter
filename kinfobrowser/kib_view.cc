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

#include <qclipboard.h>

#include <kapp.h>
#include <kdebug.h>
#include <klocale.h>
#include <kcursor.h>
#include <kstddirs.h>
#include <kprocess.h>
#include <kmessagebox.h>

#include "kib_view.h"

kibView::kibView( QWidget *parent, char *name )
  : khcBaseView( parent, name )
  , m_pFindDlg( 0L )
  , m_fontBase( 3 )
{
  kDebugInfo( 1400, "kibView::kibView" );

  m_pProc = new KProcess();

  setBackgroundColor( red );

  connect( m_pProc, SIGNAL( processExited( KProcess* ) ), this ,SLOT( slotProcessExited( KProcess* ) ) );
  connect( m_pProc, SIGNAL( receivedStdout( KProcess*, char*, int ) ), this, SLOT( slotReceivedStdout( KProcess*, char*, int ) ) );

  m_pView = new KHTMLWidget( this );
  CHECK_PTR( m_pView );

  // TODO: remove this
  //m_pView->openURL( "file:/home/devel/public_html/index.html" );
  
  m_pView->setURLCursor( KCursor::handCursor() );
  m_pView->setUnderlineLinks( true );
  m_pView->setFocusPolicy( QWidget::StrongFocus );
  m_pView->setFocus();
  m_pView->setUpdatesEnabled( true );
  m_pView->setDefaultBGColor( white );
  m_pView->setDefaultTextColors( black, blue, blue );
  //m_pView->setStandardFont();
  //m_pView->setFixedFont();

  connect( m_pView, SIGNAL( setTitle( const QString& ) ), this, SLOT( slotSetTitle( const QString& ) ) );
  connect( m_pView, SIGNAL( documentDone() ), this, SLOT( slotCompleted() ) );
  connect( m_pView, SIGNAL( scrollVert( int ) ), this, SLOT( slotScrollVert( int ) ) );
  connect( m_pView, SIGNAL( scrollHorz( int ) ), this, SLOT( slotScrollHorz( int ) ) );
  connect( m_pView, SIGNAL( resized( const QSize& ) ), SLOT( slotM_PViewResized( const QSize& ) ) );
  connect( m_pView, SIGNAL( URLSelected( QString, int ) ), this, SLOT( slotURLSelected( QString, int ) ) );

  m_Info2html = locate( "data", "kinfobrowser/kde-info2html" );

  if( m_Info2html.isEmpty() )
  {
    KMessageBox::error( this, i18n( "Critical error: Info2html perl script not found.\n" ) + i18n( "Quit!" ) );
    kapp->quit();
  }
}

kibView::~kibView()
{
  delete m_pProc;
  delete m_pFindDlg;
}

void kibView::slotScrollVert( int y )
{
  m_pView->scrollBy( 0, y );
}

void kibView::slotScrollHorz( int x )
{
  m_pView->scrollBy( x, 0 );
}

void kibView::slotViewResized( const QSize& )
{
}

void kibView::resizeEvent( QResizeEvent* )
{
  m_pView->resize( size() );
}

void kibView::open( const QString& url, bool reload, int xoffset, int yoffset )
{
  kDebugInfo( 1400, "kibView::open" );
  
  m_url = url;

  int pos1 = url.find( '(' );
  int pos2 = url.find( ')' );

  if( pos2 == -1 || pos1 == -1 || pos2 < pos1 )
  {
    KMessageBox::error(this,"Invalid URL: \n\n" + url +
			    "\n\nKInfoBrowser can only handle URL's in the format: info:(<infofile>)<node>.\n"
			    "For example: info:(bash)Top"
			    , "KDE InfoBrowser" );
    return;
  }

  QString file = url.mid( pos1 + 1, pos2 - pos1 - 1);
  QString node = url.right( url.length() - pos2 - 1);

  kDebugInfo( 1400, "file: %s, node: %s", file.latin1(), node.latin1() );

/*
  m_pProc->kill();
  m_pProc->clearArguments();

  *m_pProc << "perl" << m_Info2html.latin1() << file.latin1() << node.latin1();
  
  m_pView->end();
  // TODO : use standard temp dir not hardcoded path 
  m_pView->begin( "file:/tmp", xoffset, yoffset );
  
  m_pProc->start( KProcess::NotifyOnExit, KProcess::Stdout );
*/
}

bool kibView::mappingOpenURL( const QString& eventURL )
{
  //SIGNAL_CALL2( "started", id(), eventURL.url );
  //SIGNAL_CALL2( "setLocationBarURL", id(), eventURL.url );
  open( eventURL, true );
  //open( eventURL, eventURL.reload );
  return true;
}

void kibView::slotURLSelected( const QString& url, int /*button*/)
{
  // replace "%20" with " "

/*
  TODO: reactivate this
  
  int pos = url.find( "%20" );
  
  while( pos > -1 )
  {
    url.replace( (unsigned int) pos, 3, " " );
    pos = url.find( "%20" );
  }

  // ask the mainview to open this url, as it might not be suited for this view.
  openURL( url, false, 0, 0 );
*/
}

void kibView::slotSetTitle( const QString& title )
{
}

void kibView::slotStarted( const QString& url )
{
  //SIGNAL_CALL2("started", id(), QCString(url) );
}

void kibView::slotCompleted()
{
}

void kibView::slotCanceled()
{
}

void kibView::stop()
{
  // FIXME
  //m_pView->stopParser();
}

void kibView::setDefaultFontBase( int fSize )
{
  m_pView->resetFontSizes();

  if( fSize != 3 )
  {
    int fontSizes[ 7 ];
    m_pView->fontSizes( fontSizes );
	
    if( fSize > 3 )
    {
      for(int i = 0; i < 7; i++)
      {
        int j = i + fSize - 3;
	
	if( j > 6 ) j = 6;
			
	fontSizes[ i ] = fontSizes[ j ];
      }
    }
    else
    {
      for( int i = 7; i--; )
      {
        int j = i + fSize - 3;
	
	if( j < 0 ) j = 0;
			
	fontSizes[ i ] = fontSizes[ j ];
      }
    }
    m_pView->setFontSizes(fontSizes);
  }
}

void kibView::zoomIn()
{
  if( canZoomIn() )
  {
    m_fontBase++;
    setDefaultFontBase( m_fontBase );
    //SIGNAL_CALL2( "started", id(), url() );
    open( url(), true );
  }
}

void kibView::zoomOut()
{
  if( canZoomOut() )
  {
    m_fontBase--;
    setDefaultFontBase( m_fontBase );
    //SIGNAL_CALL2( "started", id(), url() );
    open( url(), true );
  }
}

bool kibView::canZoomIn()
{
  return (m_fontBase < 5);
}

bool kibView::canZoomOut()
{
  return (m_fontBase > 2);
}

long kibView::xOffset()
{
  return m_pView->contentsX();
}

long kibView::yOffset()
{
  return m_pView->contentsY();
}

void kibView::openURL(const QString& url, bool reload, int xoffset, int yoffset, const char* post_data )
{
  kDebugInfo( 1400, "kibView::openURL" );
    
  open( url, reload, xoffset, yoffset );
}

QString kibView::url()
{
  return m_url;
}

void kibView::print()
{
  // FIXME
  //m_pView->print();
}

void kibView::slotProcessExited( KProcess* )
{
  m_pView->end();
}

void kibView::slotReceivedStdout( KProcess*, char *buffer, int buflen )
{
  QCString in( buffer, buflen );
  m_pView->write( QCString( buffer, buflen ).data() );
}

void kibView::slotCopy()
{
  QString text = m_pView->selectedText();
  QClipboard *cb = kapp->clipboard();
  cb->setText( text );
}

void kibView::slotFind()
{
  if( !m_pFindDlg )
  {
    m_pFindDlg = new KFindTextDialog();
    QObject::connect( m_pFindDlg, SIGNAL( find( const QRegExp& ) ), this, SLOT( slotFindNext( const QRegExp& ) ) );
  }

  // reset the find iterator
  m_pView->findTextBegin();
  m_pFindDlg->show();
}

void kibView::slotFindNext()
{
  if( m_pFindDlg && !m_pFindDlg->regExp().isEmpty() )
    slotFindNext( m_pFindDlg->regExp() );
  else
    slotFind();
}

void kibView::slotFindNext( const QRegExp& regExp )
{
  if( !m_pView->findTextNext( regExp ) )
  {
    if( !KMessageBox::questionYesNo( this, i18n( "Continue search from the top of the page?" ) ) )
    {
      m_pView->findTextBegin();
      slotFindNext( regExp );
    }
    else
      m_pFindDlg->hide();
  }
}

#include "kib_view.moc"
