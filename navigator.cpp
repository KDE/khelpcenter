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

using namespace KHC;

#include "navigator.h"
#include "navigator.moc"

Navigator::Navigator( View *view, QWidget *parent,
                                        const char *name )
   : QWidget( parent, name ),
     mView( view )
{
    KConfig *config = kapp->config();
    config->setGroup("ScrollKeeper");
    mScrollKeeperShowEmptyDirs = config->readBoolEntry("ShowEmptyDirs",false);
    
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
    }
}

Navigator::~Navigator()
{
  delete mSearchEngine;
}

SearchEngine *Navigator::searchEngine()
{
  return mSearchEngine;
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

class PluginTraverser : public DocEntryTraverser
{
  public:
    PluginTraverser( Navigator *navigator, QListView *listView  ) :
      mListView( listView ), mParentItem( 0 ), mCurrentItem( 0 ),
      mNavigator( navigator ) {}
    PluginTraverser( Navigator *navigator, NavigatorItem *listViewItem  ) :
      mListView( 0 ), mParentItem( listViewItem ), mCurrentItem( 0 ),
      mNavigator( navigator ) {}

    void process( DocEntry *entry )
    {
      if ( !mListView && !mParentItem ) {
        kdDebug( 1400 ) << "ERROR! Neither mListView nor mParentItem is set." << endl;
        return;
      }

      if ( !entry->docExists() & !mNavigator->showMissingDocs() ) return;

#if 0
      kdDebug() << "PluginTraverser::process(): " << entry->name()
                << " (weight: " << entry->weight() << " parent: "
                << ( mParentItem ? mParentItem->name() : "0" ) << ")" << endl;
#endif

      if (entry->khelpcenterSpecial() == "apps") {
        NavigatorAppItem *appItem;
        if ( mListView )
          appItem = new NavigatorAppItem( mListView, mCurrentItem );
        else
          appItem = new NavigatorAppItem( mParentItem, mCurrentItem );
        KConfig *cfg = kapp->config();
        cfg->setGroup( "General" );
        appItem->setRelpath( cfg->readEntry( "AppsRoot" ) );
        mCurrentItem = appItem;
      } else if ( entry->khelpcenterSpecial() == "scrollkeeper" ) {
        if ( mParentItem ) {
          mCurrentItem = mNavigator->insertScrollKeeperDocs( mParentItem,
                                                             mCurrentItem );
        }
        return;
      } else {
        if ( mListView )
          mCurrentItem = new NavigatorItem( mListView, mCurrentItem );
        else
          mCurrentItem = new NavigatorItem( mParentItem, mCurrentItem );

        if (entry->khelpcenterSpecial() == "applets" )
          mNavigator->insertAppletDocs( mCurrentItem );

        else if ( entry->khelpcenterSpecial() == "kinfocenter" ||
                  entry->khelpcenterSpecial() == "kcontrol" ||
                  entry->khelpcenterSpecial() == "konqueror" )
          mNavigator->insertParentAppDocs( entry->khelpcenterSpecial(),
                                           mCurrentItem );

        else if ( entry->khelpcenterSpecial() == "kioslave" )
          mNavigator->insertIOSlaveDocs( entry->khelpcenterSpecial(),
                                         mCurrentItem );

        else if ( entry->khelpcenterSpecial() == "info" )
          mNavigator->insertInfoDocs( mCurrentItem );
      }

      mCurrentItem->setName( entry->name() );
      mCurrentItem->setUrl( entry->docPath() );

      if ( !entry->docExists() ) {
        mCurrentItem->setIcon( "unknown" );
      } else {
        if ( entry->icon().isEmpty() ) {
          if ( entry->isDirectory() ) {
            mCurrentItem->setIcon( "contents2" );
          } else {
            mCurrentItem->setIcon( "document2" );
          }
        } else {
          mCurrentItem->setIcon( entry->icon() );
        }
      }
    }

    DocEntryTraverser *createChild( DocEntry * )
    {
      if ( mCurrentItem ) {
        return new PluginTraverser( mNavigator, mCurrentItem );
      }
      kdDebug( 1400 ) << "ERROR! mCurrentItem is not set." << endl;
      return 0;
    }

  private:
    QListView *mListView;
    NavigatorItem *mParentItem;
    NavigatorItem *mCurrentItem;
    
    Navigator *mNavigator;
};


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
      NavigatorItem *item = new NavigatorItem( topItem, *it );
      // First parameter is ignored if second is an absolute path
      KURL url(KURL("help:/"), docPath);
      item->setUrl( url.url() );
      QString icon = KProtocolInfo::icon(*it);
      item->setIcon( icon.isEmpty() ? "document2" : icon );
    }
  }
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

void Navigator::createItemFromDesktopFile( NavigatorItem *topItem, const QString &file )
{
    KDesktopFile desktopFile( file );
    QString docPath = desktopFile.readDocPath();
    if ( !docPath.isNull() ) {
      NavigatorItem *item = new NavigatorItem( topItem, desktopFile.readName() );
      // First parameter is ignored if second is an absolute path
      KURL url(KURL("help:/"), docPath);
      item->setUrl( url.url() );
      QString icon = desktopFile.readIcon();
      item->setIcon( icon.isNull() ? "document2" : icon );
    }
}

void Navigator::insertInfoDocs( NavigatorItem *topItem )
{
  InfoTree *infoTree = new InfoTree( this );
  connect( infoTree, SIGNAL( urlSelected( const KURL & ) ),
           mView, SLOT( openURL( const KURL & ) ) );
  infoTree->build( topItem );
}

NavigatorItem *Navigator::insertScrollKeeperDocs( NavigatorItem *topItem,
                                                  NavigatorItem *after )
{
    QString lang = KGlobal::locale()->language();

    kdDebug(1400) << "ScrollKeeper language: " << lang << endl;

    KProcIO proc;
    proc << "scrollkeeper-get-content-list";
    proc << lang;
    connect(&proc,SIGNAL(readReady(KProcIO *)),SLOT(getScrollKeeperContentsList(KProcIO *)));
    if (!proc.start(KProcess::Block)) {
      kdDebug(1400) << "Could not execute scrollkeeper-get-content-list" << endl;
      return 0;
    }

    if (!QFile::exists(mScrollKeeperContentsList)) {
      kdDebug(1400) << "Scrollkeeper contents file '" << mScrollKeeperContentsList
                    << "' does not exist." << endl;
      return 0;
    }

    QDomDocument doc("ScrollKeeperContentsList");
    QFile f(mScrollKeeperContentsList);
    if ( !f.open( IO_ReadOnly ) )
        return 0;
    if ( !doc.setContent( &f ) ) {
        f.close();
        return 0;
    }
    f.close();

    // Create top-level item
    scrollKeeperItems.append(topItem);

    QDomElement docElem = doc.documentElement();

    NavigatorItem *result = 0;

    QDomNode n = docElem.firstChild();
    while( !n.isNull() ) {
        QDomElement e = n.toElement();
        if( !e.isNull() ) {
            if (e.tagName() == "sect") {
              NavigatorItem *createdItem;
              insertScrollKeeperSection( topItem, after, e, createdItem );
              if ( createdItem ) result = createdItem;
            }
        }
        n = n.nextSibling();
    }

    return result;
}

void Navigator::getScrollKeeperContentsList(KProcIO *proc)
{
    QString filename;
    proc->readln(filename,true);

    mScrollKeeperContentsList = filename;
}

int Navigator::insertScrollKeeperSection( NavigatorItem *parentItem,
                                          NavigatorItem *after,
                                          QDomNode sectNode,
                                          NavigatorItem * &sectItem )
{
    sectItem = new NavigatorItem( parentItem, after, "", "contents2" );
    sectItem->setUrl("");
    scrollKeeperItems.append(sectItem);

    int numDocs = 0;  // Number of docs created in this section

    QDomNode n = sectNode.firstChild();
    while( !n.isNull() ) {
        QDomElement e = n.toElement();
        if( !e.isNull() ) {
            if (e.tagName() == "title") {
                sectItem->setText(0,e.text());
            } else if (e.tagName() == "sect") {
                NavigatorItem *created;
                numDocs += insertScrollKeeperSection( sectItem, 0, e, created );
            } else if (e.tagName() == "doc") {
                insertScrollKeeperDoc(sectItem,e);
                ++numDocs;
            }
        }
        n = n.nextSibling();
    }

    // Remove empty sections
    if (!mScrollKeeperShowEmptyDirs && numDocs == 0) {
      delete sectItem;
      sectItem = 0;
    }

    return numDocs;
}

void Navigator::insertScrollKeeperDoc(NavigatorItem *parentItem,QDomNode docNode)
{
    NavigatorItem *docItem = new NavigatorItem(parentItem,"","document2");
    scrollKeeperItems.append(docItem);

    QString url;

    QDomNode n = docNode.firstChild();
    while( !n.isNull() ) {
        QDomElement e = n.toElement();
        if( !e.isNull() ) {
            if (e.tagName() == "doctitle") {
                docItem->setText(0,e.text());
            } else if (e.tagName() == "docsource") {
                url.append(e.text());
            } else if (e.tagName() == "docformat") {
                QString mimeType = e.text();
                if (mimeType == "text/html") {
                    // Let the HTML part figure out how to get the doc
                } else if (mimeType == "text/xml") {
                    if ( url.left( 5 ) == "file:" ) url = url.mid( 5 );
                    url.prepend( "ghelp:" );
                    url.replace( QRegExp( ".xml$" ), ".html" );
                } else if (mimeType == "text/sgml") {
                    // GNOME docs use this type. We don't have a real viewer for this.
                    url.prepend("file:");
                } else if (mimeType.left(5) == "text/") {
                    url.prepend("file:");
                }
            }
        }
        n = n.nextSibling();
    }

    docItem->setUrl(url);
}

void Navigator::selectItem( const KURL &url )
{
  kdDebug() << "Navigator::selectItem(): " << url.url() << endl;

  if ( url.protocol() == "help" ) {
    kdDebug() << "TODO: Look up help item." << endl;
  } else {
    QListViewItemIterator it( mContentsTree );
    while ( it.current() != 0 ) {
      NavigatorItem *item = static_cast<NavigatorItem *>( it.current() );
      KURL itemUrl( item->url() );
      if ( itemUrl == url ) {
        mContentsTree->setCurrentItem( item );
        mContentsTree->ensureItemVisible( item );
        break;
      }
      ++it;
    }
  }
}

void Navigator::clearSelection()
{
  mContentsTree->clearSelection();
}

void Navigator::slotURLSelected(QString url)
{
    emit itemSelected(url);
}

void Navigator::slotItemSelected(QListViewItem* currentItem)
{
  if (!currentItem)
    return;
  NavigatorItem *item = static_cast<NavigatorItem*>(currentItem);

  kdDebug(1400) << "Navigator::slotItemSelected(): " << item->name() << endl;  

  if (item->childCount() > 0 || item->isExpandable())
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

  if (!item->url().isEmpty()) {
    KURL u = item->url();
    if ( u.protocol() == "help" ) {
      kdDebug( 1400 ) << "slotURLSelected(): Got help URL " << item->url() << endl;
      if ( !item->toc() ) {
        TOC *tocTree = item->createTOC();
        kdDebug( 1400 ) << "slotURLSelected(): Trying to build TOC for " << item->name() << endl;
        tocTree->setApplication( u.directory() );
        QString doc = View::langLookup( u.path() );
        // Enforce the original .docbook version, in case langLookup returns a
        // cached version
        if ( !doc.isNull() ) {
          int pos = doc.find( ".html" );
          if ( pos >= 0 ) {
            doc.replace( pos, 5, ".docbook" );
          }
          kdDebug( 1400 ) << "slotURLSelected(): doc = " << doc << endl;

          tocTree->build( doc );
        }
      }
    }
    emit itemSelected(item->url());
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

  slotURLSelected( u );
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

// vim:ts=2:sw=2:et
