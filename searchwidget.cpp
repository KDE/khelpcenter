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
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <qlabel.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qlayout.h>

#include <ksimpleconfig.h>
#include <kapplication.h>
#include <klocale.h>
#include <kdebug.h>
#include <kiconloader.h>

#include "scopeitem.h"
#include "docentrytraverser.h"
#include "kcmhelpcenter.h"
#include "prefs.h"

#include "searchwidget.h"

namespace KHC {

SearchWidget::SearchWidget( QWidget *parent )
  : QWidget( parent ), DCOPObject( "SearchWidget" ), mIndexDialog( 0 )
{
  QBoxLayout *topLayout = new QVBoxLayout( this, 2, 2 );

  QBoxLayout *hLayout = new QHBoxLayout( topLayout );

  mMethodCombo = new QComboBox( this );
  mMethodCombo->insertItem( i18n("and") );
  mMethodCombo->insertItem( i18n("or") );

  QLabel *l = new QLabel( mMethodCombo, i18n("&Method:"), this );

  hLayout->addWidget( l );
  hLayout->addWidget( mMethodCombo );

  hLayout = new QHBoxLayout( topLayout );

  mPagesCombo = new QComboBox( this );
  mPagesCombo->insertItem( "5" );
  mPagesCombo->insertItem( "10" );
  mPagesCombo->insertItem( "25" );
  mPagesCombo->insertItem( "50" );
  mPagesCombo->insertItem( "1000" );

  l = new QLabel( mPagesCombo, i18n("Max. &results:"), this );
  
  hLayout->addWidget( l );
  hLayout->addWidget( mPagesCombo );

  hLayout = new QHBoxLayout( topLayout );

  mScopeCombo = new QComboBox( this );
  for (int i=0; i < ScopeNum; ++i ) {
    mScopeCombo->insertItem( scopeSelectionLabel( i ) );
  }
  connect( mScopeCombo, SIGNAL( activated( int ) ),
           SLOT( scopeSelectionChanged( int ) ) );

  l = new QLabel( mScopeCombo, i18n("&Scope selection:"), this );

  hLayout->addWidget( l );
  hLayout->addWidget( mScopeCombo );

  mScopeListView = new QListView( this );
  mScopeListView->setRootIsDecorated( true );
  mScopeListView->addColumn( i18n("Scope") );
  topLayout->addWidget( mScopeListView, 1 );

  QPushButton *indexButton = new QPushButton( i18n("Create Search &Index..."),
                                              this );
  connect( indexButton, SIGNAL( clicked() ), SLOT( slotIndex() ) );
  topLayout->addWidget( indexButton );

  connect( mScopeListView, SIGNAL( doubleClicked( QListViewItem * ) ),
           SLOT( scopeDoubleClicked( QListViewItem * ) ) );
  connect( mScopeListView, SIGNAL( clicked( QListViewItem * ) ),
           SLOT( scopeClicked( QListViewItem * ) ) );
}


SearchWidget::~SearchWidget()
{
  writeConfig( KGlobal::config() );
}


void SearchWidget::readConfig( KConfig *cfg )
{
  cfg->setGroup( "Search" );

  int scopeSelection = cfg->readNumEntry( "ScopeSelection", ScopeDefault );
  mScopeCombo->setCurrentItem( scopeSelection );
  if ( scopeSelection != ScopeDefault ) scopeSelectionChanged( scopeSelection );

  mMethodCombo->setCurrentItem( Prefs::method() );
  mPagesCombo->setCurrentItem( Prefs::maxCount() );

  if ( scopeSelection == ScopeCustom ) {
    cfg->setGroup( "Custom Search Scope" );
    QListViewItemIterator it( mScopeListView );
    while( it.current() ) {
      if ( it.current()->rtti() == ScopeItem::rttiId() ) {
        ScopeItem *item = static_cast<ScopeItem *>( it.current() );
        item->setOn( cfg->readBoolEntry( item->entry()->identifier(),
                                         item->isOn() ) );
      }
      ++it;
    }
  }
  
  checkScope();
}

void SearchWidget::writeConfig( KConfig *cfg )
{
  cfg->setGroup( "Search" );
  
  cfg->writeEntry( "ScopeSelection", mScopeCombo->currentItem() );
  Prefs::setMethod( mMethodCombo->currentItem() );
  Prefs::setMaxCount( mPagesCombo->currentItem() );

  if ( mScopeCombo->currentItem() == ScopeCustom ) {
    cfg->setGroup( "Custom Search Scope" );
    QListViewItemIterator it( mScopeListView );
    while( it.current() ) {
      if ( it.current()->rtti() == ScopeItem::rttiId() ) {
        ScopeItem *item = static_cast<ScopeItem *>( it.current() );
        cfg->writeEntry( item->entry()->identifier(), item->isOn() );
      }
      ++it;
    }
  }
}

void SearchWidget::slotIndex()
{
  if ( !mIndexDialog ) {
    mIndexDialog = new KCMHelpCenter( this );
    connect( mIndexDialog, SIGNAL( finished() ), SLOT( updateConfig() ) );
    connect( mIndexDialog, SIGNAL( searchIndexUpdated() ),
             SLOT( updateScopeList() ) );
  }
  mIndexDialog->show();
  mIndexDialog->raise();
}

void SearchWidget::slotSwitchBoxes()
{
  QListViewItemIterator it( mScopeListView );
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
  QListViewItemIterator it( mScopeListView );
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
  if ( mMethodCombo->currentItem() == 1)
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

  QListViewItemIterator it( mScopeListView );
  while( it.current() ) {
    if ( it.current()->rtti() == ScopeItem::rttiId() ) {
      ScopeItem *item = static_cast<ScopeItem *>( it.current() );
      if ( item->isOn() ) {
        if ( !scope.isEmpty() ) scope += "&";
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
      if ( entry->isSearchable() ) {
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
        QListViewItem *item = 0;
        if ( mParentItem ) {
          item = new QListViewItem( mParentItem, entry->name() );
        } else {
          item = new QListViewItem( mWidget->listView(), entry->name() );
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
    QListViewItem *mParentItem;

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

void SearchWidget::scopeDoubleClicked( QListViewItem *item )
{
  if ( !item || item->rtti() != ScopeItem::rttiId() ) return;
  ScopeItem *scopeItem = static_cast<ScopeItem *>( item );

  QString searchUrl = scopeItem->entry()->search();
  
  kdDebug() << "DoubleClick: " << searchUrl << endl;
  
  emit searchResult( searchUrl );
}

void SearchWidget::scopeClicked( QListViewItem * )
{
  checkScope();

  mScopeCombo->setCurrentItem( ScopeCustom );
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
  int scopeCount = 0;

  QListViewItemIterator it( mScopeListView );
  while( it.current() ) {
    if ( it.current()->rtti() == ScopeItem::rttiId() ) {
      ScopeItem *item = static_cast<ScopeItem *>( it.current() );
      if ( item->isOn() ) {
        scopeCount += 1;
      }
      item->entry()->enableSearch( item->isOn() );
    }
    ++it;
  }
  
  emit enableSearch( scopeCount > 0 );
}

}

#include "searchwidget.moc"

// vim:ts=2:sw=2:et
