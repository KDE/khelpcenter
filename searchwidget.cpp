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
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <qlabel.h>
#include <qvbox.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <qframe.h>
#include <qimage.h>
#include <qiconset.h>
#include <qapplication.h>
#include <qregexp.h>
#include <qscrollview.h>
#include <qwhatsthis.h>
#include <qlistview.h>

#include <kglobal.h>
#include <kstandarddirs.h>
#include <ksimpleconfig.h>
#include <kapplication.h>
#include <kprocess.h>
#include <klocale.h>
#include <kpixmap.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <kiconloader.h>

#include "scopeitem.h"
#include "docentrytraverser.h"

#include "searchwidget.h"
#include "searchwidget.moc"


namespace KHC {

KHC::SearchWidget::SearchWidget( QWidget *parent )
  : QWidget( parent ),
    mScopeCount( 0 )
{
  QVBoxLayout *vbox = new QVBoxLayout( this, 2, 2 );
  vbox->setAlignment( Qt::AlignLeft );

  mAdvOptions = new QFrame( this );
  vbox->addWidget( mAdvOptions );

  QVBoxLayout *vframebox = new QVBoxLayout( mAdvOptions, 3 );

  QHBoxLayout *hbox = new QHBoxLayout( vframebox );

  mMethodCombo = new QComboBox( mAdvOptions );
  mMethodCombo->insertItem( i18n("and") );
  mMethodCombo->insertItem( i18n("or") );

  QLabel *l = new QLabel( mMethodCombo, i18n("Method"), mAdvOptions);
  hbox->addWidget( l );
  hbox->addWidget( mMethodCombo );

  hbox = new QHBoxLayout( vframebox );

  mPagesCombo = new QComboBox( mAdvOptions );
  mPagesCombo->insertItem( "5" );
  mPagesCombo->insertItem( "10" );
  mPagesCombo->insertItem( "25" );
  mPagesCombo->insertItem( "50" );
  mPagesCombo->insertItem( "1000" );

  l = new QLabel( mPagesCombo, i18n("Max. &results"), mAdvOptions );
  hbox->addWidget( l );
  hbox->addWidget( mPagesCombo );

  vframebox->addSpacing( 10 );

  hbox = new QHBoxLayout( vframebox );

  // insert toggle box checked status button
  QPushButton *button = new QPushButton( "reload", mAdvOptions );
  button->setPixmap(KGlobal::iconLoader()->loadIcon("reload", KIcon::Toolbar));
  button->setFixedSize(button->sizeHint());
  connect(button, SIGNAL(clicked()), this, SLOT(slotSwitchBoxes()));
  hbox->addWidget( button );

  hbox->addStretch();

  // insert kcmshell launcher
  QPushButton *kcmButton = new QPushButton( "kcmshell", mAdvOptions );
  kcmButton->setPixmap(KGlobal::iconLoader()->loadIcon("package_settings", KIcon::Toolbar));
  kcmButton->setFixedSize(button->sizeHint());
  connect( kcmButton, SIGNAL( clicked() ), this, SLOT( slotIndex() ) );
  hbox->addWidget( kcmButton );

  mScopeListView = new QListView( this );
  mScopeListView->setRootIsDecorated( true );
  mScopeListView->addColumn( i18n("Scope") );
  vbox->addWidget( mScopeListView, 1 );

  connect( mScopeListView, SIGNAL( doubleClicked( QListViewItem * ) ),
           SLOT( scopeDoubleClicked( QListViewItem * ) ) );
  connect( mScopeListView, SIGNAL( clicked( QListViewItem * ) ),
           SLOT( scopeClicked( QListViewItem * ) ) );
}


SearchWidget::~SearchWidget()
{
}


void SearchWidget::slotIndex()
{
  kapp->startServiceByDesktopName( "kcmhelpcenter", QString::null );
}

void SearchWidget::slotSwitchBoxes()
{
  QListViewItemIterator it( mScopeListView );
  while( it.current() ) {
    if ( it.current()->rtti() == ScopeItem::rttiId() ) {
      ScopeItem *item = static_cast<ScopeItem *>( it.current() );
      item->setOn( !item->isOn() );
      updateScopeItem( item );
    }
    ++it;
  }

  emit enableSearch( mScopeCount > 0 );
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
      if ( !entry->search().isEmpty() && entry->indexExists() ) {
        ScopeItem *item = 0;
        if ( mParentItem ) {
          item = new ScopeItem( mParentItem, entry );
        } else {
          item = new ScopeItem( mWidget->listView(), entry );
        }
        mWidget->registerScopeItem( item );
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

void SearchWidget::registerScopeItem( ScopeItem *item )
{
  item->setOn( item->entry()->searchEnabled() );
  if ( item->entry()->searchEnabled() ) mScopeCount++;
}

void SearchWidget::updateScopeList()
{
  mScopeListView->clear();
  mScopeCount = 0;

  ScopeTraverser t( this, 0 );
  DocMetaInfo::self()->traverseEntries( &t );

  emit enableSearch( mScopeCount > 0 );
}

void SearchWidget::scopeDoubleClicked( QListViewItem *item )
{
  if ( !item || item->rtti() != ScopeItem::rttiId() ) return;
  ScopeItem *scopeItem = static_cast<ScopeItem *>( item );

  QString searchUrl = scopeItem->entry()->search();
  
  kdDebug() << "DoubleClick: " << searchUrl << endl;
  
  emit searchResult( searchUrl );
}

void SearchWidget::scopeClicked( QListViewItem *item )
{
  if ( !item || item->rtti() != ScopeItem::rttiId() ) return;
  ScopeItem *scopeItem = static_cast<ScopeItem *>( item );

  updateScopeItem( scopeItem );

//  kdDebug() << "SearchWidget::scopeClicked(): count: " << mScopeCount << endl;

  emit enableSearch( mScopeCount > 0 );
}

void SearchWidget::updateScopeItem( ScopeItem *item )
{
  DocEntry *entry = item->entry();

  if ( item->isOn() ) {
    if ( !entry->searchEnabled() ) {
      mScopeCount++;
      entry->enableSearch( true );
    }
  } else {
    if ( entry->searchEnabled() ) {
      mScopeCount--;
      entry->enableSearch( false );
    }
  }
}

}
// vim:ts=2:sw=2:et
