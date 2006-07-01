/*
 *  searchwidget.cpp - part of the KDE Help Center
 *
 *  Copyright (C) 1999 Matthias Elter (me@kde.org)
 *            (C) 2000 Matthias Hoelzer-Kluepfel (hoelzer@kde.org)
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

#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QBoxLayout>

#include <ksimpleconfig.h>
#include <kapplication.h>
#include <klocale.h>
#include <kdebug.h>
#include <kiconloader.h>

#include "scopeitem.h"
#include "docentrytraverser.h"
#include "kcmhelpcenter.h"
#include "prefs.h"
#include "searchengine.h"

#include "searchwidget.h"
#include <dbus/qdbusconnection.h>

namespace KHC {

SearchWidget::SearchWidget( SearchEngine *engine, QWidget *parent )
  : QWidget( parent ), mEngine( engine ),
  mScopeCount( 0 )
{
    QDBus::sessionBus().registerObject("/SearchWidget", this, QDBusConnection::ExportSlots);

  QBoxLayout *topLayout = new QVBoxLayout( this );
  topLayout->setMargin( 2 );
  topLayout->setSpacing( 2 );

  QBoxLayout *hLayout = new QHBoxLayout();
  topLayout->addLayout( hLayout );

  mMethodCombo = new QComboBox( this );
  mMethodCombo->addItem( i18n("and") );
  mMethodCombo->addItem( i18n("or") );

  QLabel *l = new QLabel( i18n("&Method:"), this );
  l->setBuddy( mMethodCombo );

  hLayout->addWidget( l );
  hLayout->addWidget( mMethodCombo );

  hLayout = new QHBoxLayout();
  topLayout->addLayout( hLayout );

  mPagesCombo = new QComboBox( this );
  mPagesCombo->addItem( "5" );
  mPagesCombo->addItem( "10" );
  mPagesCombo->addItem( "25" );
  mPagesCombo->addItem( "50" );
  mPagesCombo->addItem( "1000" );

  l = new QLabel( i18n("Max. &results:"), this );
  l->setBuddy( mPagesCombo );

  hLayout->addWidget( l );
  hLayout->addWidget( mPagesCombo );

  hLayout = new QHBoxLayout();
  topLayout->addLayout( hLayout );

  mScopeCombo = new QComboBox( this );
  for (int i=0; i < ScopeNum; ++i ) {
    mScopeCombo->addItem( scopeSelectionLabel( i ) );
  }
  connect( mScopeCombo, SIGNAL( activated( int ) ),
           SLOT( scopeSelectionChanged( int ) ) );

  l = new QLabel( i18n("&Scope selection:"), this );
  l->setBuddy( mScopeCombo );

  hLayout->addWidget( l );
  hLayout->addWidget( mScopeCombo );

  mScopeListView = new Q3ListView( this );
  mScopeListView->setRootIsDecorated( true );
  mScopeListView->addColumn( i18n("Scope") );
  topLayout->addWidget( mScopeListView, 1 );

  QPushButton *indexButton = new QPushButton( i18n("Build Search &Index..."),
                                              this );
  connect( indexButton, SIGNAL( clicked() ), SIGNAL( showIndexDialog() ) );
  topLayout->addWidget( indexButton );

// FIXME: Use SearchHandler on double-clicked document
#if 0
  connect( mScopeListView, SIGNAL( doubleClicked( Q3ListViewItem * ) ),
           SLOT( scopeDoubleClicked( Q3ListViewItem * ) ) );
#endif
  connect( mScopeListView, SIGNAL( clicked( Q3ListViewItem * ) ),
           SLOT( scopeClicked( Q3ListViewItem * ) ) );
}


SearchWidget::~SearchWidget()
{
  writeConfig( KGlobal::config() );
}


void SearchWidget::readConfig( KConfig *cfg )
{
  cfg->setGroup( "Search" );

  int scopeSelection = cfg->readEntry( "ScopeSelection", (int)ScopeDefault );
  mScopeCombo->setCurrentIndex( scopeSelection );
  if ( scopeSelection != ScopeDefault ) scopeSelectionChanged( scopeSelection );

  mMethodCombo->setCurrentIndex( Prefs::method() );
  mPagesCombo->setCurrentIndex( Prefs::maxCount() );

  if ( scopeSelection == ScopeCustom ) {
    cfg->setGroup( "Custom Search Scope" );
    Q3ListViewItemIterator it( mScopeListView );
    while( it.current() ) {
      if ( it.current()->rtti() == ScopeItem::rttiId() ) {
        ScopeItem *item = static_cast<ScopeItem *>( it.current() );
        item->setOn( cfg->readEntry( item->entry()->identifier(),
                                         QVariant(item->isOn()) ).toBool() );
      }
      ++it;
    }
  }

  checkScope();
}

void SearchWidget::writeConfig( KConfig *cfg )
{
  cfg->setGroup( "Search" );

  cfg->writeEntry( "ScopeSelection", mScopeCombo->currentIndex() );
  Prefs::setMethod( mMethodCombo->currentIndex() );
  Prefs::setMaxCount( mPagesCombo->currentIndex() );

  if ( mScopeCombo->currentIndex() == ScopeCustom ) {
    cfg->setGroup( "Custom Search Scope" );
    Q3ListViewItemIterator it( mScopeListView );
    while( it.current() ) {
      if ( it.current()->rtti() == ScopeItem::rttiId() ) {
        ScopeItem *item = static_cast<ScopeItem *>( it.current() );
        cfg->writeEntry( item->entry()->identifier(), item->isOn() );
      }
      ++it;
    }
  }
}

void SearchWidget::slotSwitchBoxes()
{
  Q3ListViewItemIterator it( mScopeListView );
  while( it.current() ) {
    if ( it.current()->rtti() == ScopeItem::rttiId() ) {
      ScopeItem *item = static_cast<ScopeItem *>( it.current() );
      item->setOn( !item->isOn() );
    }
    ++it;
  }

  checkScope();
}

void SearchWidget::scopeSelectionChanged( int id )
{
  Q3ListViewItemIterator it( mScopeListView );
  while( it.current() ) {
    if ( it.current()->rtti() == ScopeItem::rttiId() ) {
      ScopeItem *item = static_cast<ScopeItem *>( it.current() );
      bool state = item->isOn();
      switch( id ) {
        case ScopeDefault:
          state = item->entry()->searchEnabledDefault();
          break;
        case ScopeAll:
          state = true;
          break;
        case ScopeNone:
          state = false;
          break;
        default:
          break;
      }
      if ( state != item->isOn() ) {
        item->setOn( state );
      }
    }
    ++it;
  }

  checkScope();
}

QString SearchWidget::method()
{
  QString m = "and";
  if ( mMethodCombo->currentIndex() == 1)
    m = "or";

  return m;
}

int SearchWidget::pages()
{
  int p = mPagesCombo->currentText().toInt();

  return p;
}

QString SearchWidget::scope()
{
  QString scope;

  Q3ListViewItemIterator it( mScopeListView );
  while( it.current() ) {
    if ( it.current()->rtti() == ScopeItem::rttiId() ) {
      ScopeItem *item = static_cast<ScopeItem *>( it.current() );
      if ( item->isOn() ) {
        if ( !scope.isEmpty() ) scope += '&';
        scope += "scope=" + item->entry()->identifier();
      }
    }
    ++it;
  }

  return scope;
}

class ScopeTraverser : public DocEntryTraverser
{
  public:
    ScopeTraverser( SearchWidget *widget, int level ) :
      mWidget( widget ), mLevel( level ), mParentItem( 0 ) {}

    ~ScopeTraverser()
    {
      if( mParentItem && !mParentItem->childCount() ) delete mParentItem;
    }

    void process( DocEntry *entry )
    {
      if ( mWidget->engine()->canSearch( entry ) &&
           ( !mWidget->engine()->needsIndex( entry ) ||
           entry->indexExists( Prefs::indexDirectory() ) ) ) {
        ScopeItem *item = 0;
        if ( mParentItem ) {
          item = new ScopeItem( mParentItem, entry );
        } else {
          item = new ScopeItem( mWidget->listView(), entry );
        }
        item->setOn( entry->searchEnabled() );
      }
    }

    DocEntryTraverser *createChild( DocEntry *entry )
    {
      if ( mLevel >= mNestingLevel ) {
        ++mLevel;
        return this;
      } else {
        ScopeTraverser *t = new ScopeTraverser( mWidget, mLevel + 1 );
        Q3ListViewItem *item = 0;
        if ( mParentItem ) {
          item = new Q3ListViewItem( mParentItem, entry->name() );
        } else {
          item = new Q3ListViewItem( mWidget->listView(), entry->name() );
        }
        item->setOpen( true );
        t->mParentItem = item;
        return t;
      }
    }

    DocEntryTraverser *parentTraverser()
    {
      if ( mLevel > mNestingLevel ) return this;
      else return mParent;
    }

    void deleteTraverser()
    {
      if ( mLevel > mNestingLevel ) --mLevel;
      else delete this;
    }

  private:
    SearchWidget *mWidget;
    int mLevel;
    Q3ListViewItem *mParentItem;

    static int mNestingLevel;
};

int ScopeTraverser::mNestingLevel = 2;

void SearchWidget::searchIndexUpdated()
{
  KGlobal::config()->reparseConfiguration();
  updateScopeList();
  update();
}

void SearchWidget::updateScopeList()
{
  mScopeListView->clear();

  ScopeTraverser t( this, 0 );
  DocMetaInfo::self()->traverseEntries( &t );

  checkScope();
}

void SearchWidget::scopeDoubleClicked( Q3ListViewItem *item )
{
  if ( !item || item->rtti() != ScopeItem::rttiId() ) return;
  ScopeItem *scopeItem = static_cast<ScopeItem *>( item );

  QString searchUrl = scopeItem->entry()->search();

  kDebug() << "DoubleClick: " << searchUrl << endl;

  emit searchResult( searchUrl );
}

void SearchWidget::scopeClicked( Q3ListViewItem * )
{
  checkScope();

  mScopeCombo->setCurrentIndex( ScopeCustom );
}

QString SearchWidget::scopeSelectionLabel( int id ) const
{
  switch( id ) {
    case ScopeCustom:
      return i18n("Custom");
    case ScopeDefault:
      return i18n("Default");
    case ScopeAll:
      return i18n("All");
    case ScopeNone:
      return i18n("None");
    default:
      return i18n("unknown");
  }
}

void SearchWidget::checkScope()
{
  mScopeCount = 0;

  Q3ListViewItemIterator it( mScopeListView );
  while( it.current() ) {
    if ( it.current()->rtti() == ScopeItem::rttiId() ) {
      ScopeItem *item = static_cast<ScopeItem *>( it.current() );
      if ( item->isOn() ) {
        ++mScopeCount;
      }
      item->entry()->enableSearch( item->isOn() );
    }
    ++it;
  }

  emit scopeCountChanged( mScopeCount );
}

int SearchWidget::scopeCount() const
{
  return mScopeCount;
}

}

#include "searchwidget.moc"

// vim:ts=2:sw=2:et
