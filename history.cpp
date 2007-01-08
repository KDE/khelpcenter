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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include "history.h"
#include "view.h"
#include <QMenu>
#include <kaction.h>
#include <kactioncollection.h>
#include <kapplication.h>
#include <kdebug.h>
#include <kicon.h>
#include <kmainwindow.h>
#include <kmenu.h>
#include <kstandardguiitem.h>
#include <kstringhandler.h>
#include <ktoolbarpopupaction.h>
#include <kxmlguifactory.h>


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
  QPair<KGuiItem, KGuiItem> backForward = KStandardGuiItem::backAndForward();

  m_backAction = new KToolBarPopupAction( KIcon( backForward.first.iconName() ), backForward.first.text(), this );
  coll->addAction( "back", m_backAction );
  m_backAction->setShortcut( Qt::ALT+Qt::Key_Left );
  connect( m_backAction, SIGNAL( triggered() ), this, SLOT( back() ) );

  connect( m_backAction->menu(), SIGNAL( activated( int ) ),
           SLOT( backActivated( int ) ) );
  connect( m_backAction->menu(), SIGNAL( aboutToShow() ),
           SLOT( fillBackMenu() ) );
  m_backAction->setEnabled( false );

  m_forwardAction = new KToolBarPopupAction( KIcon( backForward.second.iconName() ), backForward.second.text(), this );
  coll->addAction( QLatin1String("forward"), m_forwardAction );
  m_forwardAction->setShortcut( Qt::ALT+Qt::Key_Right );
  connect( m_forwardAction, SIGNAL( triggered() ), this, SLOT( forward() ) );

  connect( m_forwardAction->menu(), SIGNAL( activated( int ) ),
           SLOT( forwardActivated( int ) ) );
  connect( m_forwardAction->menu(), SIGNAL( aboutToShow() ),
           SLOT( fillForwardMenu() ) );
  m_forwardAction->setEnabled( false );
}
void History::installMenuBarHook( KMainWindow *mainWindow )
{
  QMenu *goMenu = dynamic_cast<QMenu *>(
      mainWindow->guiFactory()->container( QLatin1String("go_web"), mainWindow ) );
  if ( goMenu ) {
    connect( goMenu, SIGNAL( aboutToShow() ), SLOT( fillGoMenu() ) );
    connect( goMenu, SIGNAL( activated( int ) ),
             SLOT( goMenuActivated( int ) ) );
    m_goMenuIndex = goMenu->actions().count();
  }
}

void History::createEntry()
{
  kDebug() << "History::createEntry()" << endl;

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

    // If current entry is empty reuse it.
    if ( !current->view ) return;
  }

  // Append a new entry
  m_entries.append( new Entry ); // made current
  Q_ASSERT( m_entries.at() == (int) m_entries.count() - 1 );
}

void History::updateCurrentEntry( View *view )
{
  if ( m_entries.isEmpty() )
    return;

  KUrl url = view->url();

  Entry *current = m_entries.current();

  QDataStream stream( &current->buffer, QIODevice::WriteOnly );
  view->browserExtension()->saveState( stream );

  current->view = view;

  if ( url.isEmpty() ) {
    kDebug() << "History::updateCurrentEntry(): internal url" << endl;
    url = view->internalUrl();
  }

  kDebug() << "History::updateCurrentEntry(): " << view->title()
            << " (URL: " << url.url() << ")" << endl;

  current->url = url;
  current->title = view->title();

  current->search = view->state() == View::Search;
}

void History::updateActions()
{
  m_backAction->setEnabled( canGoBack() );
  m_forwardAction->setEnabled( canGoForward() );
}

void History::back()
{
  kDebug( 1400 ) << "History::back()" << endl;
  goHistoryActivated( -1 );
}

void History::backActivated( int id )
{
  kDebug( 1400 ) << "History::backActivated(): id = " << id << endl;
  goHistoryActivated( -( m_backAction->menu()->indexOf( id ) + 1 ) );
}

void History::forward()
{
  kDebug( 1400 ) << "History::forward()" << endl;
  goHistoryActivated( 1 );
}

void History::forwardActivated( int id )
{
  kDebug( 1400 ) << "History::forwardActivated(): id = " << id << endl;
  goHistoryActivated( m_forwardAction->menu()->indexOf( id ) + 1 );
}

void History::goHistoryActivated( int steps )
{
  kDebug( 1400 ) << "History::goHistoryActivated(): m_goBuffer = " << m_goBuffer << endl;
  if ( m_goBuffer )
    return;
  m_goBuffer = steps;
  QTimer::singleShot( 0, this, SLOT( goHistoryDelayed() ) );
}

void History::goHistoryDelayed()
{
  kDebug( 1400 ) << "History::goHistoryDelayed(): m_goBuffer = " << m_goBuffer << endl;
  if ( !m_goBuffer )
    return;
  int steps = m_goBuffer;
  m_goBuffer = 0;
  goHistory( steps );
}

void History::goHistory( int steps )
{
  kDebug() << "History::goHistory(): " << steps << endl;

  // If current entry is empty remove it.
  Entry *current = m_entries.current();
  if ( current && !current->view ) m_entries.remove();

  int newPos = m_entries.at() + steps;

  current = m_entries.at( newPos );
  if ( !current ) {
    kError() << "No History entry at position " << newPos << endl;
    return;
  }

  if ( !current->view ) {
    kWarning() << "Empty history entry." << endl;
    return;
  }

  if ( current->search ) {
    kDebug() << "History::goHistory(): search" << endl;
    current->view->lastSearch();
    return;
  }

  if ( current->url.protocol() == QLatin1String("khelpcenter") ) {
    kDebug() << "History::goHistory(): internal" << endl;
    emit goInternalUrl( current->url );
    return;
  }

  kDebug() << "History::goHistory(): restore state" << endl;

  emit goUrl( current->url );

  Entry h( *current );
  h.buffer.detach();

  QDataStream stream( h.buffer );

  h.view->closeUrl();
  updateCurrentEntry( h.view );
  h.view->browserExtension()->restoreState( stream );

  updateActions();
}

void History::fillBackMenu()
{
  QMenu *menu = m_backAction->menu();
  menu->clear();
  fillHistoryPopup( menu, true, false, false );
}

void History::fillForwardMenu()
{
  QMenu *menu = m_forwardAction->menu();
  menu->clear();
  fillHistoryPopup( menu, false, true, false );
}

void History::fillGoMenu()
{
  KMainWindow *mainWindow = static_cast<KMainWindow *>( kapp->activeWindow() );
  QMenu *goMenu = dynamic_cast<QMenu *>( mainWindow->guiFactory()->container( QLatin1String( "go" ), mainWindow ) );
  if ( !goMenu || m_goMenuIndex == -1 )
    return;

  for ( int i = goMenu->actions().count() - 1 ; i >= m_goMenuIndex; i-- )
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
  KMainWindow *mainWindow = static_cast<KMainWindow *>( kapp->activeWindow() );
  QMenu *goMenu = dynamic_cast<QMenu *>( mainWindow->guiFactory()->container( QLatin1String( "go" ), mainWindow ) );
  if ( !goMenu )
    return;

  // 1 for first item in the list, etc.
  int index = goMenu->indexOf(id) - m_goMenuIndex + 1;
  if ( index > 0 )
  {
    kDebug(1400) << "Item clicked has index " << index << endl;
    // -1 for one step back, 0 for don't move, +1 for one step forward, etc.
    int steps = ( m_goMenuHistoryStartPos+1 ) - index - m_goMenuHistoryCurrentPos; // make a drawing to understand this :-)
    kDebug(1400) << "Emit activated with steps = " << steps << endl;
    goHistory( steps );
  }
}

void History::fillHistoryPopup( QMenu *popup, bool onlyBack, bool onlyForward, bool checkCurrentItem, uint startPos )
{
  Q_ASSERT ( popup ); // kill me if this 0... :/

  Entry * current = m_entries.current();
  Q3PtrListIterator<Entry> it( m_entries );
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
