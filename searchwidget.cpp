/*
    SPDX-FileCopyrightText: 1999 Matthias Elter <me@kde.org>
    SPDX-FileCopyrightText: 2000 Matthias Hoelzer-Kluepfel <hoelzer@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "searchwidget.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QHash>
#include <QLabel>
#include <QVBoxLayout>

#include <KConfig>
#include <KLocalizedString>

#include "scopeitem.h"
#include "docentrytraverser.h"
#include "searchengine.h"
#include "khc_debug.h"

#include <prefs.h>

namespace KHC {

SearchWidget::SearchWidget( SearchEngine *engine, QWidget *parent )
  : QWidget( parent ), mEngine( engine )
{
  QBoxLayout *topLayout = new QVBoxLayout( this );
  topLayout->setContentsMargins( 2, 2, 2, 2 );
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
  mPagesCombo->addItem( QStringLiteral("5") );
  mPagesCombo->addItem( QStringLiteral("10") );
  mPagesCombo->addItem( QStringLiteral("25") );
  mPagesCombo->addItem( QStringLiteral("50") );
  mPagesCombo->addItem( QStringLiteral("1000") );

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
  connect(mScopeCombo, QOverload<int>::of(&QComboBox::activated), this, &SearchWidget::scopeSelectionChanged);

  l = new QLabel( i18n("&Scope selection:"), this );
  l->setBuddy( mScopeCombo );

  hLayout->addWidget( l );
  hLayout->addWidget( mScopeCombo );

  mScopeListView = new QTreeWidget( this );
  mScopeListView->setColumnCount( 1 );
  mScopeListView->setHeaderLabels( QStringList() << i18n("Scope") );
  topLayout->addWidget( mScopeListView, 1 );

// FIXME: Use SearchHandler on double-clicked document
#if 0
  connect(mScopeListView, &QTreeWidget::itemDoubleClicked, this, &SearchWidget::scopeDoubleClicked);
#endif
  connect(mScopeListView, &QTreeWidget::itemClicked, this, &SearchWidget::scopeClicked);
}


SearchWidget::~SearchWidget()
{
  writeConfig( KSharedConfig::openConfig().data() );
}


void SearchWidget::readConfig( KConfig *cfg )
{
  KConfigGroup searchGroup(cfg, "Search");
  int scopeSelection = searchGroup.readEntry( "ScopeSelection", (int)ScopeDefault );
  mScopeCombo->setCurrentIndex( scopeSelection );
  if ( scopeSelection != ScopeDefault ) scopeSelectionChanged( scopeSelection );

  mMethodCombo->setCurrentIndex( Prefs::method() );
  mPagesCombo->setCurrentIndex( Prefs::maxCount() );

  if ( scopeSelection == ScopeCustom ) {
    KConfigGroup searchScopeGroup(cfg, "Custom Search Scope" );
    QTreeWidgetItemIterator it( mScopeListView );
    while( *it ) {
      if ( (*it)->type() == ScopeItem::ScopeItemType ) {
        ScopeItem *item = static_cast<ScopeItem *>(*it);
        item->setOn( searchScopeGroup.readEntry(
                         item->entry()->identifier(),
                         item->isOn() ) );
      }
      ++it;
    }
  }

  checkScope();
}

void SearchWidget::writeConfig( KConfig *cfg )
{
  KConfigGroup cg (cfg, "Search");

  cg.writeEntry( "ScopeSelection", mScopeCombo->currentIndex() );
  Prefs::setMethod( mMethodCombo->currentIndex() );
  Prefs::setMaxCount( mPagesCombo->currentIndex() );

  if ( mScopeCombo->currentIndex() == ScopeCustom ) {
    KConfigGroup cg2 (cfg, "Custom Search Scope");
    QTreeWidgetItemIterator it( mScopeListView );
    while( (*it) ) {
      if ( (*it)->type() == ScopeItem::ScopeItemType ) {
        ScopeItem *item = static_cast<ScopeItem *>( (*it) );
        cg2.writeEntry( item->entry()->identifier(), item->isOn() );
      }
      ++it;
    }
  }
}

void SearchWidget::slotSwitchBoxes()
{
  QTreeWidgetItemIterator it( mScopeListView );
  while( (*it) ) {
    if ( (*it)->type() == ScopeItem::ScopeItemType ) {
      ScopeItem *item = static_cast<ScopeItem *>( (*it) );
      item->setOn( !item->isOn() );
    }
    ++it;
  }

  checkScope();
}

void SearchWidget::scopeSelectionChanged( int id )
{
  QTreeWidgetItemIterator it( mScopeListView );
  while( (*it) ) {
    if ( (*it)->type() == ScopeItem::ScopeItemType ) {
      ScopeItem *item = static_cast<ScopeItem *>( (*it) );
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

QString SearchWidget::method() const
{
  QString m = QStringLiteral("and");
  if ( mMethodCombo->currentIndex() == 1)
    m = QStringLiteral("or");

  return m;
}

int SearchWidget::pages() const
{
  int p = mPagesCombo->currentText().toInt();

  return p;
}

QStringList SearchWidget::scope() const
{
  QStringList scope;

  QTreeWidgetItemIterator it( mScopeListView );
  while( (*it) ) {
    if ( (*it)->type() == ScopeItem::ScopeItemType ) {
      ScopeItem *item = static_cast<ScopeItem *>( (*it) );
      if ( item->isOn() ) {
        scope += item->entry()->identifier();
      }
    }
    ++it;
  }

  return scope;
}

class ScopeTraverser : public DocEntryTraverser
{
  public:
    ScopeTraverser( SearchEngine *engine, QTreeWidgetItem *parentItem, int level ) :
      mEngine( engine ), mLevel( level ), mParentItem( parentItem ) {}

    ~ScopeTraverser()
    {
      if( mParentItem->type() == TemporaryItem && !mParentItem->childCount() ) delete mParentItem;
    }

    void process( DocEntry *entry ) override
    {
      if ( mEngine->canSearch( entry ) ) {
        ScopeItem *item = new ScopeItem( mParentItem, entry );
        item->setOn( entry->searchEnabled() );
        mItems.insert( entry, item );
      }
    }

    DocEntryTraverser *createChild( DocEntry *entry ) override
    {
      if ( mLevel >= MaxNestingLevel ) {
        ++mLevel;
        return this;
      } else {
        QTreeWidgetItem *item = mItems.value( entry );
        if ( !item ) {
          item = new QTreeWidgetItem( mParentItem, QStringList() << entry->name(), TemporaryItem );
        }
        item->setExpanded( true );
        ScopeTraverser *t = new ScopeTraverser( mEngine, item, mLevel + 1 );
        return t;
      }
    }

    DocEntryTraverser *parentTraverser() override
    {
      if ( mLevel > MaxNestingLevel ) return this;
      else return mParent;
    }

    void deleteTraverser() override
    {
      if ( mLevel > MaxNestingLevel ) --mLevel;
      else delete this;
    }

  private:
    SearchEngine *mEngine = nullptr;
    int mLevel;
    QTreeWidgetItem *mParentItem = nullptr;
    QHash<DocEntry *, QTreeWidgetItem *> mItems;

    enum { MaxNestingLevel = 2 };
    enum { TemporaryItem = QTreeWidgetItem::UserType + 100 };
};

void SearchWidget::searchIndexUpdated()
{
  KSharedConfig::openConfig()->reparseConfiguration();
  updateScopeList();
  update();
}

void SearchWidget::updateScopeList()
{
  mScopeListView->clear();

  ScopeTraverser t( mEngine, mScopeListView->invisibleRootItem(), 0 );
  DocMetaInfo::self()->traverseEntries( &t );

  checkScope();
}

void SearchWidget::scopeDoubleClicked( QTreeWidgetItem* item )
{
  if ( !item || item->type() != ScopeItem::ScopeItemType ) return;
  ScopeItem *scopeItem = static_cast<ScopeItem *>( item );

  QString searchUrl = scopeItem->entry()->search();

  qCDebug(KHC_LOG) << "DoubleClick: " << searchUrl;

  Q_EMIT searchResult( searchUrl );
}

void SearchWidget::scopeClicked( QTreeWidgetItem* )
{
  checkScope();

  mScopeCombo->setCurrentIndex( ScopeCustom );
}

QString SearchWidget::scopeSelectionLabel( int id )
{
  switch( id ) {
    case ScopeCustom:
      return i18nc("Label for searching documentation using custom (user defined) scope", "Custom");
    case ScopeDefault:
      return i18nc("Label for searching documentation using default search scope", "Default");
    case ScopeAll:
      return i18nc("Label for searching documentation in all subsections", "All");
    case ScopeNone:
      return i18nc("Label for scope that deselects all search subsections", "None");
    default:
      return i18nc("Label for Unknown search scope, that should never appear", "unknown");
  }
}

void SearchWidget::checkScope()
{
  mScopeCount = 0;

  QTreeWidgetItemIterator it( mScopeListView );
  while( (*it) ) {
    if ( (*it)->type() == ScopeItem::ScopeItemType ) {
      ScopeItem *item = static_cast<ScopeItem *>( (*it) );
      if ( item->isOn() ) {
        ++mScopeCount;
      }
      item->entry()->enableSearch( item->isOn() );
    }
    ++it;
  }

  Q_EMIT scopeCountChanged( mScopeCount );
}

int SearchWidget::scopeCount() const
{
  return mScopeCount;
}

}



// vim:ts=2:sw=2:et
