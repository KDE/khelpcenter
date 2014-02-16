/*
 *  This file is part of the KDE Help Center
 *
 *  Copyright (C) 1999 Matthias Elter (me@kde.org)
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

#include "navigator.h"

#include <QDir>
#include <QFile>
#include <QPixmap>

#include <QLabel>
#include <QtXml/QtXml>
#include <QTextStream>
#include <QRegExp>
#include <QLayout>
#include <QPushButton>
#include <QFrame>
#include <QHBoxLayout>
#include <QBoxLayout>
#include <QVBoxLayout>
#include <QTreeWidgetItemIterator>

#include <KAction>
#include <KApplication>
#include <KConfig>
#include <KStandardDirs>
#include <KGlobal>
#include <KLocale>
#include <KDebug>

#include <KLineEdit>
#include <KMessageBox>
#include <KIconLoader>
#include <KCharsets>
#include <KDialog>
#include <KDesktopFile>
#include <KProtocolInfo>
#include <KServiceGroup>
#include <KServiceTypeTrader>
#include <KCModuleInfo>
#include <KCModule>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <QStandardPaths>

#include "navigatoritem.h"
#include "navigatorappitem.h"
#include "searchwidget.h"
#include "searchengine.h"
#include "docmetainfo.h"
#include "docentrytraverser.h"
#include "glossary.h"
#include "toc.h"
#include "view.h"
#include "infotree.h"
#include "mainwindow.h"
#include "plugintraverser.h"
#include "scrollkeepertreebuilder.h"
#include "kcmhelpcenter.h"
#include "formatter.h"
#include "history.h"
#include "prefs.h"

using namespace KHC;

Navigator::Navigator( View *view, QWidget *parent, const char *name )
   : QWidget( parent ), mIndexDialog( 0 ),
     mView( view ), mSelected( false )
{
    setObjectName( name );

    KConfigGroup config(KSharedConfig::openConfig(), "General");
    mShowMissingDocs = config.readEntry("ShowMissingDocs", false);

    mSearchEngine = new SearchEngine( view );
    connect( mSearchEngine, SIGNAL( searchFinished() ),
             SLOT( slotSearchFinished() ) );

    DocMetaInfo::self()->scanMetaInfo();

    QBoxLayout *topLayout = new QVBoxLayout( this );

    mSearchFrame = new QFrame( this );
    topLayout->addWidget( mSearchFrame );

    QBoxLayout *searchLayout = new QHBoxLayout( mSearchFrame );
    searchLayout->setSpacing( KDialog::spacingHint() );
    searchLayout->setMargin( 6 );

    mSearchEdit = new KLineEdit( mSearchFrame );
    mSearchEdit->setClearButtonShown(true);
    searchLayout->addWidget( mSearchEdit );
    connect( mSearchEdit, SIGNAL( returnPressed() ), SLOT( slotSearch() ) );
    connect( mSearchEdit, SIGNAL( textChanged( const QString & ) ),
             SLOT( checkSearchButton() ) );

    mSearchButton = new QPushButton( i18n("&Search"), mSearchFrame );
    searchLayout->addWidget( mSearchButton );
    connect( mSearchButton, SIGNAL( clicked() ), SLOT( slotSearch() ) );

    mTabWidget = new QTabWidget( this );
    topLayout->addWidget( mTabWidget );

    setupContentsTab();
    setupGlossaryTab();
    setupSearchTab();

    insertPlugins();
    hideSearch();
/*
    if ( !mSearchEngine->initSearchHandlers() ) {
      hideSearch();
    } else {
      mSearchWidget->updateScopeList();
      mSearchWidget->readConfig( KSharedConfig::openConfig().data() );
    }
    */
    connect( mTabWidget, SIGNAL( currentChanged( QWidget * ) ),
             SLOT( slotTabChanged( QWidget * ) ) );
}

Navigator::~Navigator()
{
  delete mSearchEngine;
}

SearchEngine *Navigator::searchEngine() const
{
  return mSearchEngine;
}

Formatter *Navigator::formatter() const
{
  return mView->formatter();
}

bool Navigator::showMissingDocs() const
{
  return mShowMissingDocs;
}

void Navigator::setupContentsTab()
{
    mContentsTree = new QTreeWidget( mTabWidget );
    mContentsTree->setFrameStyle( QFrame::NoFrame );
    mContentsTree->setAllColumnsShowFocus(true);
    mContentsTree->setRootIsDecorated(false);
    mContentsTree->headerItem()->setHidden(true);

    connect(mContentsTree, SIGNAL(itemActivated(QTreeWidgetItem*,int)),
            SLOT(slotItemSelected(QTreeWidgetItem*)));
    
    mTabWidget->addTab(mContentsTree, i18n("&Contents"));
}

void Navigator::setupSearchTab()
{
  
    mSearchWidget = new SearchWidget( mSearchEngine, mTabWidget );
    connect( mSearchWidget, SIGNAL( searchResult( const QString & ) ),
             SLOT( slotShowSearchResult( const QString & ) ) );
    connect( mSearchWidget, SIGNAL( scopeCountChanged( int ) ),
             SLOT( checkSearchButton() ) );
    connect( mSearchWidget, SIGNAL( showIndexDialog() ),
      SLOT( showIndexDialog() ) );

    mTabWidget->addTab( mSearchWidget, i18n("Search Options"));
    
}

void Navigator::setupGlossaryTab()
{
    mGlossaryTree = new Glossary( mTabWidget );
    connect( mGlossaryTree, SIGNAL( entrySelected( const GlossaryEntry & ) ),
             this, SIGNAL( glossSelected( const GlossaryEntry & ) ) );
    mTabWidget->addTab( mGlossaryTree, i18n( "G&lossary" ) );
}

void Navigator::insertPlugins()
{
  PluginTraverser t( this, mContentsTree );
  DocMetaInfo::self()->traverseEntries( &t );
}

void Navigator::insertParentAppDocs( const QString &name, NavigatorItem *topItem )
{
  kDebug(1400) << "Requested plugin documents for ID " << name;

  KServiceGroup::Ptr grp = KServiceGroup::childGroup( name );
  if ( !grp )
    return;

  KServiceGroup::List entries = grp->entries();
  KServiceGroup::List::ConstIterator it = entries.constBegin();
  KServiceGroup::List::ConstIterator end = entries.constEnd();
  for ( ; it != end; ++it ) {
    QString desktopFile = ( *it )->entryPath();
    if ( QDir::isRelativePath( desktopFile ) )
        desktopFile = KStandardDirs::locate( "apps", desktopFile );
    createItemFromDesktopFile( topItem, desktopFile );
  }
}

void Navigator::insertKCMDocs( const QString &name, NavigatorItem *topItem, const QString &type )
{
  kDebug(1400) << "Requested KCM documents for ID" << name;

  KService::List list;

  if ( type == QString("kcontrol") ) {
    list = KServiceTypeTrader::self()->query( "KCModule", "[X-KDE-ParentApp] == 'kcontrol'" );
  } else {
    list = KServiceTypeTrader::self()->query( "KCModule", "[X-KDE-ParentApp] == 'kinfocenter'" );
  }

  for ( KService::List::const_iterator it = list.constBegin(); it != list.constEnd(); ++it )
  {
    KService::Ptr s = (*it);
    KCModuleInfo m = KCModuleInfo(s);
    QString desktopFile = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QLatin1String("kde5/services/") + m.fileName() );
    createItemFromDesktopFile( topItem, desktopFile );
    }
}

void Navigator::insertIOSlaveDocs( const QString &name, NavigatorItem *topItem )
{
  kDebug(1400) << "Requested IOSlave documents for ID" << name;

  QStringList list = KProtocolInfo::protocols();
  list.sort();

  NavigatorItem *prevItem = 0;
  for ( QStringList::ConstIterator it = list.constBegin(); it != list.constEnd(); ++it )
  {
    QString docPath = KProtocolInfo::docPath(*it);
    if ( !docPath.isNull() )
    {
      // First parameter is ignored if second is an absolute path
      KUrl url(KUrl("help:/"), docPath);
      QString icon = KProtocolInfo::icon(*it);
      if ( icon.isEmpty() ) icon = "text-plain";
      DocEntry *entry = new DocEntry( *it, url.url(), icon );
      NavigatorItem *item = new NavigatorItem( entry, topItem, prevItem );
      prevItem = item;
      item->setAutoDeleteDocEntry( true );
    }
  }
}

void Navigator::createItemFromDesktopFile( NavigatorItem *topItem,
                                           const QString &file )
{
    KDesktopFile desktopFile( file );
    QString docPath = desktopFile.readDocPath();
    if ( !docPath.isNull() ) {
      // First parameter is ignored if second is an absolute path
      KUrl url(KUrl("help:/"), docPath);
      QString icon = desktopFile.readIcon();
      if ( icon.isEmpty() ) icon = "text-plain";
      DocEntry *entry = new DocEntry( desktopFile.readName(), url.url(), icon );
      NavigatorItem *item = new NavigatorItem( entry, topItem );
      item->setAutoDeleteDocEntry( true );
    }
}

void Navigator::insertInfoDocs( NavigatorItem *topItem )
{
  InfoTree *infoTree = new InfoTree( this );
  infoTree->build( topItem );
}

NavigatorItem *Navigator::insertScrollKeeperDocs( NavigatorItem *topItem,
                                                  NavigatorItem *after )
{
  ScrollKeeperTreeBuilder *builder = new ScrollKeeperTreeBuilder( this );
  return builder->build( topItem, after );
}

void Navigator::selectItem( const KUrl &url )
{
  kDebug() << "Navigator::selectItem(): " << url.url();

  if ( url.url() == "khelpcenter:home" ) {
    clearSelection();
    return;
  }

  // help:/foo&anchor=bar gets redirected to help:/foo#bar
  // Make sure that we match both the original URL as well as
  // its counterpart.
  KUrl alternativeURL = url;
  if (url.hasRef())
  {
     alternativeURL.setQuery("anchor="+url.ref());
     alternativeURL.setRef(QString());
  }

  // If the navigator already has the given URL selected, do nothing.
  NavigatorItem *item;
  item = static_cast<NavigatorItem *>( mContentsTree->currentItem() );
  if ( item && mSelected ) {
    KUrl currentURL ( item->entry()->url() );
    if ( (currentURL == url) || (currentURL == alternativeURL) ) {
      kDebug() << "URL already shown.";
      return;
    }
  }

  // First, populate the NavigatorAppItems if we don't want the home page
  if ( url != homeURL() ) {
    QTreeWidgetItemIterator it1( mContentsTree );
    while( (*it1) ) 
    {
      NavigatorAppItem *appItem = dynamic_cast<NavigatorAppItem *>( (*it1) );
      if ( appItem ) appItem->populate( true );
      ++it1;
    }
  }
  
  QTreeWidgetItemIterator it( mContentsTree );
  while ( (*it) ) {
    NavigatorItem *item = static_cast<NavigatorItem *>( (*it) );
    KUrl itemUrl( item->entry()->url() );
    if ( (itemUrl == url) || (itemUrl == alternativeURL) ) {
      mContentsTree->setCurrentItem( item );
      // If the current item was not selected and remained unchanged it
      // needs to be explicitly selected
      mContentsTree->setCurrentItem(item);
      item->setExpanded( true );
      break;
    }
    ++it;
  }
  if ( !(*it) ) {
    clearSelection();
  } else {
    mSelected = true;
  }
}

void Navigator::clearSelection()
{
  mContentsTree->clearSelection();
  mSelected = false;
}

void Navigator::slotItemSelected( QTreeWidgetItem *currentItem )
{
  if ( !currentItem ) return;

  mSelected = true;

  NavigatorItem *item = static_cast<NavigatorItem *>( currentItem );

  kDebug(1400) << item->entry()->name() << endl;

  item->setExpanded( !item->isExpanded() );

  KUrl url ( item->entry()->url() );

  
  
  if ( url.protocol() == "khelpcenter" ) {
      mView->closeUrl();
      History::self().updateCurrentEntry( mView );
      History::self().createEntry();
      showOverview( item, url );
  } else {
   
    emit itemSelected( url.url() );
  }

  mLastUrl = url;
}

void Navigator::openInternalUrl( const KUrl &url )
{
  if ( url.url() == "khelpcenter:home" ) {
    clearSelection();
    showOverview( 0, url );
    return;
  }

  selectItem( url );
  if ( !mSelected ) return;

  NavigatorItem *item =
    static_cast<NavigatorItem *>( mContentsTree->currentItem() );

  if ( item ) showOverview( item, url );
}

void Navigator::showOverview( NavigatorItem *item, const KUrl &url )
{
  mView->beginInternal( url );

  QString fileName = QStandardPaths::locate(QStandardPaths::GenericDataLocation, "khelpcenter/index.html.in" );
  if ( fileName.isEmpty() )
    return;

  QFile file( fileName );

  if ( !file.open( QIODevice::ReadOnly ) )
    return;

  QTextStream stream( &file );
  QString res = stream.readAll();

  QString title,name,content;
  uint childCount;

  if ( item ) {
    title = item->entry()->name();
    name = item->entry()->name();

    QString info = item->entry()->info();
    if ( !info.isEmpty() ) content = QLatin1String("<p>") + info + QLatin1String("</p>\n");

    childCount = item->childCount();
  } else {
    title = i18n("Start Page");
    name = i18n("KDE Help Center");

    childCount = mContentsTree->topLevelItemCount();
  }

  if ( childCount > 0 ) {
    QTreeWidgetItem *child;
    if ( item ) child = item;
    else child = mContentsTree->invisibleRootItem();

    mDirLevel = 0;

    content += createChildrenList( child );
  }
  else
    content += QLatin1String("<p></p>");

  res = res.arg(title).arg(name).arg(content);

  mView->write( res );

  mView->end();
}

QString Navigator::createChildrenList( QTreeWidgetItem *child )
{
  ++mDirLevel;

  QString t;

  t += QLatin1String("<ul>\n");

  int cc = child->childCount();
  for (int i=0;i<cc;i++) 
  {
    NavigatorItem *childItem = static_cast<NavigatorItem *>( child->child(i) );

    DocEntry *e = childItem->entry();

    t += QLatin1String("<li><a href=\"") + e->url() + QLatin1String("\">");
    if ( e->isDirectory() ) t += QLatin1String("<b>");
    t += e->name();
    if ( e->isDirectory() ) t += QLatin1String("</b>");
    t += QLatin1String("</a>");

    if ( !e->info().isEmpty() ) {
      t += QLatin1String("<br>") + e->info();
    }

    t += QLatin1String("</li>\n");

    if ( childItem->childCount() > 0 && mDirLevel < 2 ) {
      t += createChildrenList( childItem );
    }

  }

  t += QLatin1String("</ul>\n");

  --mDirLevel;

  return t;
}

void Navigator::slotSearch()
{
  
  kDebug(1400) << "Navigator::slotSearch()";

  if ( !checkSearchIndex() ) return;

  if ( mSearchEngine->isRunning() ) return;

  QString words = mSearchEdit->text();
  QString method = mSearchWidget->method();
  int pages = mSearchWidget->pages();
  QString scope = mSearchWidget->scope();

  kDebug(1400) << "Navigator::slotSearch() words: " << words;
  kDebug(1400) << "Navigator::slotSearch() scope: " << scope;

  if ( words.isEmpty() || scope.isEmpty() ) return;

  // disable search Button during searches
  mSearchButton->setEnabled(false);
  QApplication::setOverrideCursor(Qt::WaitCursor);

  if ( !mSearchEngine->search( words, method, pages, scope ) ) {
    slotSearchFinished();
    KMessageBox::sorry( this, i18n("Unable to run search program.") );
  }
  
}

void Navigator::slotShowSearchResult( const QString &url )
{
  QString u = url;
  u.replace( "%k", mSearchEdit->text() );

  emit itemSelected( u );
}

void Navigator::slotSearchFinished()
{
  mSearchButton->setEnabled(true);
  QApplication::restoreOverrideCursor();

  kDebug( 1400 ) << "Search finished.";
}

void Navigator::checkSearchButton()
{
  mSearchButton->setEnabled( !mSearchEdit->text().isEmpty() &&
    mSearchWidget->scopeCount() > 0 );
  mTabWidget->setCurrentIndex( mTabWidget->indexOf( mSearchWidget ) );
}


void Navigator::hideSearch()
{
  mSearchFrame->hide();
  mTabWidget->removeTab( mTabWidget->indexOf( mSearchWidget ) );
}

bool Navigator::checkSearchIndex()
{
  KConfigGroup cfg(KSharedConfig::openConfig(), "Search" );
  if ( cfg.readEntry( "IndexExists", false) ) return true;

  if ( mIndexDialog && !mIndexDialog->isHidden() ) return true;

  QString text = i18n( "A search index does not yet exist. Do you want "
                       "to create the index now?" );

  int result = KMessageBox::questionYesNo( this, text, QString(),
                                           KGuiItem(i18n("Create")),
                                           KGuiItem(i18n("Do Not Create")),
                                           QLatin1String("indexcreation") );
  if ( result == KMessageBox::Yes ) {
    showIndexDialog();
    return false;
  }

  return true;
}

void Navigator::slotTabChanged( QWidget *wid )
{
  if ( wid == mSearchWidget ) checkSearchIndex();
}

void Navigator::slotSelectGlossEntry( const QString &id )
{
  mGlossaryTree->slotSelectGlossEntry( id );
}

KUrl Navigator::homeURL()
{
  if ( !mHomeUrl.isEmpty() ) return mHomeUrl;

  KSharedConfig::Ptr cfg = KSharedConfig::openConfig();
  // We have to reparse the configuration here in order to get a
  // language-specific StartUrl, e.g. "StartUrl[de]".
  cfg->reparseConfiguration();
  mHomeUrl = cfg->group("General").readPathEntry( "StartUrl", QLatin1String("khelpcenter:home") );
  return mHomeUrl;
}

void Navigator::showIndexDialog()
{
  if ( !mIndexDialog ) {
    mIndexDialog = new KCMHelpCenter( mSearchEngine, this );
    connect( mIndexDialog, SIGNAL( searchIndexUpdated() ), mSearchWidget,
             SLOT( updateScopeList() ) );
  }
  mIndexDialog->show();
  mIndexDialog->raise();
}

void Navigator::readConfig()
{
  if ( Prefs::currentTab() == Prefs::Search ) {
    mTabWidget->setCurrentIndex( mTabWidget->indexOf( mSearchWidget ) );
  } else if ( Prefs::currentTab() == Prefs::Glossary ) {
    mTabWidget->setCurrentIndex( mTabWidget->indexOf( mGlossaryTree ) );
  } else {
    mTabWidget->setCurrentIndex( mTabWidget->indexOf( mContentsTree ) );
  }
}

void Navigator::writeConfig()
{
  if ( mTabWidget->currentWidget() == mSearchWidget ) {
    Prefs::setCurrentTab( Prefs::Search );
  } else if ( mTabWidget->currentWidget() == mGlossaryTree ) {
    Prefs::setCurrentTab( Prefs::Glossary );
  } else {
    Prefs::setCurrentTab( Prefs::Content );
  }
}

void Navigator::clearSearch()
{
  mSearchEdit->setText( QString() );
}

#include "navigator.moc"

// vim:ts=2:sw=2:et
