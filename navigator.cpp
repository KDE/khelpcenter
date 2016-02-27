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
#include <QTextStream>
#include <QPushButton>
#include <QFrame>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTreeWidgetItemIterator>
#include <qapplication.h>
#include <QProgressBar>
#include <QTabWidget>
#include <QTreeWidget>

#include <KLineEdit>
#include <KMessageBox>
#include <KDesktopFile>
#include <KProtocolInfo>
#include <KServiceGroup>
#include <KServiceTypeTrader>
#include <KProcess>
#include <KShell>

#include <QStandardPaths>

#include "navigatoritem.h"
#include "navigatorappitem.h"
#include "searchwidget.h"
#include "searchengine.h"
#include "searchhandler.h"
#include "docmetainfo.h"
#include "docentrytraverser.h"
#include "glossary.h"
#include "toc.h"
#include "view.h"
#include "infotree.h"
#include "plugintraverser.h"
#include "scrollkeepertreebuilder.h"
#include "formatter.h"
#include "history.h"
#include "khc_debug.h"
#include "prefs.h"

using namespace KHC;

Navigator::Navigator( View *view, QWidget *parent )
   : QWidget( parent ),
     mView( view ), mSelected( false ), mIndexingProc( 0 )
{
    KConfigGroup config(KSharedConfig::openConfig(), "General");
    mShowMissingDocs = config.readEntry("ShowMissingDocs", false);

    mSearchEngine = new SearchEngine( view );
    connect(mSearchEngine, &SearchEngine::searchFinished, this, &Navigator::slotSearchFinished);

    DocMetaInfo::self()->scanMetaInfo();

    QBoxLayout *topLayout = new QVBoxLayout( this );

    mSearchFrame = new QFrame( this );
    topLayout->addWidget( mSearchFrame );

    QBoxLayout *searchLayout = new QHBoxLayout( mSearchFrame );
    searchLayout->setMargin( 6 );

    mSearchEdit = new KLineEdit( mSearchFrame );
    mSearchEdit->setClearButtonShown(true);
    searchLayout->addWidget( mSearchEdit );
    connect(mSearchEdit, &KLineEdit::returnPressed, this, &Navigator::slotSearch);
    connect(mSearchEdit, &KLineEdit::textChanged, this, &Navigator::checkSearchButton);

    mSearchButton = new QPushButton( i18n("&Search"), mSearchFrame );
    searchLayout->addWidget( mSearchButton );
    connect(mSearchButton, &QPushButton::clicked, this, &Navigator::slotSearch);

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

    connect(mContentsTree, &QTreeWidget::itemActivated, this, &Navigator::slotItemSelected);
    
    mTabWidget->addTab(mContentsTree, i18n("&Contents"));
}

void Navigator::setupSearchTab()
{
  
    mSearchWidget = new SearchWidget( mSearchEngine, mTabWidget );
    connect(mSearchWidget, &SearchWidget::searchResult, this, &Navigator::slotShowSearchResult);
    connect(mSearchWidget, &SearchWidget::scopeCountChanged, this, &Navigator::checkSearchButton);

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
  khcDebug() << "Requested plugin documents for ID " << name;

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
  khcDebug() << "Requested KCM documents for ID" << name;
  QString systemsettingskontrolconstraint = "[X-KDE-System-Settings-Parent-Category] != ''";
  QString konquerorcontrolconstraint = "[X-KDE-PluginKeyword] == 'khtml_general'\
                                     or [X-KDE-PluginKeyword] == 'performance'\
                                     or [X-KDE-PluginKeyword] == 'bookmarks'";
  QString filemanagercontrolconstraint = "[X-KDE-PluginKeyword] == 'behavior'\
                                       or [X-KDE-PluginKeyword] == 'dolphinviewmodes'\
                                       or [X-KDE-PluginKeyword] == 'dolphinnavigation'\
                                       or [X-KDE-PluginKeyword] == 'dolphinservices'\
                                       or [X-KDE-PluginKeyword] == 'dolphingeneral'\
                                       or [X-KDE-PluginKeyword] == 'trash'";
  QString browsercontrolconstraint = "[X-KDE-PluginKeyword] == 'khtml_behavior'\
                                   or [X-KDE-PluginKeyword] == 'proxy'\
                                   or [X-KDE-PluginKeyword] == 'khtml_appearance'\
                                   or [X-KDE-PluginKeyword] == 'khtml_filter'\
                                   or [X-KDE-PluginKeyword] == 'cache'\
                                   or [X-KDE-PluginKeyword] == 'cookie'\
                                   or [X-KDE-PluginKeyword] == 'useragent'\
                                   or [X-KDE-PluginKeyword] == 'khtml_java_js'\
                                   or [X-KDE-PluginKeyword] == 'khtml_plugins'";
/* missing in browsercontrolconstraint
History                 no X-KDE-PluginKeyword in kcmhistory.desktop
*/
  QString othercontrolconstraint = "[X-KDE-PluginKeyword] == 'cgi'";

  KService::List list;

  if ( type == QString("kcontrol") ) {
    list = KServiceTypeTrader::self()->query( "KCModule", systemsettingskontrolconstraint );
  } else if ( type == QString("konquerorcontrol") ) {
    list = KServiceTypeTrader::self()->query( "KCModule", konquerorcontrolconstraint );
  } else if ( type == QString("browsercontrol") ) {
    list = KServiceTypeTrader::self()->query( "KCModule", browsercontrolconstraint );
  } else if ( type == QString("filemanagercontrol") ) {
    list = KServiceTypeTrader::self()->query( "KCModule", filemanagercontrolconstraint );
  } else if ( type == QString("othercontrol") ) {
    list = KServiceTypeTrader::self()->query( "KCModule", othercontrolconstraint );
  } else if ( type == QString("kinfocenter") ) {
    list = KServiceTypeTrader::self()->query( "KCModule", "[X-KDE-ParentApp] == 'kinfocenter'" );
  }

  for ( KService::List::const_iterator it = list.constBegin(); it != list.constEnd(); ++it )
  {
    KService::Ptr s(*it);
    QString desktopFile = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QLatin1String("kservices5/") + s->entryPath() );
    createItemFromDesktopFile( topItem, desktopFile );
    }
    topItem->sortChildren( 0, Qt::AscendingOrder /* ascending */ );
}

void Navigator::insertIOSlaveDocs( const QString &name, NavigatorItem *topItem )
{
  khcDebug() << "Requested IOSlave documents for ID" << name;

  QStringList list = KProtocolInfo::protocols();
  list.sort();

  NavigatorItem *prevItem = 0;
  for ( QStringList::ConstIterator it = list.constBegin(); it != list.constEnd(); ++it )
  {
    QString docPath = KProtocolInfo::docPath(*it);
    if ( !docPath.isNull() )
    {
      // First parameter is ignored if second is an absolute path
      QUrl url(QStringLiteral("help:/") + docPath);
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
      QUrl url(QStringLiteral("help:/") + docPath);
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

void Navigator::selectItem( const QUrl &url )
{
  khcDebug() << "Navigator::selectItem(): " << url.url();

  if ( url.url() == "khelpcenter:home" ) {
    clearSelection();
    return;
  }

  // help:/foo&anchor=bar gets redirected to help:/foo#bar
  // Make sure that we match both the original URL as well as
  // its counterpart.
  QUrl alternativeURL = url;
  if (url.hasFragment())
  {
     alternativeURL.setQuery("anchor="+url.fragment());
     alternativeURL.setFragment(QString());
  }

  // If the navigator already has the given URL selected, do nothing.
  NavigatorItem *item;
  item = static_cast<NavigatorItem *>( mContentsTree->currentItem() );
  if ( item && mSelected ) {
    QUrl currentURL ( item->entry()->url() );
    if ( (currentURL == url) || (currentURL == alternativeURL) ) {
      khcDebug() << "URL already shown.";
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
    QUrl itemUrl( item->entry()->url() );
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

  khcDebug() << item->entry()->name() << endl;

  item->setExpanded( !item->isExpanded() );

  QUrl url ( item->entry()->url() );

  
  
  if ( url.scheme() == "khelpcenter" ) {
      mView->closeUrl();
      History::self().updateCurrentEntry( mView );
      History::self().createEntry();
      showOverview( item, url );
  } else {
   
    emit itemSelected( url.url() );
  }

  mLastUrl = url;
}

void Navigator::openInternalUrl( const QUrl &url )
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

void Navigator::showOverview( NavigatorItem *item, const QUrl &url )
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

    if ( childItem->childCount() > 0 && mDirLevel < 2 ) {
      t += createChildrenList( childItem );
    }

    t += QLatin1String("</li>\n");

  }

  t += QLatin1String("</ul>\n");

  --mDirLevel;

  return t;
}

void Navigator::slotSearch()
{
  
  khcDebug() << "Navigator::slotSearch()";

  if ( mIndexingProc ) return;

  if ( mSearchEngine->isRunning() ) return;

  QString words = mSearchEdit->text();
  QString method = mSearchWidget->method();
  int pages = mSearchWidget->pages();
  QString scope = mSearchWidget->scope();

  khcDebug() << "Navigator::slotSearch() words: " << words;
  khcDebug() << "Navigator::slotSearch() scope: " << scope;

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

  khcDebug() << "Search finished.";
}

void Navigator::checkSearchButton()
{
  mSearchButton->setEnabled( !mSearchEdit->text().isEmpty() &&
    mSearchWidget->scopeCount() > 0 && !mIndexingProc );
  mTabWidget->setCurrentIndex( mTabWidget->indexOf( mSearchWidget ) );
}


void Navigator::hideSearch()
{
  mSearchFrame->hide();
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
  mHomeUrl = cfg->group("General").readPathEntry( "StartUrl", QLatin1String("khelpcenter:home") );
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
  foreach ( DocEntry *entry, entries ) {
    if ( mSearchEngine->needsIndex( entry ) ) {
      mIndexingQueue.append( entry );
    }
  }

  if ( mIndexingQueue.isEmpty() ) {
    return;
  }

  emit setStatusBarText( i18n( "Updating search index..." ) );

  mIndexingTimer.start();

  slotDoIndexWork();
}

void Navigator::slotDoIndexWork()
{
  if ( mIndexingQueue.isEmpty() ) {
    mIndexingTimer.stop();
    emit setStatusBarText( i18n( "Updating search index... done." ) );
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
    khcWarning() << "Indexing path error for" << entry->name() << ":" << error;
    return slotDoIndexWork();
  }
  QString indexer = handler->indexCommand( entry->identifier() );
  if ( indexer.isEmpty() ) {
    khcWarning() << "Empty indexer for" << entry->identifier() << entry->documentType();
    return slotDoIndexWork();
  }

  const QString indexDir = Prefs::indexDirectory();

  indexer.replace( QLatin1String( "%i" ), entry->identifier() );
  indexer.replace( QLatin1String( "%d" ), indexDir );
  indexer.replace( QLatin1String( "%p" ), entry->url() );
  khcDebug() << "Indexer:" << indexer;

  if ( !QDir().mkpath( indexDir ) ) {
    khcWarning() << "cannot create the directory:" << indexDir;
    return slotDoIndexWork();
  }

  mIndexingProc = new KProcess;

  *mIndexingProc << KShell::splitArgs( indexer );

  connect( mIndexingProc, SIGNAL( finished( int, QProcess::ExitStatus) ),
           SLOT( slotProcessExited( int, QProcess::ExitStatus) ) );

  mIndexingProc->start();

  if ( !mIndexingProc->waitForStarted() )  {
    khcWarning() << "Unable to start command" << indexer;
    delete mIndexingProc;
    mIndexingProc = 0;
    return slotDoIndexWork();
  }
}

void Navigator::slotProcessExited( int exitCode, QProcess::ExitStatus exitStatus )
{
  if ( exitStatus != QProcess::NormalExit ) {
    khcWarning() << "Process failed";
    khcWarning() << "stdout output:" << mIndexingProc->readAllStandardOutput();
    khcWarning() << "stderr output:" << mIndexingProc->readAllStandardError();
  } else if ( exitCode != 0 ) {
    khcWarning() << "running" << mIndexingProc->program() << "failed with exitCode" << exitCode;
    khcWarning() << "stdout output:" << mIndexingProc->readAllStandardOutput();
    khcWarning() << "stderr output:" << mIndexingProc->readAllStandardError();
  }
  delete mIndexingProc;
  mIndexingProc = 0;

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
