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

#include "searchwidget.h"
#include "searchwidget.moc"

class ScopeItem : public QCheckListItem
{
  public:
    ScopeItem( QListView *parent, DocEntry *entry )
      : QCheckListItem( parent, entry->name(), QCheckListItem::CheckBox ),
        mEntry( entry ) {}

    DocEntry *entry() { return mEntry; }
    
  private:
    DocEntry *mEntry;
};


SearchWidget::SearchWidget( QWidget *parent )
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
  connect(kcmButton, SIGNAL(clicked()), this, SLOT(slotKControl()));
  hbox->addWidget( kcmButton );

  mScopeListView = new QListView( this );
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
  // TODO: fix this ugly system call
  system("kcmshell Help/htmlsearch &");
}

void SearchWidget::slotSwitchBoxes()
{
  QListViewItemIterator it( mScopeListView );
  while( it.current() ) {
    ScopeItem *item = static_cast<ScopeItem *>( it.current() );
    item->setOn( !item->isOn() );
    ++it;
  }
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
    ScopeItem *item = static_cast<ScopeItem *>( it.current() );
    if ( item->isOn() ) {
      if ( !scope.isEmpty() ) scope += "+";
      scope += item->entry()->identifier() + "_" + item->entry()->lang();
    }
    ++it;
  }

  return scope;
}

void SearchWidget::updateScopeList()
{
  mScopeListView->clear();
  mScopeCount = 0;

  DocEntry::List entries = DocMetaInfo::self()->docEntries();
  DocEntry::List::ConstIterator it;
  for( it = entries.begin(); it != entries.end(); ++it ) {
    if ( !(*it)->search().isEmpty() ) {
      ScopeItem *item = new ScopeItem( mScopeListView, *it );
      item->setOn( (*it)->searchEnabled() );
      if ( (*it)->searchEnabled() ) mScopeCount++; 
    }
  }

  emit enableSearch( mScopeCount > 0 );
}

void SearchWidget::scopeDoubleClicked( QListViewItem *item )
{
  ScopeItem *scopeItem = static_cast<ScopeItem *>( item );
  if ( !item ) return;

  QString searchUrl = scopeItem->entry()->search();
  
  kdDebug() << "DoubleClick: " << searchUrl << endl;
  
  emit searchResult( searchUrl );
}

void SearchWidget::scopeClicked( QListViewItem *item )
{
  ScopeItem *scopeItem = static_cast<ScopeItem *>( item );
  if ( !scopeItem ) return;

  DocEntry *entry = scopeItem->entry();

  if ( scopeItem->isOn() ) {
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

  kdDebug() << "SearchWidget::scopeClicked(): count: " << mScopeCount << endl;

  emit enableSearch( mScopeCount > 0 );
}
