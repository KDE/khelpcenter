/*
 *  khc_navigator.cc - part of the KDE Help Center
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

#include "khc_navigator.h"
#include "khc_navigatoritem.h"
#include "khc_indexwidget.h"
#include "khc_searchwidget.h"
#include "khc_factory.h"

#include <qdir.h>
#include <qfile.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qlabel.h>
#include <qtabbar.h>
#include <qheader.h>

#include <kaction.h>
#include <kapp.h>
#include <ksimpleconfig.h>
#include <kstddirs.h>	
#include <kglobal.h>
#include <klocale.h>
#include <kdebug.h>
#include <klistview.h>
#include <kservicegroup.h>
#include <ksycocaentry.h>
#include <kservice.h>
#include <kservicegroup.h>      
#include <kmessagebox.h>                                                             


template class QList<khcNavigatorItem>;

khcNavigator::khcNavigator(QWidget *parent, const char *name)
    : KParts::ReadOnlyPart(parent,name)
{
    kDebugInfo("khcNavigator::khcNavigator");
    setInstance( KHCFactory::instance() );

    setWidget( new khcNavigatorWidget( parent ) );

    m_extension = new khcNavigatorExtension( this, "khcNavigatorExtension" );
    connect( widget(), SIGNAL( itemSelected(const QString&) ),
             m_extension, SLOT( slotItemSelected(const QString&) ) );
}

bool khcNavigator::openFile()
{
  return true; // easy one...
}

khcNavigator::~khcNavigator()
{
  // KParts deletes the widget. Cool.
}

void khcNavigatorExtension::slotItemSelected(const QString& url)
{
  KParts::URLArgs urlArgs( false, 0, 0 );

  emit openURLRequest( url, urlArgs );
}

khcNavigatorWidget::khcNavigatorWidget(QWidget *parent, const char *name)
   : QWidget(parent, name)
{
    tabBar = new QTabBar(parent);

    setupContentsTab();
    setupIndexTab();
    setupSearchTab();

    connect(tabBar, SIGNAL(selected(int)),this,
	    SLOT(slotTabSelected(int)));

    buildTree();
}

khcNavigatorWidget::~khcNavigatorWidget()
{
    delete tree;
    delete search;
    delete index;
    delete tabBar;
}

void khcNavigatorWidget::resizeEvent(QResizeEvent *)
{
    tabBar->setGeometry(0, 0, width(), 28);
    tree->setGeometry(0, 28, width(), height()-28);
    search->setGeometry(0, 28, width(), height()-28);
    index->setGeometry(0, 28, width(), height()-28);
}

void khcNavigatorWidget::setupContentsTab()
{
    tree = new KListView(this);
    tree->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    tree->addColumn("");
    tree->setAllColumnsShowFocus(true);
    tree->header()->hide();
    tree->setRootIsDecorated(false);
    tree->setSorting(-1, false);

    connect(tree, SIGNAL(clicked(QListViewItem*)),this,
	    SLOT(slotItemSelected(QListViewItem*)));

    QTab *newTab = new QTab;
    newTab->label = "Contents";
    tabBar->addTab(newTab);
    tree->show();
}

void khcNavigatorWidget::setupIndexTab()
{
    index = new IndexWidget(this);
    index->hide();

    QTab *newTab = new QTab;
    newTab->label = "Man/Info";
    tabBar->addTab(newTab);
}

void khcNavigatorWidget::setupSearchTab()
{
    search = new SearchWidget(this);
    search->hide();

    connect(search, SIGNAL(matchSelected(QString)),this,
	    SLOT(slotURLSelected(QString)));

    QTab *newTab = new QTab;
    newTab->label = "Search";
    tabBar->addTab(newTab);
}

void khcNavigatorWidget::buildTree()
{
  // kde contacts
  khcNavigatorItem *ti_contact = new khcNavigatorItem(tree, i18n("Contact Information"), "helpdoc.png");
  ti_contact->setURL(QString("help:/khelpcenter/contact.html"));
  staticItems.append(ti_contact);

  // kde links
  khcNavigatorItem *ti_links = new khcNavigatorItem(tree, i18n("KDE on the web"), "helpdoc.png");
  ti_links->setURL(QString("help:/khelpcenter/links.html"));
  staticItems.append(ti_links);

  // KDE FAQ
  khcNavigatorItem *ti_faq = new khcNavigatorItem(tree, i18n("The KDE FAQ"), "helpdoc.png");
  ti_faq->setURL(QString("help:/khelpcenter/faq/index.html"));
  staticItems.append(ti_faq);

  // scan plugin dir for plugins
  insertPlugins();

  // info browser
  khcNavigatorItem *ti_info = new khcNavigatorItem(tree, i18n("Browse info pages"), "helpdoc.png");
  ti_info->setURL(QString("info:/dir"));
  staticItems.append(ti_info);

  // unix man pages
  khcNavigatorItem *ti_man = new khcNavigatorItem(tree, i18n("Unix manual pages"), "helpbook.png");
  ti_man->setURL(QString("man:/(index)"));
  staticItems.append(ti_man);

  // fill the man pages subtree
  buildManSubTree(ti_man);

  // application manuals
  khcNavigatorItem *ti_manual = new khcNavigatorItem(tree, i18n("Application manuals"), "helpbook.png");
  ti_manual->setURL("");
  staticItems.append(ti_manual);

  // fill the application manual subtree
  buildManualSubTree(ti_manual, "");

  // KDE user's manual
  khcNavigatorItem *ti_um = new khcNavigatorItem(tree, i18n("KDE user's manual"), "helpdoc.png");
  ti_um->setURL(QString("help:/khelpcenter/userguide/index.html"));
  staticItems.append(ti_um);

  // KDE quickstart guide
  khcNavigatorItem *ti_qs = new khcNavigatorItem(tree, i18n("Introduction to KDE"), "helpdoc.png");
  ti_qs->setURL(QString("help:/khelpcenter/quickstart/index.html"));
  staticItems.append(ti_qs);

  // introduction page
  khcNavigatorItem *ti_intro = new khcNavigatorItem(tree, i18n("Introduction"), "helpdoc.png");
  ti_intro->setURL(QString("help:/khelpcenter/main.html"));
  staticItems.append(ti_intro);

  tree->setCurrentItem(ti_intro);
}

void khcNavigatorWidget::clearTree()
{
    tree->clear();

    while(!staticItems.isEmpty())
	staticItems.removeFirst();

    while(!manualItems.isEmpty())
	manualItems.removeFirst();

    while(!pluginItems.isEmpty())
	pluginItems.removeFirst();
}

void khcNavigatorWidget::buildManSubTree(khcNavigatorItem *parent)
{
  // man(n)
  khcNavigatorItem *ti_man_sn = new khcNavigatorItem(parent, i18n("(n) New"), "helpdoc.png");
  ti_man_sn->setURL(QString("man:/(n)"));
  staticItems.append(ti_man_sn);

  // man(9)
  khcNavigatorItem *ti_man_s9 = new khcNavigatorItem(parent, i18n("(9) Kernel"), "helpdoc.png");
  ti_man_s9->setURL(QString("man:/(9)"));
  staticItems.append(ti_man_s9);

  // man(8)
  khcNavigatorItem *ti_man_s8 = new khcNavigatorItem(parent, i18n("(8) Sys. Administration"), "helpdoc.png");
  ti_man_s8->setURL(QString("man:/(8)"));
  staticItems.append(ti_man_s8);

  // man(7)
  khcNavigatorItem *ti_man_s7 = new khcNavigatorItem(parent, i18n("(7) Miscellaneous"), "helpdoc.png");
  ti_man_s7->setURL(QString("man:/(7)"));
  staticItems.append(ti_man_s7);

  // man(6)
  khcNavigatorItem *ti_man_s6 = new khcNavigatorItem(parent, i18n("(6) Games"), "helpdoc.png");
  ti_man_s6->setURL(QString("man:/(6)"));
  staticItems.append(ti_man_s6);

  // man(5)
  khcNavigatorItem *ti_man_s5 = new khcNavigatorItem(parent, i18n("(5) File Formats"), "helpdoc.png");
  ti_man_s5->setURL(QString("man:/(5)"));
  staticItems.append(ti_man_s5);

  // man(4)
  khcNavigatorItem *ti_man_s4 = new khcNavigatorItem(parent, i18n("(4) Devices"), "helpdoc.png");
  ti_man_s4->setURL(QString("man:/(4)"));
  staticItems.append(ti_man_s4);

  // man(3)
  khcNavigatorItem *ti_man_s3 = new khcNavigatorItem(parent, i18n("(3) Subroutines"), "helpdoc.png");
  ti_man_s3->setURL(QString("man:/(3)"));
  staticItems.append(ti_man_s3);

  // man(2)
  khcNavigatorItem *ti_man_s2 = new khcNavigatorItem(parent, i18n("(2) System calls"), "helpdoc.png");
  ti_man_s2->setURL(QString("man:/(2)"));
  staticItems.append(ti_man_s2);
  // man (1)
  khcNavigatorItem *ti_man_s1 = new khcNavigatorItem(parent, i18n("(1) Usr commands"), "helpdoc.png");
  ti_man_s1->setURL(QString("man:/(1)"));
  staticItems.append(ti_man_s1);
}

void khcNavigatorWidget::buildManualSubTree(khcNavigatorItem *parent, QString relPath)
{
  KServiceGroup::Ptr root = KServiceGroup::group(relPath);
  KServiceGroup::List list = root->entries();                                                

   
  for (KServiceGroup::List::ConstIterator it = list.begin(); it != list.end(); ++it)
    {
      KSycocaEntry * e = *it;
      KService::Ptr s;
      khcNavigatorItem *item;
      KServiceGroup::Ptr g;
      QString url;

      switch (e->sycocaType()) 
	{	
	case KST_KService:
	  s = static_cast<KService*>(e);
	  url = documentationURL(s);
	  if (!url.isEmpty())
	    {
	      item = new khcNavigatorItem(parent, s->name(), s->icon());
	      item->setURL(url);
	      staticItems.append(item);
	    }
	  break;
	  
	case KST_KServiceGroup:
	  g = static_cast<KServiceGroup*>(e);
	  item = new khcNavigatorItem(parent, g->caption(), g->icon());
	  item->setURL("");
	  buildManualSubTree(item, g->relPath());
	  if (item->childCount() > 0)
	    staticItems.append(item);
	  else
	    delete item;
	  break;

	default:
	  break;
	}
    }
}



bool lookupFile(QString fname)
{
  QStringList search;
 
  // assemble the local search paths
  QStringList const &localDoc = KGlobal::dirs()->findDirs("html", "");
 
  // look up the different languages
  for (int id=localDoc.count()-1; id >= 0; --id)
    {
      QStringList langs = KGlobal::locale()->languageList();
      langs.append("default");
      langs.append("en");
      QStringList::ConstIterator lang;
      for (lang = langs.begin(); lang != langs.end(); ++lang)
        search.append(QString("%1/%2/%3").arg(localDoc[id]).arg(*lang).arg(fname));
    }
 
  // try to locate the urls
  QStringList::Iterator it;
  for (it = search.begin(); it != search.end(); ++it)
    {
      QFileInfo info(*it);
      if (info.exists() && info.isFile() && info.isReadable())
	return true;
    }
 
  return false;
} 


// derive a valid URL to the documentation
QString khcNavigatorWidget::documentationURL(KService *s)
{
  // if entry contains a DocPath, process it

  // TODO: Once DocPath is in ksycoca, use this instead of
  // parsing the file
  //  QString docPath = s->property("DocPath").toString();
  
  QString path = locate("apps", s->desktopEntryPath());
  KSimpleConfig config(path, true);
  config.setDesktopGroup();
  QString docPath = config.readEntry("DocPath");

  if (!docPath.isEmpty())
    {
      // see if it is part of our help system, or external
      // note that this test might be a bit too stupid
      if (docPath.left(5) == "file:" || docPath.left(5) == "http:")
	return docPath;
      
      // strip index.html, as we will have to look
      // for the anchor first
      if (docPath.right(11) == "/index.html")
	docPath = docPath.left(docPath.length() - 11);      
    }
  else
    docPath = s->desktopEntryName();

  // TODO: extract the name of the index file from the .anchors file
  if (lookupFile(docPath+"/index.html"))
    return QString("help:/%1/index.html").arg(docPath);    

  return QString::null;
}


void khcNavigatorWidget::insertPlugins()
{
    // Scan plugin dir
    QStringList list = KGlobal::dirs()->findDirs("appdata", "plugins");
    for(QStringList::Iterator it=list.begin(); it!=list.end(); it++) {
      processDir(*it, 0, &pluginItems);
      appendEntries(*it, 0, &pluginItems);
    }
}

void khcNavigatorWidget::slotReloadTree()
{
    emit setBussy(true);
    clearTree();
    buildTree();
    emit setBussy(false);
}

void khcNavigatorWidget::slotTabSelected(int id)
{
    if (id == 0)
    {
	tree->show();
	index->hide();
	search->hide();
    }
    else if (id == 1)
    {
	tree->hide();
	index->show();
	search->hide();
	index->tabSelected();
    }
    else if (id == 2)
    {
	tree->hide();
	index->hide();
	search->show();
	search->tabSelected();
    }
}

void khcNavigatorWidget::slotURLSelected(QString url)
{
  emit itemSelected(url);
}

void khcNavigatorWidget::slotItemSelected(QListViewItem* currentItem)
{
  if (!currentItem)
    return;
  khcNavigatorItem *item = static_cast<khcNavigatorItem*>(currentItem);

  if (item->childCount() > 0)
    {
      if (item->isOpen())
        item->setOpen(false);
      else
        item->setOpen(true);
    }

  // find the highlighted item in our lists
  for (item = staticItems.first(); item != 0; item = staticItems.next())
    {
      if (item == tree->currentItem())
	{
	  if (!item->getURL().isEmpty())
	    emit itemSelected(item->getURL());
	  return;
	}
    }
  for (item = manualItems.first(); item != 0; item = manualItems.next())
    {
      if (item == tree->currentItem())
	{
	  if (!item->getURL().isEmpty())
	    emit itemSelected(item->getURL());
	  return;
	}
    }
  for (item = pluginItems.first(); item != 0; item = pluginItems.next())
    {
      if (item == tree->currentItem())
	{
	  if (!item->getURL().isEmpty())
	    emit itemSelected(item->getURL());
	  return;
	}
    }
}

bool khcNavigatorWidget::appendEntries(const char *dirName, khcNavigatorItem *parent, QList<khcNavigatorItem> *appendList)
{
    QDir fileDir(dirName, "*.desktop", 0, QDir::Files | QDir::Hidden | QDir::Readable);

    if (!fileDir.exists())
	return false;

    QStringList fileList = fileDir.entryList();
    QStringList::Iterator itFile;

    for ( itFile = fileList.begin(); !(*itFile).isNull(); ++itFile )
    {
	QString filename = dirName;
	filename += "/";
	filename += *itFile;

	khcNavigatorItem *entry = new khcNavigatorItem(parent);

	if (entry->readKDElnk(filename))
	    appendList->append(entry);
	else
	    delete entry;
    }

    return true;
}


bool khcNavigatorWidget::processDir( const char *dirName, khcNavigatorItem *parent,  QList<khcNavigatorItem> *appendList)
{
    QString folderName;

    QDir dirDir( dirName, "*", 0, QDir::Dirs );

    if (!dirDir.exists()) return false;

    QStringList dirList = dirDir.entryList();
    QStringList::Iterator itDir;

    for ( itDir = dirList.begin(); !(*itDir).isNull(); ++itDir )
    {
	if ( (*itDir)[0] == '.' )
	    continue;


	QString filename = dirDir.path();
	filename += "/";
	filename += *itDir;

	QString dirFile = filename;
	dirFile += "/.directory";
	QString icon;
	
	if ( QFile::exists( dirFile ) )
	{
	    KSimpleConfig sc( dirFile, true );
            sc.setDesktopGroup();
	    folderName = sc.readEntry("Name");

	    //icon = sc.readEntry("MiniIcon");
	    //if (icon.isEmpty())
	    icon = "helpbook.png";
	}
	else
	{
	    folderName = *itDir;
	    icon = "helpbook.png";
	}
	
	khcNavigatorItem *dirItem = new khcNavigatorItem(parent, folderName, icon);
	appendList->append(dirItem);
	
	
	// read and append child items
	appendEntries(filename, dirItem, appendList);	
	processDir(filename, dirItem, appendList);
    }
    return true;
}


#include "khc_navigator.moc"
