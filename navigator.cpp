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
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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
#if KDE_VERSION < 305
#  include <kservice.h>
#else
#  include <kservicegroup.h>
#endif

#include "navigatoritem.h"
#include "navigatorappitem.h"
#include "searchwidget.h"
#include "infoconsts.h"
#include "infohierarchymaker.h"
#include "infonode.h"
#include "searchengine.h"
#include "view.h"
#include "docmetainfo.h"
#include "docentrytraverser.h"
#include "glossary.h"
#include "toc.h"
#include "view.h"

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

    // compiling the regex used while parsing the info directory (dir) file
    int nResult = regcomp(&compInfoRegEx, "^\\* ([^:]+)\\: \\(([^)]+)\\)([[:space:]]|(([^.]*)\\.)).*$", REG_EXTENDED);
    Q_ASSERT(!nResult);

    // set up the timer which produces signals every 30 sec. The cleanHierarchyMakers function
    // checks entries in the hierarchyMakers map and deletes these which have already finished work.
    connect(&cleaningTimer, SIGNAL(timeout()), SLOT(slotCleanHierarchyMakers()));
    cleaningTimer.start(30000);

    /* Cog-wheel animation handling -- enable after creating the icons
    m_animationTimer = new QTimer( this );
    connect( m_animationTimer, SIGNAL( timeout() ), this, SLOT( slotAnimation() ) );
    */

    buildTree();

    if ( DocMetaInfo::self()->searchEntries().isEmpty() ) {
      hideSearch();
    } else {
      mSearchWidget->updateScopeList();
      slotSearchTextChanged( mSearchEdit->text() );
    }
}

Navigator::~Navigator()
{
  HierarchyMap::Iterator it = hierarchyMakers.begin();
  HierarchyMap::Iterator end = hierarchyMakers.end();
  for (; it != end; ++it)
    delete *it;

  regfree(&compInfoRegEx);

  delete mSearchEngine;
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
    connect(mContentsTree, SIGNAL(expanded(QListViewItem*)),
            SLOT(slotItemExpanded(QListViewItem*)));
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

void Navigator::buildTree()
{
  // scan plugin dir for plugins
  insertPlugins();

  insertScrollKeeperItems();
}

void Navigator::clearTree()
{
  // Remove all children.
  for(QListViewItem *child = mContentsTree->firstChild(); child; child = mContentsTree->firstChild())
  {
     delete child;
  }
}

void Navigator::buildInfoSubTree(NavigatorItem *parent)
{
  QString dirContents;
  if (!readInfoDirFile(dirContents)) return;

  // actual processing...
  QRegExp reSectionHdr("^[A-Za-z0-9]");
  QTextStream stream(&dirContents, IO_ReadOnly);
  QString sLine;

  sLine = stream.readLine();
  while (!sLine.isNull()) {
    if ( sLine.startsWith( "* Menu: " ) ) {
      // will point to the last added section item
      NavigatorItem* pLastSection = 0;

      sLine = stream.readLine();
      while (!sLine.isNull()) {
        if (reSectionHdr.search(sLine, 0) == 0) {
          // add the section header
          NavigatorItem* pSectionRoot = new NavigatorItem(parent, pLastSection, sLine, "contents2");
          pSectionRoot->setUrl("");

          // will point to the last added subitem
          NavigatorItem* pLastChild = 0;

          sLine = stream.readLine();
          while (!sLine.isNull()) {
            if (sLine.startsWith("* ")) {
              NavigatorItem *item = addInfoNode( pSectionRoot, pLastChild, sLine );
              if ( item ) pLastChild = item;
            } else if (sLine.isEmpty())
              break; // go to the next section
            sLine = stream.readLine();
          }

          if (pSectionRoot->childCount() > 0) pLastSection = pSectionRoot;
          else delete pSectionRoot;
        } else if (sLine.startsWith("* ")) {
          NavigatorItem *item = addInfoNode( parent, pLastSection, sLine );
          if ( item ) pLastSection = item;
        }
        sLine = stream.readLine();
      }
    }
    sLine = stream.readLine();
  }
}

NavigatorItem *Navigator::addInfoNode( NavigatorItem *parent,
                                                   NavigatorItem *last,
                                                   const QString &line )
{
  QString sItemTitle, sItemURL;
  if (parseInfoSubjectLine(line, sItemTitle, sItemURL))
  {
    // add subject's root node
    NavigatorItem *pItem = new NavigatorItem(parent, last, sItemTitle, "document2");
    pItem->setUrl(sItemURL);
    pItem->setExpandable(true);
    return pItem;
  }
  
  return 0;
}

QString Navigator::findInfoDirFile()
{
  for (uint i = 0; i < INFODIRS; i++)
    if (QFile::exists(INFODIR[i] + "dir"))
      return INFODIR[i] + "dir";
  return QString();
}

bool Navigator::readInfoDirFile(QString& sFileContents)
{
  const QString dirPath = findInfoDirFile();
  if (dirPath.isEmpty())
  {
    kdWarning() << "Info directory (dir) file not found." << endl;
    return false;
  }

  QFile file(dirPath);
  if (!file.open(IO_ReadOnly))
  {
    kdWarning() << "Cannot open info directory (dir) file." << endl;
    return false;
  }

  QTextStream stream(&file);
  sFileContents = stream.read();

  file.close();
  return true;
}

bool Navigator::parseInfoSubjectLine(QString sLine, QString& sItemTitle, QString& sItemURL)
{
  regmatch_t* pRegMatch = new regmatch_t[compInfoRegEx.re_nsub + 1];
  Q_CHECK_PTR(pRegMatch);

  int nResult = regexec(&compInfoRegEx, sLine.latin1(),
                        compInfoRegEx.re_nsub + 1, pRegMatch, 0);
  if (nResult)
  {
    kdWarning() << "Could not parse line \'" << sLine << "\' from the info directory (dir) file; regexec() returned " <<
      nResult << "." << endl;
    delete[] pRegMatch;
    return false;
  }

  Q_ASSERT(pRegMatch[0].rm_so == 0 && pRegMatch[0].rm_eo == int(sLine.length()));

  sItemTitle = sLine.mid(pRegMatch[1].rm_so, pRegMatch[1].rm_eo - pRegMatch[1].rm_so);
  sItemURL = "info:/" + sLine.mid(pRegMatch[2].rm_so, pRegMatch[2].rm_eo - pRegMatch[2].rm_so);
  if (pRegMatch[5].rm_eo - pRegMatch[5].rm_so > 0) // it isn't the main node
    sItemURL += "/" + sLine.mid(pRegMatch[5].rm_so, pRegMatch[5].rm_eo - pRegMatch[5].rm_so);

  //   kdDebug( 1400 ) << "title: " << sItemTitle << "; url: " << sItemURL << endl;

  delete[] pRegMatch;
  return true;
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

      bool missing = false;
      QString docPath = entry->docPath();
      if ( !docPath.isEmpty() ) {
        KURL docUrl( docPath );
        if ( docUrl.isLocalFile() && !KStandardDirs::exists( docUrl.path() ) ) {
          missing = true;
        }
      }

      if ( missing & !mNavigator->showMissingDocs() ) return;

      if (entry->khelpcenterSpecial() == "apps") {
        if ( mListView )
          mCurrentItem = new NavigatorAppItem( mListView, mCurrentItem );
        else
          mCurrentItem = new NavigatorAppItem( mParentItem, mCurrentItem );
      } else {
        if ( mListView )
          mCurrentItem = new NavigatorItem( mListView, mCurrentItem );
        else
          mCurrentItem = new NavigatorItem( mParentItem, mCurrentItem );

        if ( entry->khelpcenterSpecial() == "info" )
          mNavigator->buildInfoSubTree( mCurrentItem );
        else if (entry->khelpcenterSpecial() == "applets" )
          mNavigator->insertAppletDocs( mCurrentItem );
        else if ( entry->khelpcenterSpecial() == "kinfocenter" ||
                  entry->khelpcenterSpecial() == "kcontrol" ||
                  entry->khelpcenterSpecial() == "konqueror" )
          mNavigator->insertParentAppDocs( entry->khelpcenterSpecial(),
                                           mCurrentItem );
      }

      mCurrentItem->setName( entry->name() );
      mCurrentItem->setUrl( entry->docPath() );
      
      if ( missing ) {
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
      if ( mCurrentItem ) return new PluginTraverser( mNavigator, mCurrentItem );
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
 
#if KDE_VERSION < 305
// Disabled because it does not work.
#if 0
  KService::List services = KService::allServices();
  KService::List::ConstIterator it = services.begin();
  KService::List::ConstIterator end = services.end();
  for ( ; it != end; ++it ) {
    KService::Ptr srv = *it;
    if ( srv->property( QString::fromLatin1( "X-KDE-ParentApp" ) ) == name )
      createItemFromDesktopFile( topItem,
          locate( "apps", srv->name() ) );
  }
#endif
#else
  KServiceGroup::Ptr grp = KServiceGroup::childGroup( name );
  if ( !grp ) {
    kdDebug(1400) << "Eeek, our group pointer is NULL!" << endl;
    return;
  }

  KServiceGroup::List entries = grp->entries();
  KServiceGroup::List::ConstIterator it = entries.begin();
  KServiceGroup::List::ConstIterator end = entries.end();
  for ( ; it != end; ++it ) {
    QString desktopFile = ( *it )->entryPath();
    if ( QDir::isRelativePath( desktopFile ) )
        desktopFile = locate( "apps", desktopFile );
    createItemFromDesktopFile( topItem, desktopFile );
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
#if KDE_VERSION < 305
    QString docPath = desktopFile.readEntry( "DocPath" );
#else
    QString docPath = desktopFile.readDocPath();
#endif
    if ( !docPath.isNull() ) {
      NavigatorItem *item = new NavigatorItem( topItem, desktopFile.readName() );
      item->setUrl( "help:/" + docPath );
      QString icon = desktopFile.readIcon();
      item->setIcon( icon.isNull() ? "document2" : icon );
    }
}

void Navigator::insertScrollKeeperItems()
{
    KProcIO proc;
    proc << "scrollkeeper-get-content-list";
    proc << KGlobal::locale()->language();
    connect(&proc,SIGNAL(readReady(KProcIO *)),SLOT(getScrollKeeperContentsList(KProcIO *)));
    if (!proc.start(KProcess::Block)) {
      kdDebug(1400) << "Could not execute scrollkeeper-get-content-list" << endl;
      return;
    }

    if (!QFile::exists(mScrollKeeperContentsList)) {
      kdDebug(1400) << "Scrollkeeper contents file '" << mScrollKeeperContentsList
                    << "' does not exist." << endl;
      return;
    }

    QDomDocument doc("ScrollKeeperContentsList");
    QFile f(mScrollKeeperContentsList);
    if ( !f.open( IO_ReadOnly ) )
        return;
    if ( !doc.setContent( &f ) ) {
        f.close();
        return;
    }
    f.close();

    // Create top-level item
    NavigatorItem *topItem = new NavigatorItem(mContentsTree, i18n("Scrollkeeper"),"contents2");
    topItem->setUrl("");
    scrollKeeperItems.append(topItem);

    QDomElement docElem = doc.documentElement();

    QDomNode n = docElem.firstChild();
    while( !n.isNull() ) {
        QDomElement e = n.toElement();
        if( !e.isNull() ) {
            if (e.tagName() == "sect") {
              insertScrollKeeperSection(topItem,e);
            }
        }
        n = n.nextSibling();
    }
}

void Navigator::getScrollKeeperContentsList(KProcIO *proc)
{
    QString filename;
    proc->readln(filename,true);

    mScrollKeeperContentsList = filename;
}

int Navigator::insertScrollKeeperSection(NavigatorItem *parentItem,QDomNode sectNode)
{
    NavigatorItem *sectItem = new NavigatorItem(parentItem,"","contents2");
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
                numDocs += insertScrollKeeperSection(sectItem,e);
            } else if (e.tagName() == "doc") {
                insertScrollKeeperDoc(sectItem,e);
                ++numDocs;
            }
        }
        n = n.nextSibling();
    }

    // Remove empty sections
    if (!mScrollKeeperShowEmptyDirs && numDocs == 0) delete sectItem;

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
                    // Should probably check for the DTD here
                    url.prepend("help:");
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
      if ( item->url() == url.url() ) {
        item->setSelected( true );
        mContentsTree->ensureItemVisible( item );
        break;
      }
      ++it;
    }
  }
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

  kdDebug() << "Navigator::slotItemSelected(): " << item->name() << endl;  

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
          doc.replace( doc.find( ".html" ), 5, ".docbook" );
          kdDebug( 1400 ) << "slotURLSelected(): doc = " << doc << endl;

          tocTree->build( doc );
        }
      }
    }
    emit itemSelected(item->url());
  }
}

void Navigator::slotItemExpanded(QListViewItem* index)
{
  if (!index)
    return;

  kdDebug() << "Navigator::slotItemExpanded()" << endl;  

  QListViewItem* parent;
  if ((parent = index->parent())) // it _is_ an assignment, not a comparison !
    if ((parent = parent->parent())) // it _is_ an assignment, not a comparison !
      // item is at least on the third level of the tree
      if (parent->text(0) == i18n("Browse info pages") && index->childCount() == 0)
        // it is an unexpanded info subject's root node. Let's check if we are have already started to create the hierarchy.
      {
        NavigatorItem* item = static_cast<NavigatorItem*>(index);
          // const QString itemName(item->name());
        if (hierarchyMakers.find(item) == hierarchyMakers.end())
          // no. We must create one.
        {
          InfoHierarchyMaker* pMaker = new InfoHierarchyMaker;
          Q_CHECK_PTR(pMaker);
          hierarchyMakers[item] = pMaker;

          QString sURL = item->url();
          Q_ASSERT(!sURL.isEmpty());

          regex_t reInfoURL;
          int nResult = regcomp(&reInfoURL, "^info:/([^/]*)(/(.*))?$", REG_EXTENDED);
          Q_ASSERT(!nResult);
          Q_ASSERT(reInfoURL.re_nsub == 3);

          regmatch_t regMatch[4];

          nResult = regexec(&reInfoURL, sURL.latin1(), reInfoURL.re_nsub + 1, regMatch, 0);
          if (nResult)
          {
            kdWarning() << "Could not parse URL \'" << sURL << "\'; regexec() returned " << nResult << "." << endl;
            hierarchyMakers.erase(item);
            item->setExpandable(false);
            return;
          }

          Q_ASSERT(regMatch[0].rm_so == 0 && regMatch[0].rm_eo == int(sURL.length()));

          QString sTopic = sURL.mid(regMatch[1].rm_so, regMatch[1].rm_eo - regMatch[1].rm_so);
          QString sNode = sURL.mid(regMatch[3].rm_so, regMatch[3].rm_eo - regMatch[3].rm_so);

          kdDebug( 1400 ) << "sTopic: \'" << sTopic << "\'; sNode: \'" << sNode << "\'" << endl;

          // begin creating hierarchy!

          /* Cog-wheel animation handling -- enable after creating the icons
             startAnimation(item);
          */
          connect(pMaker, SIGNAL(hierarchyCreated(uint, uint, const InfoNode*)),
                  SLOT(slotInfoHierarchyCreated(uint, uint, const InfoNode*)));
          pMaker->createHierarchy((uint) item, sTopic, sNode);

          regfree(&reInfoURL);
        }
        else
          kdDebug( 1400 ) << "Hierarchy creation already in progress..." << endl;
      }
}

void Navigator::slotInfoHierarchyCreated(uint key, uint nErrorCode, const InfoNode* pRootNode)
{
  Q_ASSERT(key);
  NavigatorItem* pItem = (NavigatorItem*) key;

  kdDebug( 1400 ) << "Info hierarchy for subject \'" << pItem->name() << "\'created! Result: " << nErrorCode << endl;

  if (!nErrorCode)
  {
    if (pRootNode->m_lChildren.empty())
      // "Hierarchy" consists of only one element (pRootNode has no children)
    {
      pItem->setExpandable(false);
      pItem->repaint();
    }
    else
      addChildren(pRootNode, pItem);
    /* Cog-wheel animation handling -- enable after creating the icons
    stopAnimation(pItem);
    */
  }
  else
  {
    /* Cog-wheel animation handling -- enable after creating the icons
    stopAnimation(pItem);
    */

    QString sErrMsg;
    switch (nErrorCode)
    {
    case ERR_FILE_UNAVAILABLE:
      sErrMsg = i18n("One or more files containing info nodes belonging to the subject '%1' do(es) not exist.").arg(pItem->name());
      break;
    case ERR_NO_HIERARCHY:
      sErrMsg = i18n("Info nodes belonging to the subject '%1' seem to be not ordered in a hierarchy.").arg(pItem->name());
      break;
    default:
      sErrMsg = i18n("An unknown error occurred while creating the hierarchy of info nodes belonging to the subject '%1'.").arg(pItem->name());
    }
    KMessageBox::sorry(0, sErrMsg, i18n("Cannot Create Hierarchy of Info Nodes"));
    pItem->setExpandable(false);
    pItem->repaint();
  }
}

void Navigator::addChildren(const InfoNode* pParentNode, NavigatorItem* pParentTreeItem)
{
  NavigatorItem* pLastChild = 0;
  for (std::list<InfoNode*>::const_iterator it = pParentNode->m_lChildren.begin();
       it != pParentNode->m_lChildren.end(); ++it)
  {
    //    NavigatorItem *pItem = new NavigatorItem(pParentTreeItem, pLastChild, (*it)->m_sTitle, "document2");
    NavigatorItem *pItem = new NavigatorItem(pParentTreeItem, pLastChild,
                                                   (*it)->m_sTitle.isEmpty() ? (*it)->m_sName : (*it)->m_sTitle, "document2");
    pItem->setUrl("info:/" + (*it)->m_sTopic + "/" + (*it)->m_sName);
    pLastChild = pItem;

    addChildren(*it, pItem);
  }
}

void Navigator::slotCleanHierarchyMakers()
{
//  kdDebug( 1400 ) << "--- slotCleanHierarchyMakers ---" << endl;
  HierarchyMap::Iterator it = hierarchyMakers.begin();
  HierarchyMap::Iterator end = hierarchyMakers.end();
  for (; it != end; ++it)
    if (!(*it)->isWorking())
      delete *it;
}

/* Cog-wheel animation handling -- enable after creating the icons
void Navigator::slotAnimation()
{
    MapCurrentOpeningSubjects::Iterator it = m_mapCurrentOpeningSubjects.begin();
    MapCurrentOpeningSubjects::Iterator end = m_mapCurrentOpeningSubjects.end();
    for (; it != end; ++it )
    {
        uint & iconNumber = it.data().iconNumber;
        QString icon = QString::fromLatin1( it.data().iconBaseName ).append( QString::number( iconNumber ) );
        it.key()->setPixmap( 0, SmallIcon( icon, Factory::instance() ) );

        iconNumber++;
        if ( iconNumber > it.data().iconCount )
            iconNumber = 1;
    }
}

void Navigator::startAnimation( NavigatorItem * item, const char * iconBaseName, uint iconCount )
{
    m_mapCurrentOpeningSubjects.insert( item, AnimationInfo( iconBaseName, iconCount, *item->pixmap(0) ) );
    if ( !m_animationTimer->isActive() )
        m_animationTimer->start( 50 );
}

void Navigator::stopAnimation( NavigatorItem * item )
{
    MapCurrentOpeningSubjects::Iterator it = m_mapCurrentOpeningSubjects.find(item);
    if ( it != m_mapCurrentOpeningSubjects.end() )
    {
        item->setPixmap( 0, it.data().originalPixmap );
        m_mapCurrentOpeningSubjects.remove( item );
    }
    if (m_mapCurrentOpeningSubjects.isEmpty())
        m_animationTimer->stop();
}
*/


void Navigator::slotSearch()
{
  if ( !checkSearchIndex() ) return;

  QString words = mSearchEdit->text();
  QString method = mSearchWidget->method();
  int pages = mSearchWidget->pages();
  QString scope = mSearchWidget->scope();

  if ( words.isEmpty() || scope.isEmpty() ) return;

  // disable search Button during searches
  mSearchButton->setEnabled(false);
  QApplication::setOverrideCursor(waitCursor);

  if ( !mSearchEngine->search( words, method, pages, scope ) ) {
    slotSearchFinished();
  }
}

void Navigator::slotShowSearchResult( const QString &url )
{
  QString u = url;
  u.replace( QRegExp( "%k" ), mSearchEdit->text() );

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

  QString text = i18n( "There doesn't exist a search index yet. Do you want "
                       "to create the index now?" );

  int result = KMessageBox::questionYesNo( this, text, QString::null, 
                                           KStdGuiItem::yes(),
                                           KStdGuiItem::no(),
                                           "indexcreation" );
  if ( result == KMessageBox::Yes ) {
    mSearchWidget->slotIndex();
  }
  
  return false;
}

void Navigator::slotTabChanged( QWidget *wid )
{
  if ( wid == mSearchWidget ) checkSearchIndex();
}

// vim:ts=2:sw=2:et
