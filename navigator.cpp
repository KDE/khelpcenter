/*
    SPDX-FileCopyrightText: 1999 Matthias Elter <me@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "navigator.h"

#include <QApplication>
#include <QDir>
#include <QProgressBar>
#include <QStandardPaths>
#include <QTabWidget>
#include <QTreeWidget>
#include <QTreeWidgetItemIterator>
#include <QVBoxLayout>

#include <KDesktopFile>
#include <KLineEdit>
#include <KMessageBox>
#include <KProcess>
#include <KProtocolInfo>
#include <KServiceGroup>
#include <KServiceTypeTrader>
#include <KShell>

#include "navigatoritem.h"
#include "navigatorappitem.h"
#include "navigatorappgroupitem.h"
#include "searchwidget.h"
#include "searchengine.h"
#include "searchhandler.h"
#include "docmetainfo.h"
#include "docentrytraverser.h"
#include "toc.h"
#include "view.h"
#include "infotree.h"
#include "plugintraverser.h"
#include "scrollkeepertreebuilder.h"
#include "history.h"
#include "khc_debug.h"
#include "grantleeformatter.h"

#include <prefs.h>

using namespace KHC;

Navigator::Navigator( View *view, QWidget *parent )
   : QWidget( parent ),
     mView( view ), mSelected( false ), mIndexingProc( nullptr )
{
    mSearchEngine = new SearchEngine( view );
    connect(mSearchEngine, &SearchEngine::searchFinished, this, &Navigator::slotSearchFinished);

    DocMetaInfo::self()->scanMetaInfo();

    QBoxLayout *topLayout = new QVBoxLayout( this );

    mSearchEdit = new KLineEdit(this);
    mSearchEdit->setPlaceholderText( i18n("Search...") );
    mSearchEdit->setClearButtonEnabled(true);
    topLayout->addWidget( mSearchEdit );
    connect(mSearchEdit, &KLineEdit::returnKeyPressed, this, &Navigator::slotSearch);
    connect(mSearchEdit, &KLineEdit::textChanged, this, &Navigator::checkSearchEdit);

    mTabWidget = new QTabWidget( this );
    topLayout->addWidget( mTabWidget );

    mIndexingBar = new QProgressBar( this );
    mIndexingBar->hide();
    topLayout->addWidget( mIndexingBar );

    mIndexingTimer.setSingleShot( true );
    mIndexingTimer.setInterval( 1000 );
    connect( &mIndexingTimer, &QTimer::timeout, this, &Navigator::slotShowIndexingProgressBar );

    setupContentsTab();
    setupGlossaryTab();
    setupSearchTab();

    insertPlugins();

    if ( !mSearchEngine->initSearchHandlers() ) {
      hideSearch();
    } else {
      mSearchWidget->updateScopeList();
      mSearchWidget->readConfig( KSharedConfig::openConfig().data() );
      QTimer::singleShot( 0, this, &Navigator::slotDelayedIndexingStart );
    }
}

Navigator::~Navigator()
{
  delete mSearchEngine;
}

SearchEngine *Navigator::searchEngine() const
{
  return mSearchEngine;
}

void Navigator::setupContentsTab()
{
    mContentsTree = new QTreeWidget( mTabWidget );
    mContentsTree->setFrameStyle( QFrame::NoFrame );
    mContentsTree->setAllColumnsShowFocus(true);
    mContentsTree->setRootIsDecorated(false);
    mContentsTree->headerItem()->setHidden(true);
    mContentsTree->setExpandsOnDoubleClick(false);

    connect(mContentsTree, &QTreeWidget::itemActivated, this, &Navigator::slotItemSelected);
    connect(mContentsTree, &QTreeWidget::itemExpanded, this, &Navigator::slotItemExpanded);
    connect(mContentsTree, &QTreeWidget::itemCollapsed, this, &Navigator::slotItemCollapsed);
    
    mTabWidget->addTab(mContentsTree, i18n("&Contents"));
}

void Navigator::setupSearchTab()
{
  
    mSearchWidget = new SearchWidget( mSearchEngine, mTabWidget );
    connect(mSearchWidget, &SearchWidget::searchResult, this, &Navigator::slotShowSearchResult);
    connect(mSearchWidget, &SearchWidget::scopeCountChanged, this, &Navigator::checkSearchEdit);

    mTabWidget->addTab( mSearchWidget, i18n("Search Options"));
    
}

void Navigator::setupGlossaryTab()
{
    mGlossaryTree = new Glossary( mTabWidget );
    connect(mGlossaryTree, &Glossary::entrySelected, this, &Navigator::glossSelected);
    mTabWidget->addTab( mGlossaryTree, i18n( "G&lossary" ) );
}

void Navigator::insertPlugins()
{
  PluginTraverser t( this, mContentsTree );
  DocMetaInfo::self()->traverseEntries( &t );
}

void Navigator::insertParentAppDocs( const QString &name, NavigatorItem *topItem )
{
  qCDebug(KHC_LOG) << "Requested plugin documents for ID " << name;

  KServiceGroup::Ptr grp = KServiceGroup::childGroup( name );
  if ( !grp )
    return;

  KServiceGroup::List entries = grp->entries();
  KServiceGroup::List::ConstIterator it = entries.constBegin();
  KServiceGroup::List::ConstIterator end = entries.constEnd();
  for ( ; it != end; ++it ) {
    QString desktopFile = ( *it )->entryPath();
    if ( QDir::isRelativePath( desktopFile ) )
        desktopFile = QStandardPaths::locate(QStandardPaths::ApplicationsLocation, desktopFile );
    createItemFromDesktopFile( topItem, desktopFile );
  }
}

void Navigator::insertKCMDocs( const QString &name, NavigatorItem *topItem, const QString &type )
{
  qCDebug(KHC_LOG) << "Requested KCM documents for ID" << name;
  QString systemsettingskontrolconstraint = QStringLiteral("[X-KDE-System-Settings-Parent-Category] != ''");
  QString konquerorcontrolconstraint = QStringLiteral("[X-KDE-PluginKeyword] == 'khtml_general'\
                                     or [X-KDE-PluginKeyword] == 'performance'\
                                     or [X-KDE-PluginKeyword] == 'bookmarks'");
  QString filemanagercontrolconstraint = QStringLiteral("[X-KDE-PluginKeyword] == 'behavior'\
                                       or [X-KDE-PluginKeyword] == 'dolphinviewmodes'\
                                       or [X-KDE-PluginKeyword] == 'dolphinnavigation'\
                                       or [X-KDE-PluginKeyword] == 'dolphinservices'\
                                       or [X-KDE-PluginKeyword] == 'dolphingeneral'\
                                       or [X-KDE-PluginKeyword] == 'trash'");
  QString browsercontrolconstraint = QStringLiteral("[X-KDE-PluginKeyword] == 'khtml_behavior'\
                                   or [X-KDE-PluginKeyword] == 'proxy'\
                                   or [X-KDE-PluginKeyword] == 'khtml_appearance'\
                                   or [X-KDE-PluginKeyword] == 'khtml_filter'\
                                   or [X-KDE-PluginKeyword] == 'cache'\
                                   or [X-KDE-PluginKeyword] == 'cookie'\
                                   or [X-KDE-PluginKeyword] == 'useragent'\
                                   or [X-KDE-PluginKeyword] == 'khtml_java_js'\
                                   or [X-KDE-PluginKeyword] == 'khtml_plugins'");
/* missing in browsercontrolconstraint
History                 no X-KDE-PluginKeyword in kcmhistory.desktop
*/
  QString othercontrolconstraint = QStringLiteral("[X-KDE-PluginKeyword] == 'cgi'");

  KService::List list;

  if ( type == QLatin1String("kcontrol") ) {
    list = KServiceTypeTrader::self()->query( QStringLiteral("KCModule"), systemsettingskontrolconstraint );
  } else if ( type == QLatin1String("konquerorcontrol") ) {
    list = KServiceTypeTrader::self()->query( QStringLiteral("KCModule"), konquerorcontrolconstraint );
  } else if ( type == QLatin1String("browsercontrol") ) {
    list = KServiceTypeTrader::self()->query( QStringLiteral("KCModule"), browsercontrolconstraint );
  } else if ( type == QLatin1String("filemanagercontrol") ) {
    list = KServiceTypeTrader::self()->query( QStringLiteral("KCModule"), filemanagercontrolconstraint );
  } else if ( type == QLatin1String("othercontrol") ) {
    list = KServiceTypeTrader::self()->query( QStringLiteral("KCModule"), othercontrolconstraint );
  } else if ( type == QLatin1String("kinfocenter") ) {
    list = KServiceTypeTrader::self()->query( QStringLiteral("KCModule"), QStringLiteral("[X-KDE-ParentApp] == 'kinfocenter'") );
  }

  bool no_children_present = true;

  for ( KService::List::const_iterator it = list.constBegin(); it != list.constEnd(); ++it )
  {
    KService::Ptr s(*it);
    const QString desktopFile = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QLatin1String("kservices5/") + s->entryPath() );
    createItemFromDesktopFile( topItem, desktopFile );
    no_children_present = false;
    }
    topItem->sortChildren( 0, Qt::AscendingOrder /* ascending */ );
    topItem->setHidden(no_children_present);
}

void Navigator::insertIOSlaveDocs( const QString &name, NavigatorItem *topItem )
{
  qCDebug(KHC_LOG) << "Requested IOSlave documents for ID" << name;

  QStringList list = KProtocolInfo::protocols();
  list.sort();

  NavigatorItem *prevItem = nullptr;
  for ( QStringList::ConstIterator it = list.constBegin(); it != list.constEnd(); ++it )
  {
    QString docPath = KProtocolInfo::docPath(*it);
    if ( !docPath.isNull() )
    {
      // First parameter is ignored if second is an absolute path
      QUrl url(QStringLiteral("help:/") + docPath);
      QString icon = KProtocolInfo::icon(*it);
      if ( icon.isEmpty() ) icon = QStringLiteral("text-plain");
      DocEntry *entry = new DocEntry( *it, url.url(), icon );
      NavigatorItem *item = new NavigatorAppItem( entry, topItem, prevItem );
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
      const QUrl url(QStringLiteral("help:/") + docPath);
      QString icon = desktopFile.readIcon();
      if ( icon.isEmpty() ) icon = QStringLiteral("text-plain");
      DocEntry *entry = new DocEntry( desktopFile.readName(), url.url(), icon );
      NavigatorItem *item = new NavigatorAppItem( entry, topItem );
      item->setAutoDeleteDocEntry( true );
    }
}

void Navigator::insertInfoDocs( NavigatorItem *topItem )
{
  InfoTree *infoTree = new InfoTree( this );
  infoTree->build( topItem );
}

void Navigator::insertScrollKeeperDocs( NavigatorItem *topItem )
{
  ScrollKeeperTreeBuilder *builder = new ScrollKeeperTreeBuilder( this );
  builder->buildOrHide( topItem );
}

void Navigator::selectItem( const QUrl &url )
{
  qCDebug(KHC_LOG) << "Navigator::selectItem(): " << url.url();

  if ( url.url() == QLatin1String("khelpcenter:home") ) {
    clearSelection();
    return;
  }

  // help:/foo&anchor=bar gets redirected to help:/foo#bar
  // Make sure that we match both the original URL as well as
  // its counterpart.
  QUrl alternativeURL = url;
  QUrl contentsItemURL = url;
  if (url.hasFragment())
  {
     alternativeURL.setQuery(QStringLiteral("anchor=")+url.fragment());
     alternativeURL.setFragment(QString());
     contentsItemURL.setFragment(QString());
  }

  // If the navigator already has the given URL selected, do nothing.
  NavigatorItem *item;
  item = static_cast<NavigatorItem *>( mContentsTree->currentItem() );
  if ( item && mSelected ) {
    const QUrl currentURL ( item->entry()->url() );
    if ( (currentURL == url) || (currentURL == alternativeURL) ) {
      qCDebug(KHC_LOG) << "URL already shown.";
      return;
    }
  }

  // First, populate the NavigatorAppGroupItems if we don't want the home page
  if ( url != homeURL() ) {
    QTreeWidgetItemIterator it1( mContentsTree );
    while( (*it1) ) 
    {
      NavigatorAppGroupItem *appItem = dynamic_cast<NavigatorAppGroupItem *>( (*it1) );
      if ( appItem ) appItem->populate( true );
      ++it1;
    }
  }
  
  NavigatorItem *contentsItem = nullptr;
  QTreeWidgetItemIterator it( mContentsTree );
  while ( (*it) ) {
    NavigatorItem *item = static_cast<NavigatorItem *>( (*it) );
    const QUrl itemUrl( item->entry()->url() );
    if ( (itemUrl == url) || (itemUrl == alternativeURL) ) {
      // If the current item was not selected and remained unchanged it
      // needs to be explicitly selected
      mContentsTree->setCurrentItem(item);
      item->setExpanded( true );
      break;
    }
    if ( (contentsItem == nullptr) && (itemUrl == contentsItemURL) ) {
      contentsItem = item;
    }
    ++it;
  }
  if ( !(*it) ) {
    // if search with fragment didn't find anything, but item without fragment was found, use it
    if ( contentsItem != nullptr ) {
      mContentsTree->setCurrentItem(contentsItem);
      contentsItem->setExpanded( true );
      mSelected = true;
    } else {
      clearSelection();
    }
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

  qCDebug(KHC_LOG) << item->entry()->name();

  item->setExpanded( !item->isExpanded() );

  const QUrl url ( item->entry()->url() );
  
  if ( url.scheme() == QLatin1String("khelpcenter") ) {
      mView->closeUrl();
      History::self().updateCurrentEntry( mView );
      History::self().createEntry();
      showOverview( item, url );
  } else {
   
    Q_EMIT itemSelected( url.url() );
  }
}

void Navigator::slotItemExpanded( QTreeWidgetItem *item )
{
  NavigatorItem *ni = static_cast<NavigatorItem *>( item );
  ni->itemExpanded( true );
}

void Navigator::slotItemCollapsed( QTreeWidgetItem *item )
{
  NavigatorItem *ni = static_cast<NavigatorItem *>( item );
  ni->itemExpanded( false );
}

void Navigator::openInternalUrl( const QUrl &url )
{
  if ( url.url() == QLatin1String("khelpcenter:home") ) {
    clearSelection();
    showOverview( nullptr, url );
    return;
  }

  selectItem( url );
  if ( !mSelected ) return;

  NavigatorItem *item =
    static_cast<NavigatorItem *>( mContentsTree->currentItem() );

  if ( item ) showOverview( item, url );
}

void Navigator::showOverview( NavigatorItem *item, const QUrl &url )
{
  mView->beginInternal( url );

  QString title,name,content;
  uint childCount;

  if ( item ) {
    title = item->entry()->name();
    name = item->entry()->name();

    const QString info = item->entry()->info();
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

    content += createChildrenList( child, 0 );
  }
  else
    content += QLatin1String("<p></p>");

  mView->write( mView->grantleeFormatter()->formatOverview( title, name, content ) );

  mView->end();
}

QString Navigator::createChildrenList( QTreeWidgetItem *child, int level )
{
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

    if ( childItem->childCount() > 0 && level < 1 ) {
      t += createChildrenList( childItem, level + 1 );
    }

    t += QLatin1String("</li>\n");

  }

  t += QLatin1String("</ul>\n");

  return t;
}

void Navigator::slotSearch()
{
  
  qCDebug(KHC_LOG) << "Navigator::slotSearch()";

  if ( mIndexingProc ) return;

  if ( mSearchEngine->isRunning() ) return;

  const QString words = mSearchEdit->text();
  const QString method = mSearchWidget->method();
  const int pages = mSearchWidget->pages();
  const QStringList scope = mSearchWidget->scope();

  qCDebug(KHC_LOG) << "Navigator::slotSearch() words: " << words;
  qCDebug(KHC_LOG) << "Navigator::slotSearch() scope: " << scope;

  if ( words.isEmpty() || scope.isEmpty() ) return;

  mTabWidget->setCurrentIndex( mTabWidget->indexOf( mSearchWidget ) );

  // disable search edit during searches
  mSearchEdit->setEnabled(false);
  QApplication::setOverrideCursor(Qt::WaitCursor);

  if ( !mSearchEngine->search( words, method, pages, scope ) ) {
    slotSearchFinished();
    KMessageBox::sorry( this, i18n("Unable to run search program.") );
  }
  
}

void Navigator::slotShowSearchResult( const QString &url )
{
  QString u = url;
  u.replace( QStringLiteral("%k"), mSearchEdit->text() );

  Q_EMIT itemSelected( u );
}

void Navigator::slotSearchFinished()
{
  mSearchEdit->setEnabled(true);
  mSearchEdit->setFocus();
  QApplication::restoreOverrideCursor();

  qCDebug(KHC_LOG) << "Search finished.";
}

void Navigator::checkSearchEdit()
{
  mSearchEdit->setEnabled( mSearchWidget->scopeCount() > 0 && !mIndexingProc );
}


void Navigator::hideSearch()
{
  mSearchEdit->hide();
  mTabWidget->removeTab( mTabWidget->indexOf( mSearchWidget ) );
}

void Navigator::slotSelectGlossEntry( const QString &id )
{
  mGlossaryTree->slotSelectGlossEntry( id );
}

QUrl Navigator::homeURL()
{
  if ( !mHomeUrl.isEmpty() ) return mHomeUrl;

  KSharedConfig::Ptr cfg = KSharedConfig::openConfig();
  // We have to reparse the configuration here in order to get a
  // language-specific StartUrl, e.g. "StartUrl[de]".
  cfg->reparseConfiguration();
  mHomeUrl = QUrl(cfg->group("General").readPathEntry( "StartUrl", QStringLiteral("khelpcenter:home") ));
  return mHomeUrl;
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

void Navigator::slotDelayedIndexingStart()
{
  mIndexingQueue.clear();

  const DocEntry::List &entries = DocMetaInfo::self()->docEntries();
  for ( DocEntry *entry : entries ) {
    if ( mSearchEngine->needsIndex( entry ) ) {
      mIndexingQueue.append( entry );
    }
  }

  if ( mIndexingQueue.isEmpty() ) {
    return;
  }

  Q_EMIT setStatusBarText( i18n( "Updating search index..." ) );

  mIndexingTimer.start();

  slotDoIndexWork();
}

void Navigator::slotDoIndexWork()
{
  if ( mIndexingQueue.isEmpty() ) {
    mIndexingTimer.stop();
    Q_EMIT setStatusBarText( i18n( "Updating search index... done." ) );
    mIndexingBar->hide();
    mSearchWidget->searchIndexUpdated();
    return;
  }

  const DocEntry *entry = mIndexingQueue.takeFirst();

  QString error;
  SearchHandler *handler = mSearchEngine->handler( entry->documentType() );
  if ( !handler ) {
    return slotDoIndexWork();
  }
  if ( !handler->checkPaths( &error ) ) {
    qCWarning(KHC_LOG) << "Indexing path error for" << entry->name() << ":" << error;
    return slotDoIndexWork();
  }
  QString indexer = handler->indexCommand( entry->identifier() );
  if ( indexer.isEmpty() ) {
    qCWarning(KHC_LOG) << "Empty indexer for" << entry->identifier() << entry->documentType();
    return slotDoIndexWork();
  }

  const QString indexDir = Prefs::indexDirectory();

  indexer.replace( QLatin1String( "%i" ), entry->identifier() );
  indexer.replace( QLatin1String( "%d" ), indexDir );
  indexer.replace( QLatin1String( "%p" ), entry->url() );
  qCDebug(KHC_LOG) << "Indexer:" << indexer;

  if ( !QDir().mkpath( indexDir ) ) {
    qCWarning(KHC_LOG) << "cannot create the directory:" << indexDir;
    return slotDoIndexWork();
  }

  mIndexingProc = new KProcess;

  *mIndexingProc << KShell::splitArgs( indexer );

  connect(mIndexingProc, QOverload<int, QProcess::ExitStatus>::of(&KProcess::finished), this, &Navigator::slotProcessExited);

  mIndexingProc->start();

  if ( !mIndexingProc->waitForStarted() )  {
    qCWarning(KHC_LOG) << "Unable to start command" << indexer;
    delete mIndexingProc;
    mIndexingProc = nullptr;
    return slotDoIndexWork();
  }
}

void Navigator::slotProcessExited( int exitCode, QProcess::ExitStatus exitStatus )
{
  if ( exitStatus != QProcess::NormalExit ) {
    qCWarning(KHC_LOG) << "Process failed";
    qCWarning(KHC_LOG) << "stdout output:" << mIndexingProc->readAllStandardOutput();
    qCWarning(KHC_LOG) << "stderr output:" << mIndexingProc->readAllStandardError();
  } else if ( exitCode != 0 ) {
    qCWarning(KHC_LOG) << "running" << mIndexingProc->program() << "failed with exitCode" << exitCode;
    qCWarning(KHC_LOG) << "stdout output:" << mIndexingProc->readAllStandardOutput();
    qCWarning(KHC_LOG) << "stderr output:" << mIndexingProc->readAllStandardError();
  }
  delete mIndexingProc;
  mIndexingProc = nullptr;

  slotDoIndexWork();
}

void Navigator::slotShowIndexingProgressBar()
{
  if ( !mIndexingProc ) {
    return;
  }

  mIndexingBar->setRange( 0, 0 );
  mIndexingBar->show();
}


// vim:ts=2:sw=2:et
