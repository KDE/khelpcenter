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
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <qdir.h>
#include <qfile.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qlabel.h>
#include <qheader.h>
#include <qdom.h>
#include <qtextstream.h>
#include <qregexp.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>

#include <kaction.h>
#include <kapplication.h>
#include <ksimpleconfig.h>
#include <kstandarddirs.h>
#include <kglobal.h>
#include <klocale.h>
#include <kdebug.h>
#include <klistview.h>
#include <kmessagebox.h>
#include <kiconloader.h>
#include <kprocio.h>
#include <kcharsets.h>
#include <kdialog.h>
#include <kdesktopfile.h>
#include <kprotocolinfo.h>
#include <kservicegroup.h>

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

#include "navigator.h"

using namespace KHC;

Navigator::Navigator( View *view, QWidget *parent, const char *name )
   : QWidget( parent, name ),
     mView( view )
{
    KConfig *config = kapp->config();
    config->setGroup("General");
    mShowMissingDocs = config->readBoolEntry("ShowMissingDocs",false);

    mSearchEngine = new SearchEngine( view );
    connect( mSearchEngine, SIGNAL( searchFinished() ),
             SLOT( slotSearchFinished() ) );

    DocMetaInfo::self()->scanMetaInfo();

    QBoxLayout *topLayout = new QVBoxLayout( this );

    mSearchFrame = new QFrame( this );
    topLayout->addWidget( mSearchFrame );

    QBoxLayout *searchLayout = new QHBoxLayout( mSearchFrame );
    searchLayout->setMargin( KDialog::spacingHint() );

    mSearchEdit = new QLineEdit( mSearchFrame );
    searchLayout->addWidget( mSearchEdit );
    connect( mSearchEdit, SIGNAL( returnPressed() ), SLOT( slotSearch() ) );
    connect( mSearchEdit, SIGNAL( textChanged( const QString & ) ),
             SLOT( slotSearchTextChanged( const QString & ) ) );

    mSearchButton = new QPushButton( i18n("Search"), mSearchFrame );
    searchLayout->addWidget( mSearchButton );
    connect( mSearchButton, SIGNAL( clicked() ), SLOT( slotSearch() ) );

    mTabWidget = new QTabWidget( this );
    topLayout->addWidget( mTabWidget );
    connect( mTabWidget, SIGNAL( currentChanged( QWidget * ) ),
             SLOT( slotTabChanged( QWidget * ) ) );

    setupContentsTab();
    setupSearchTab();
    setupGlossaryTab();

    insertPlugins();

    if ( DocMetaInfo::self()->searchEntries().isEmpty() ) {
      hideSearch();
    } else {
      mSearchWidget->updateScopeList();
      slotSearchTextChanged( mSearchEdit->text() );
      
      mSearchWidget->readConfig( KGlobal::config() );
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
    mContentsTree = new KListView( mTabWidget );
    mContentsTree->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    mContentsTree->addColumn(QString::null);
    mContentsTree->setAllColumnsShowFocus(true);
    mContentsTree->header()->hide();
    mContentsTree->setRootIsDecorated(false);
    mContentsTree->setSorting(-1, false);

    connect(mContentsTree, SIGNAL(executed(QListViewItem*)),
            SLOT(slotItemSelected(QListViewItem*)));
    connect(mContentsTree, SIGNAL(returnPressed(QListViewItem*)),
           SLOT(slotItemSelected(QListViewItem*)));
    mTabWidget->addTab(mContentsTree, i18n("&Contents"));
}

void Navigator::setupSearchTab()
{
    mSearchWidget = new SearchWidget( mTabWidget );
    connect( mSearchWidget, SIGNAL( searchResult( const QString & ) ),
             SLOT( slotShowSearchResult( const QString & ) ) );
    connect( mSearchWidget, SIGNAL( enableSearch( bool ) ),
             mSearchButton, SLOT( setEnabled( bool ) ) ); 

    mTabWidget->addTab( mSearchWidget, i18n("Search"));
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

#if 0
  kdDebug( 1400 ) << "<docmetainfo>" << endl;
  DocEntry::List entries = DocMetaInfo::self()->docEntries();
  DocEntry::List::ConstIterator it;
  for( it = entries.begin(); it != entries.end(); ++it ) {
    (*it)->dump();
  }
  kdDebug( 1400 ) << "</docmetainfo>" << endl;
#endif
}

void Navigator::insertParentAppDocs( const QString &name, NavigatorItem *topItem )
{
  kdDebug(1400) << "Requested plugin documents for ID " << name << endl;
 
  KServiceGroup::Ptr grp = KServiceGroup::childGroup( name );
  if ( !grp )
    return;

  KServiceGroup::List entries = grp->entries();
  KServiceGroup::List::ConstIterator it = entries.begin();
  KServiceGroup::List::ConstIterator end = entries.end();
  for ( ; it != end; ++it ) {
    QString desktopFile = ( *it )->entryPath();
    if ( QDir::isRelativePath( desktopFile ) )
        desktopFile = locate( "apps", desktopFile );
    createItemFromDesktopFile( topItem, desktopFile );
  }
}

void Navigator::insertIOSlaveDocs( const QString &name, NavigatorItem *topItem )
{
  kdDebug(1400) << "Requested IOSlave documents for ID " << name << endl;

#if KDE_IS_VERSION( 3, 1, 90 ) 
  QStringList list = KProtocolInfo::protocols();

  for ( QStringList::ConstIterator it = list.begin(); it != list.end(); ++it ) 
  {
    QString docPath = KProtocolInfo::docPath(*it);
    if ( !docPath.isNull() ) 
    {
      // First parameter is ignored if second is an absolute path
      KURL url(KURL("help:/"), docPath);
      QString icon = KProtocolInfo::icon(*it);
      if ( icon.isEmpty() ) icon = "document2";
      DocEntry *entry = new DocEntry( *it, url.url(), icon );
      NavigatorItem *item = new NavigatorItem( entry, topItem );
      item->setAutoDeleteDocEntry( true );
    }
  }
#else
  Q_UNUSED( topItem );
#endif
}

void Navigator::insertAppletDocs( NavigatorItem *topItem )
{
  QDir appletDir( locate( "data", QString::fromLatin1( "kicker/applets/" ) ) );
  appletDir.setNameFilter( QString::fromLatin1( "*.desktop" ) );

  QStringList files = appletDir.entryList( QDir::Files | QDir::Readable );
  QStringList::ConstIterator it = files.begin();
  QStringList::ConstIterator end = files.end();
  for ( ; it != end; ++it )
    createItemFromDesktopFile( topItem, appletDir.absPath() + "/" + *it );
}

void Navigator::createItemFromDesktopFile( NavigatorItem *topItem,
                                           const QString &file )
{
    KDesktopFile desktopFile( file );
    QString docPath = desktopFile.readDocPath();
    if ( !docPath.isNull() ) {
      // First parameter is ignored if second is an absolute path
      KURL url(KURL("help:/"), docPath);
      QString icon = desktopFile.readIcon();
      if ( icon.isEmpty() ) icon = "document2";
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

void Navigator::selectItem( const KURL &url )
{
  NavigatorItem *item;
  item = static_cast<NavigatorItem *>( mContentsTree->currentItem() );
  KURL currentURL = item->entry()->url();
  if ( currentURL == url ) return;

  // First, populate the NavigatorAppItems if we don't want the home page
  if ( url != homeURL() ) {
    for ( QListViewItem *item = mContentsTree->firstChild(); item != 0; item = item->nextSibling() ) {
      NavigatorAppItem *appItem = dynamic_cast<NavigatorAppItem *>( item );
      if ( appItem != 0 )
        appItem->populate( true /* recursive */ );
    }
  }

  kdDebug() << "Navigator::selectItem(): " << url.url() << endl;

  QListViewItemIterator it( mContentsTree );
  while ( it.current() != 0 ) {
    NavigatorItem *item = static_cast<NavigatorItem *>( it.current() );
    KURL itemUrl( item->entry()->url() );
    if ( itemUrl == url ) {
      mContentsTree->setCurrentItem( item );
      mContentsTree->ensureItemVisible( item );
      slotItemSelected( item );
      break;
    }
    ++it;
  }
}

void Navigator::clearSelection()
{
  mContentsTree->clearSelection();
}

void Navigator::slotItemSelected( QListViewItem* currentItem )
{
  if ( !currentItem ) return;
  NavigatorItem *item = static_cast<NavigatorItem*>( currentItem );

  kdDebug(1400) << "Navigator::slotItemSelected(): " << item->entry()->name()
                << endl;

  if ( item->childCount() > 0 || item->isExpandable() )
    item->setOpen( !item->isOpen() );

#if 0
  if (pluginItems.find(item) > -1)
  {
    QString url = item->url();
    kdDebug( 1400 ) << "--ITEM: " << url << endl;
  
    if (!url.isEmpty()) {
      if ( url.left(1) == "/" ) {
        url = "file:" + url;
      } else {
        int colonPos = url.find(':');
        int slashPos = url.find('/');
        if ( colonPos < 0 || ( colonPos > url.find('/') && slashPos > 0 ) ) {
          url = "file:" + View::langLookup(url);
        }
      }
      
      kdDebug( 1400 ) << "--URL: " << url << endl;
      
      emit itemSelected(url);
    }
    return;
  }
#endif

  KURL url = item->entry()->url();

  if ( url.protocol() == "khelpcenter" ) {
    mView->beginInternal( url );

    QString t = formatter()->header( item->entry()->name() );
    
    t += "<h1>" + item->entry()->name() + "</h1>\n";
    
    QString info = item->entry()->info();
    if ( !info.isEmpty() ) t += "<p>" + info + "</p>\n";
    
    if ( item->childCount() > 0 ) {
      t += "<ul>\n";
      QListViewItem *child = item->firstChild();
      while ( child ) {
        NavigatorItem *childItem = static_cast<NavigatorItem *>( child );
        
        DocEntry *e = childItem->entry();
        
        t += "<li><a href=\"" + e->url() + "\">";
        if ( e->isDirectory() ) t += "<b>";
        t += e->name();
        if ( e->isDirectory() ) t += "</b>";
        t += "</a>";
        
        if ( !e->info().isEmpty() ) {
          t += "<br>" + e->info();
        }
        
        t += "</li>\n";
        
        child = child->nextSibling();
      }
    }
    
    t += formatter()->footer();
    
//    kdDebug() << "HTML--" << endl << t << "--HTML" << endl;
    
    mView->write( t );
    
    mView->end();
  } else {
    if ( url.protocol() == "help" ) {
      kdDebug( 1400 ) << "slotItemSelected(): Got help URL " << url.url()
                      << endl;
      if ( !item->toc() ) {
        TOC *tocTree = item->createTOC();
        kdDebug( 1400 ) << "slotItemSelected(): Trying to build TOC for "
                        << item->entry()->name() << endl;
        tocTree->setApplication( url.directory() );
        QString doc = View::langLookup( url.path() );
        // Enforce the original .docbook version, in case langLookup returns a
        // cached version
        if ( !doc.isNull() ) {
          int pos = doc.find( ".html" );
          if ( pos >= 0 ) {
            doc.replace( pos, 5, ".docbook" );
          }
          kdDebug( 1400 ) << "slotItemSelected(): doc = " << doc << endl;

          tocTree->build( doc );
        }
      }
    }
    emit itemSelected( url.url() );
  }
}

void Navigator::slotSearch()
{
  kdDebug(1400) << "Navigator::slotSearch()" << endl;

  if ( !checkSearchIndex() ) return;

  if ( mSearchEngine->isRunning() ) return;

  QString words = mSearchEdit->text();
  QString method = mSearchWidget->method();
  int pages = mSearchWidget->pages();
  QString scope = mSearchWidget->scope();

  kdDebug(1400) << "Navigator::slotSearch() words: " << words << endl;
  kdDebug(1400) << "Navigator::slotSearch() scope: " << scope << endl;

  if ( words.isEmpty() || scope.isEmpty() ) return;

  // disable search Button during searches
  mSearchButton->setEnabled(false);
  QApplication::setOverrideCursor(waitCursor);

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

  kdDebug( 1400 ) << "Search finished." << endl;
}

void Navigator::slotSearchTextChanged( const QString &text )
{
//  kdDebug( 1400 ) << "Navigator::slotSearchTextCanged() '" << text << "'"
//            << endl;

  mSearchButton->setEnabled( !text.isEmpty() );
}

void Navigator::hideSearch()
{
  mSearchFrame->hide();
  mTabWidget->removePage( mSearchWidget );
}

bool Navigator::checkSearchIndex()
{
  KConfig *cfg = KGlobal::config();
  cfg->setGroup( "Search" );
  if ( cfg->readBoolEntry( "IndexExists", false ) ) return true;

  KCMHelpCenter *indexDialog = mSearchWidget->indexDialog();
  if ( indexDialog && indexDialog->isShown() ) return true;

  QString text = i18n( "A search index does not yet exist. Do you want "
                       "to create the index now?" );

  int result = KMessageBox::questionYesNo( this, text, QString::null,
                                           KStdGuiItem::yes(),
                                           KStdGuiItem::no(),
                                           "indexcreation" );
  if ( result == KMessageBox::Yes ) {
    mSearchWidget->slotIndex();
    return false;
  }
  
  return true;
}

void Navigator::slotTabChanged( QWidget *wid )
{
  if ( wid == mSearchWidget ) checkSearchIndex();
}

void Navigator::showPreferencesDialog()
{
  mSearchWidget->slotIndex();
}

void Navigator::slotSelectGlossEntry( const QString &id )
{
  mGlossaryTree->slotSelectGlossEntry( id );
}

KURL Navigator::homeURL()
{
  if ( !mHomeUrl.isEmpty() ) return mHomeUrl;

  KConfig *cfg = KGlobal::config();
  // We have to reparse the configuration here in order to get a
  // language-specific StartUrl, e.g. "StartUrl[de]".
  cfg->reparseConfiguration();
  cfg->setGroup( "General" );
  mHomeUrl = cfg->readPathEntry( "StartUrl",
                               "help:/khelpcenter/index.html?anchor=welcome" );
  return mHomeUrl;
}

#include "navigator.moc"

// vim:ts=2:sw=2:et
