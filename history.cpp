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

#include <KAction>
#include <KActionCollection>
#include <KApplication>
#include <KDebug>
#include <KIcon>
#include <KXmlGuiWindow>
#include <KMenu>
#include <KStandardGuiItem>
#include <KStringHandler>
#include <KToolBarPopupAction>

#include <kxmlguifactory.h>

using namespace KHC;

// TODO: Needs complete redo!
// TODO: oh yeah

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
  m_entries_current = m_entries.end();
}

History::~History()
{
  qDeleteAll(m_entries);
}

void History::setupActions( KActionCollection *coll )
{
  QPair<KGuiItem, KGuiItem> backForward = KStandardGuiItem::backAndForward();

  m_backAction = new KToolBarPopupAction( KIcon( backForward.first.iconName() ), backForward.first.text(), this );
  coll->addAction( "back", m_backAction );
  m_backAction->setShortcuts(KStandardShortcut::back());
  
  connect(m_backAction, &KToolBarPopupAction::triggered, this, &History::back);

  connect( m_backAction->menu(), SIGNAL( triggered( QAction* ) ),
           SLOT( backActivated( QAction* ) ) );
  
  connect( m_backAction->menu(), SIGNAL( aboutToShow() ),
           SLOT( fillBackMenu() ) );
  
  m_backAction->setEnabled( false );

  m_forwardAction = new KToolBarPopupAction( KIcon( backForward.second.iconName() ), backForward.second.text(), this );
  coll->addAction( QLatin1String("forward"), m_forwardAction );
  m_forwardAction->setShortcuts(KStandardShortcut::forward());
  
  connect(m_forwardAction, &KToolBarPopupAction::triggered, this, &History::forward);

  connect( m_forwardAction->menu(), SIGNAL( triggered( QAction* ) ),
           SLOT( forwardActivated( QAction* ) ) );
  
  connect( m_forwardAction->menu(), SIGNAL( aboutToShow() ),
           SLOT( fillForwardMenu() ) );
  
  m_forwardAction->setEnabled( false );
}

void History::installMenuBarHook( KXmlGuiWindow *mainWindow )
{
  QMenu *goMenu = dynamic_cast<QMenu *>(
      mainWindow->guiFactory()->container( QLatin1String("go_web"), mainWindow ) );
  if ( goMenu ) 
  {
    connect(goMenu, &QMenu::aboutToShow, this, &History::fillGoMenu);
    
    connect(goMenu, &QMenu::triggered, this, &History::goMenuActivated);
    
    m_goMenuIndex = goMenu->actions().count();
  }
}

void History::createEntry()
{
  kDebug() << "History::createEntry()";

  // First, remove any forward history
  if (m_entries_current!=m_entries.end())
  {
  
    m_entries.erase(m_entries.begin(),m_entries_current);
    
    // If current entry is empty reuse it.
    if ( !(*m_entries_current)->view ) { 
      return;
    }
  }
  // Append a new entry
  m_entries_current = m_entries.insert(m_entries_current, new Entry ); // made current
}

void History::updateCurrentEntry( View *view )
{
  if ( m_entries.isEmpty() )
    return;

  QUrl url = view->url();

  Entry *current = *m_entries_current;

  QDataStream stream( &current->buffer, QIODevice::WriteOnly );
  view->browserExtension()->saveState( stream );

  current->view = view;

  if ( url.isEmpty() ) {
    qDebug() << "History::updateCurrentEntry(): internal url";
    url = view->internalUrl();
  }

  qDebug() << "History::updateCurrentEntry(): " << view->title()
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
  kDebug( 1400 ) << "History::back()";
  goHistoryActivated( -1 );
}

void History::backActivated( QAction *action )
{
  int id = action->data().toInt();
  kDebug( 1400 ) << "History::backActivated(): id = " << id;
  goHistoryActivated( -( id + 1 ) );
}

void History::forward()
{
  kDebug( 1400 ) << "History::forward()";
  goHistoryActivated( 1 );
}

void History::forwardActivated( QAction *action )
{
  int id = action->data().toInt();
  kDebug( 1400 ) << "History::forwardActivated(): id = " << id;
  goHistoryActivated( id + 1 );
}

void History::goHistoryActivated( int steps )
{
  kDebug( 1400 ) << "History::goHistoryActivated(): m_goBuffer = " << m_goBuffer;
  if ( m_goBuffer )
    return;
  m_goBuffer = steps;
  QTimer::singleShot( 0, this, SLOT( goHistoryDelayed() ) );
}

void History::goHistoryDelayed()
{
  kDebug( 1400 ) << "History::goHistoryDelayed(): m_goBuffer = " << m_goBuffer;
  if ( !m_goBuffer )
    return;
  int steps = m_goBuffer;
  m_goBuffer = 0;
  goHistory( steps );
}

void History::goHistory( int steps )
{
  kDebug() << "History::goHistory(): " << steps;

  // If current entry is empty remove it.
  Entry *current = *m_entries_current;
  if ( current && !current->view ) m_entries_current = m_entries.erase(m_entries_current);

  EntryList::iterator newPos = m_entries_current - steps;
  
  current = *newPos;
  if ( !current ) {
    qWarning() << "No History entry at position " << newPos - m_entries.begin() << endl;
    return;
  }

  if ( !current->view ) {
    qWarning() << "Empty history entry." ;
    return;
  }
  
  m_entries_current = newPos;

  if ( current->search ) {
    qDebug() << "History::goHistory(): search";
    current->view->lastSearch();
    return;
  }

  if ( current->url.scheme() == QLatin1String("khelpcenter") ) {
    qDebug() << "History::goHistory(): internal";
    emit goInternalUrl( current->url );
    return;
  }


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
  KXmlGuiWindow *mainWindow = static_cast<KXmlGuiWindow *>( kapp->activeWindow() );
  QMenu *goMenu = dynamic_cast<QMenu *>( mainWindow->guiFactory()->container( QLatin1String( "go" ), mainWindow ) );
  if ( !goMenu || m_goMenuIndex == -1 )
    return;

  for ( int i = goMenu->actions().count() - 1 ; i >= m_goMenuIndex; i-- )
    goMenu->removeAction( goMenu->actions()[i] );

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
    m_goMenuHistoryStartPos = (m_entries_current - m_entries.begin()) + 4;

    // Forward not big enough ?
    if ( m_goMenuHistoryStartPos > (int) m_entries.count() - 4 )
      m_goMenuHistoryStartPos = m_entries.count() - 1;
  }
  Q_ASSERT( m_goMenuHistoryStartPos >= 0 && (int) m_goMenuHistoryStartPos < m_entries.count() );
  m_goMenuHistoryCurrentPos = m_entries_current - m_entries.begin(); // for slotActivated
  fillHistoryPopup( goMenu, false, false, true, m_goMenuHistoryStartPos );
}

void History::goMenuActivated( QAction* action )
{
  KXmlGuiWindow *mainWindow = static_cast<KXmlGuiWindow *>( kapp->activeWindow() );
  QMenu *goMenu = dynamic_cast<QMenu *>( mainWindow->guiFactory()->container( QLatin1String( "go" ), mainWindow ) );
  if ( !goMenu )
    return;

  // 1 for first item in the list, etc.
  int index = goMenu->actions().indexOf(action) - m_goMenuIndex + 1;
  if ( index > 0 )
  {
    kDebug(1400) << "Item clicked has index " << index;
    // -1 for one step back, 0 for don't move, +1 for one step forward, etc.
    int steps = ( m_goMenuHistoryStartPos+1 ) - index - m_goMenuHistoryCurrentPos; // make a drawing to understand this :-)
    kDebug(1400) << "Emit activated with steps = " << steps;
    goHistory( steps );
  }
}

void History::fillHistoryPopup( QMenu *popup, bool onlyBack, bool onlyForward, bool checkCurrentItem, uint startPos )
{
  Q_ASSERT ( popup ); // kill me if this 0... :/

  Entry * current = *m_entries_current;
  QList<Entry*>::iterator it = m_entries.begin();
  if (onlyBack || onlyForward)
  {
    it = m_entries_current; // Jump to current item
    // And move off it
    if ( !onlyForward ) {
        if ( it != m_entries.end() ) ++it;
    } else {
        if ( it != m_entries.begin() ) --it;
    }
  } else if ( startPos )
    it += startPos; // Jump to specified start pos

  uint i = 0;
  while ( it != m_entries.end() )
  {
    QString text = (*it)->title;
    text = KStringHandler::csqueeze(text, 50); //CT: squeeze
    text.replace( '&', "&&" );
    QAction *action = popup->addAction( text );
    action->setData( i );
    if ( checkCurrentItem && *it == current )
    {
      action->setChecked( true ); // no pixmap if checked
    }
    if ( ++i > 10 )
      break;
    if ( !onlyForward ) {
        ++it;
    } else {
        if ( it == m_entries.begin() ) {
            it = m_entries.end();
        } else {
            --it;
        }
    }
  }
}

bool History::canGoBack() const
{
  return m_entries.size()>1 && EntryList::const_iterator(m_entries_current) != (m_entries.begin()+(m_entries.size()-1));
}

bool History::canGoForward() const
{
  return EntryList::const_iterator(m_entries_current) != m_entries.constBegin() && m_entries.size() > 1;
}

void History::dumpHistory() const {
  for(EntryList::const_iterator it = m_entries.constBegin() ; it!=m_entries.constEnd() ; ++it) {
    kDebug() << (*it)->title << (*it)->url << (it==EntryList::const_iterator(m_entries_current) ? "current" : "" ) ;
  }

}

#include "history.moc"
// vim:ts=2:sw=2:et
