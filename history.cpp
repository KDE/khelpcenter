/*
 *  This file is part of the KDE Help Center
 *
 *  Copyright (C) 2002 Frerich Raabe <raabe@kde.org>
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
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include "history.h"
#include "view.h"

#include <kaction.h>
#include <kapplication.h>
#include <kdebug.h>
#include <kmainwindow.h>
#include <kpopupmenu.h>
#include <kstdguiitem.h>
#include <kstringhandler.h>



using namespace KHC;

History *History::m_instance = 0;

History &History::self()
{
  if  ( !m_instance )
    m_instance = new History;
  return *m_instance;
}

History::History() : QObject(),
  m_goBuffer( 0 )
{
  m_entries.setAutoDelete( true );
}

History::~History()
{
}

void History::setupActions( KActionCollection *coll )
{
  QPair<KGuiItem, KGuiItem> backForward = KStdGuiItem::backAndForward();

  m_backAction = new KToolBarPopupAction( backForward.first, ALT+Key_Left,
      this, SLOT( back() ), coll, "back" );
  connect( m_backAction->popupMenu(), SIGNAL( activated( int ) ),
      this, SLOT( backActivated( int ) ) );
  connect( m_backAction->popupMenu(), SIGNAL( aboutToShow() ),
      this, SLOT( fillBackMenu() ) );
  m_backAction->setEnabled( false );

  m_forwardAction = new KToolBarPopupAction( backForward.second, ALT+Key_Right,
      this, SLOT( forward() ), coll,
      "forward" );
  connect( m_forwardAction->popupMenu(), SIGNAL( activated( int ) ),
      this, SLOT( forwardActivated( int ) ) );
  connect( m_forwardAction->popupMenu(), SIGNAL( aboutToShow() ),
      this, SLOT( fillForwardMenu() ) );
  m_backAction->setEnabled( false );
}

void History::installMenuBarHook( KMainWindow *mainWindow )
{
  QPopupMenu *goMenu = dynamic_cast<QPopupMenu *>(
      mainWindow->guiFactory()->container( "go_web", mainWindow ) );
  if ( goMenu ) {
    connect( goMenu, SIGNAL( aboutToShow() ), this, SLOT( fillGoMenu() ) );
    connect( goMenu, SIGNAL( activated( int ) ),
        this, SLOT( goMenuActivated( int ) ) );
    m_goMenuIndex = goMenu->count();
  }
}

void History::createEntry()
{
  // First, remove any forward history
  Entry * current = m_entries.current();
  if (current)
  {
    m_entries.at( m_entries.count() - 1 ); // go to last one
    for ( ; m_entries.current() != current ; )
    {
      if ( !m_entries.removeLast() ) { // and remove from the end (faster and easier)
        Q_ASSERT(0);
        return;
      }
      else
        m_entries.at( m_entries.count() - 1 );
    }
    // Now current is the current again.
  }
  // Append a new entry
  m_entries.append( new Entry ); // made current
  Q_ASSERT( m_entries.at() == (int) m_entries.count() - 1 );
}

void History::updateCurrentEntry( View *view )
{
  if ( m_entries.isEmpty() )
    return;

  Entry *current = m_entries.current();

  QDataStream stream( current->buffer, IO_WriteOnly );
  view->browserExtension()->saveState( stream );

  current->view = view;
  current->url = view->url();
  current->title = view->title();

  current->search = false;
}

void History::updateActions()
{
  m_backAction->setEnabled( canGoBack() );
  m_forwardAction->setEnabled( canGoForward() );
}

void History::back()
{
  kdDebug( 1400 ) << "History::back()" << endl;
  goHistoryActivated( -1 );
}

void History::backActivated( int id )
{
  kdDebug( 1400 ) << "History::backActivated(): id = " << id << endl;
  goHistoryActivated( -( m_backAction->popupMenu()->indexOf( id ) + 1 ) );
}

void History::forward()
{
  kdDebug( 1400 ) << "History::forward()" << endl;
  goHistoryActivated( 1 );
}

void History::forwardActivated( int id )
{
  kdDebug( 1400 ) << "History::forwardActivated(): id = " << id << endl;
  goHistoryActivated( m_forwardAction->popupMenu()->indexOf( id ) + 1 );
}

void History::goHistoryActivated( int steps )
{
  kdDebug( 1400 ) << "History::goHistoryActivated(): m_goBuffer = " << m_goBuffer << endl;
  if ( m_goBuffer )
    return;
  m_goBuffer = steps;
  QTimer::singleShot( 0, this, SLOT( goHistoryDelayed() ) );
}

void History::goHistoryDelayed()
{
  kdDebug( 1400 ) << "History::goHistoryDelayed(): m_goBuffer = " << m_goBuffer << endl;
  if ( !m_goBuffer )
    return;
  int steps = m_goBuffer;
  m_goBuffer = 0;
  goHistory( steps );
}

void History::goHistory( int steps )
{
  int newPos = m_entries.at() + steps;

  Entry *current = m_entries.at( newPos );
  Q_ASSERT( current );

  if ( current->search ) {
    current->view->lastSearch();
    return;
  }

  Entry h( *current );
  h.buffer.detach();

  QDataStream stream( h.buffer, IO_ReadOnly );

  h.view->closeURL();
  updateCurrentEntry( h.view );
  h.view->browserExtension()->restoreState( stream );

  updateActions();
}

void History::fillBackMenu()
{
  QPopupMenu *menu = m_backAction->popupMenu();
  menu->clear();
  fillHistoryPopup( menu, true, false, false );
}

void History::fillForwardMenu()
{
  QPopupMenu *menu = m_forwardAction->popupMenu();
  menu->clear();
  fillHistoryPopup( menu, false, true, false );
}

void History::fillGoMenu()
{
  KMainWindow *mainWindow = static_cast<KMainWindow *>( kapp->mainWidget() );
  QPopupMenu *goMenu = dynamic_cast<QPopupMenu *>( mainWindow->guiFactory()->container( QString::fromLatin1( "go" ), mainWindow ) );
  if ( !goMenu || m_goMenuIndex == -1 )
    return;

  for ( int i = goMenu->count() - 1 ; i >= m_goMenuIndex; i-- )
    goMenu->removeItemAt( i );

  // TODO perhaps smarter algorithm (rename existing items, create new ones only if not enough) ?

  // Ok, we want to show 10 items in all, among which the current url...

  if ( m_entries.count() <= 9 )
  {
    // First case: limited history in both directions -> show it all
    m_goMenuHistoryStartPos = m_entries.count() - 1; // Start right from the end
  } else
    // Second case: big history, in one or both directions
  {
    // Assume both directions first (in this case we place the current URL in the middle)
    m_goMenuHistoryStartPos = m_entries.at() + 4;

    // Forward not big enough ?
    if ( m_entries.at() > (int)m_entries.count() - 4 )
      m_goMenuHistoryStartPos = m_entries.count() - 1;
  }
  Q_ASSERT( m_goMenuHistoryStartPos >= 0 && (uint)m_goMenuHistoryStartPos < m_entries.count() );
  m_goMenuHistoryCurrentPos = m_entries.at(); // for slotActivated
  fillHistoryPopup( goMenu, false, false, true, m_goMenuHistoryStartPos );
}

void History::goMenuActivated( int id )
{
  KMainWindow *mainWindow = static_cast<KMainWindow *>( kapp->mainWidget() );
  QPopupMenu *goMenu = dynamic_cast<QPopupMenu *>( mainWindow->guiFactory()->container( QString::fromLatin1( "go" ), mainWindow ) );
  if ( !goMenu )
    return;

  // 1 for first item in the list, etc.
  int index = goMenu->indexOf(id) - m_goMenuIndex + 1;
  if ( index > 0 )
  {
    kdDebug(1400) << "Item clicked has index " << index << endl;
    // -1 for one step back, 0 for don't move, +1 for one step forward, etc.
    int steps = ( m_goMenuHistoryStartPos+1 ) - index - m_goMenuHistoryCurrentPos; // make a drawing to understand this :-)
    kdDebug(1400) << "Emit activated with steps = " << steps << endl;
    goHistory( steps );
  }
}

void History::fillHistoryPopup( QPopupMenu *popup, bool onlyBack, bool onlyForward, bool checkCurrentItem, uint startPos )
{
  Q_ASSERT ( popup ); // kill me if this 0... :/

  Entry * current = m_entries.current();
  QPtrListIterator<Entry> it( m_entries );
  if (onlyBack || onlyForward)
  {
    it += m_entries.at(); // Jump to current item
    if ( !onlyForward ) --it; else ++it; // And move off it
  } else if ( startPos )
    it += startPos; // Jump to specified start pos

  uint i = 0;
  while ( it.current() )
  {
    QString text = it.current()->title;
    text = KStringHandler::csqueeze(text, 50); //CT: squeeze
    text.replace( "&", "&&" );
    if ( checkCurrentItem && it.current() == current )
    {
      int id = popup->insertItem( text ); // no pixmap if checked
      popup->setItemChecked( id, true );
    } else
      popup->insertItem( text );
    if ( ++i > 10 )
      break;
    if ( !onlyForward ) --it; else ++it;
  }
}

bool History::canGoBack() const
{
  return m_entries.at() > 0;
}

bool History::canGoForward() const
{
  return m_entries.at() != static_cast<int>( m_entries.count() ) - 1;
}

#include "history.moc"
// vim:ts=2:sw=2:et
