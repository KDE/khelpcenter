/*
 *  htabview.cpp - part of the KDE Help Center
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

#include "htabview.h"
#include "htreelistitem.h"
#include "indexwidget.h"
#include "searchwidget.h"

#include <qdir.h>
#include <qfile.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qlabel.h>
#include <qmessagebox.h>

#include <kapp.h>
#include <ksimpleconfig.h>	

HTabView::HTabView(QWidget *parent, const char *name)
  : KTabCtl(parent,name)
{
  setupContentsTab();
  setupIndexTab();
  setupSearchTab();

  connect(this, SIGNAL(tabSelected(int)),this,
		  SLOT(slotTabSelected(int)));

  setBorder(false);

  buildTree();
}

HTabView::~HTabView()
{
  delete tree;
  delete search;
}

void HTabView::setupContentsTab()
{
  tree = new KTreeList(this);

  tree->setSmoothScrolling(true);
  tree->setTreeDrawing(false);
  tree->setExpandButtonDrawing(false);
  tree->setIndentSpacing(0);

  connect(tree, SIGNAL(highlighted(int)),this,
		  SLOT(slotItemSelected(int)));
  connect(tree, SIGNAL(selected(int)),this,
		  SLOT(slotItemSelected(int)));
  
  addTab(tree, "Contents");
}

void HTabView::setupIndexTab()
{
  index = new IndexWidget(this);

  addTab(index, "Index");
}

void HTabView::setupSearchTab()
{
  search = new SearchWidget(this);
  
  connect(search, SIGNAL(matchSelected(QString)),this,
		  SLOT(slotURLSelected(QString)));

  addTab(search, "Search");
}

void HTabView::buildTree()
{
  // introduction document
  HTreeListItem *ti_intro = new HTreeListItem("Introduction", "helpdoc.xpm");
  ti_intro->setURL(QString("file:" + kapp->kde_htmldir() +"/default/khelpcenter/main.html"));
  ti_intro->insertInTree(tree, 0);
  staticItems.append(ti_intro);
  
  // set the introduction page as current document
  tree->setCurrentItem(tree->itemIndex(ti_intro));

  // application manuals
  HTreeListItem *ti_manual = new HTreeListItem("Application manuals", "helpbook.xpm");
  ti_manual->insertInTree(tree, 0);
  staticItems.append(ti_manual);

  // fill the application manual subtree
  buildManualSubTree(ti_manual);

  // unix man pages
  HTreeListItem *ti_man = new HTreeListItem("Unix manual pages", "helpbook.xpm");
  ti_man->setURL(QString("man:/(index)"));
  ti_man->insertInTree(tree, 0);
  staticItems.append(ti_man);

  // fill the man pages subtree
  buildManSubTree(ti_man);

  // info browser 
  HTreeListItem *ti_info = new HTreeListItem("Browse info pages", "helpdoc.xpm");
  ti_info->setURL(QString("file:/cgi-bin/info2html"));
  ti_info->insertInTree(tree,0);
  staticItems.append(ti_info);

  // scan plugin dir for plugins
  insertPlugins();

  // kde links
  HTreeListItem *ti_links = new HTreeListItem("KDE related WWW links", "helpdoc.xpm");
  ti_links->setURL(QString("file:" + kapp->kde_htmldir()
						   +"/default/khelpcenter/links.html"));
  ti_links->insertInTree(tree,0);
  staticItems.append(ti_links);

  // kde contacts
  HTreeListItem *ti_contact = new HTreeListItem("Contact Information", "helpdoc.xpm");
  ti_contact->setURL(QString("file:" + kapp->kde_htmldir().copy()
						   +"/default/khelpcenter/contact.html"));
  ti_contact->insertInTree(tree,0);
  staticItems.append(ti_contact);  
}

void HTabView::clearTree()
{
  tree->clear();

  while(!staticItems.isEmpty())
	staticItems.removeFirst();

  while(!manualItems.isEmpty())
	manualItems.removeFirst();

  while(!pluginItems.isEmpty())
	  pluginItems.removeFirst();
}

void HTabView::buildManSubTree(HTreeListItem *parent)
{
  // man (1)
  HTreeListItem *ti_man_s1 = new HTreeListItem("(1) User commands", "helpdoc.xpm");
  ti_man_s1->setURL(QString("man:/(1)"));
  ti_man_s1->insertInTree(tree, parent);
  staticItems.append(ti_man_s1);
  
  // man(2)
  HTreeListItem *ti_man_s2 = new HTreeListItem("(2) System calls", "helpdoc.xpm");
  ti_man_s2->setURL(QString("man:/(2)"));
  ti_man_s2->insertInTree(tree, parent);
  staticItems.append(ti_man_s2);

  // man(3)
  HTreeListItem *ti_man_s3 = new HTreeListItem("(3) Subroutines", "helpdoc.xpm");
  ti_man_s3->setURL(QString("man:/(3)"));
  ti_man_s3->insertInTree(tree, parent);
  staticItems.append(ti_man_s3);

  // man(4)
  HTreeListItem *ti_man_s4 = new HTreeListItem("(4) Devices", "helpdoc.xpm");
  ti_man_s4->setURL(QString("man:/(4)"));
  ti_man_s4->insertInTree(tree, parent);
  staticItems.append(ti_man_s4);

  // man(5)
  HTreeListItem *ti_man_s5 = new HTreeListItem("(5) File Formats", "helpdoc.xpm");
  ti_man_s5->setURL(QString("man:/(5)"));
  ti_man_s5->insertInTree(tree, parent);
  staticItems.append(ti_man_s5);

  // man(6)
  HTreeListItem *ti_man_s6 = new HTreeListItem("(6) Games", "helpdoc.xpm");
  ti_man_s6->setURL(QString("man:/(6)"));
  ti_man_s6->insertInTree(tree, parent);
  staticItems.append(ti_man_s6);

  // man(7)
  HTreeListItem *ti_man_s7 = new HTreeListItem("(7) Miscellaneous", "helpdoc.xpm");
  ti_man_s7->setURL(QString("man:/(7)"));
  ti_man_s7->insertInTree(tree, parent);
  staticItems.append(ti_man_s7);

  // man(8)
  HTreeListItem *ti_man_s8 = new HTreeListItem("(8) Sys. Administration", "helpdoc.xpm");
  ti_man_s8->setURL(QString("man:/(8)"));
  ti_man_s8->insertInTree(tree, parent);
  staticItems.append(ti_man_s8);

  // man(9)
  HTreeListItem *ti_man_s9 = new HTreeListItem("(9) Kernel", "helpdoc.xpm");
  ti_man_s9->setURL(QString("man:/(9)"));
  ti_man_s9->insertInTree(tree, parent);
  staticItems.append(ti_man_s9);

  // man(n)
  HTreeListItem *ti_man_sn = new HTreeListItem("(n) New", "helpdoc.xpm");
  ti_man_sn->setURL(QString("man:/(n)"));
  ti_man_sn->insertInTree(tree, parent);
  staticItems.append(ti_man_sn);
}

void HTabView::buildManualSubTree(HTreeListItem *parent)
{
  // System applications
  QString appPath = kapp->kde_appsdir();
  processDir(appPath, parent, &manualItems);
  appendEntries(appPath, parent, &manualItems);

  /*
  // User applications
  appPath = KApplication::localkdedir() + "/share/applnk";
  processDir(appPath, userManualTop);
  appendEntries(appPath, userManualTop);
  */
}

void HTabView::insertPlugins()
{
  // Scan plugin dir
  QString path = kapp->kde_datadir() + "/khelpcenter/plugins";
  processDir(path, 0, &pluginItems);
  appendEntries(path, 0, &pluginItems);
}

void HTabView::slotReloadTree()
{
  emit setBussy(true);
  clearTree();
  buildTree();
  emit setBussy(false);
}

void HTabView::slotTabSelected(int id)
{
  printf("khelpcenter: tab %d selected.\n", id); fflush(stdout);
  if (id == 1)
	index->tabSelected();
  else if (id == 2)
	search->tabSelected();
}

void HTabView::slotURLSelected(QString url)
{
  emit itemSelected(url);
}

void HTabView::slotItemSelected(int index)
{
  HTreeListItem *item;
  KTreeListItem *currentItem;

  // get a pointer to the highlighted item
  currentItem = tree->getCurrentItem();

  // expand currentItem and collapse all items that are no parents of currentItemms
  tree->expandItem(tree->itemIndex(currentItem));

  // build a list of parents
  QList<KTreeListItem> parentList;
  parentList.append(currentItem);

  KTreeListItem *pitem = currentItem;
  while (pitem->hasParent())
	{
	  pitem = pitem->getParent();
	  if (pitem == 0)
		break;
	  parentList.append(pitem);
	}

  // collapse all visible items that are not in parentList
  for (int i = 0; i <= tree->visibleCount(); i++)
	{
	  bool isParent = false;

	  for (pitem = parentList.first(); pitem != 0; pitem = parentList.next())
		{
		  if (tree->itemAt(i) == pitem)
			{
			  isParent = true;
			  break;
			}
		}
	  if (isParent)
		continue;
	  tree->collapseItem(i);
	}

  // set currentItem
  tree->setCurrentItem(tree->itemIndex(currentItem));
  
  // now find the highlighted item in our lists
  for (item = staticItems.first(); item != 0; item = staticItems.next())
	{
	  if (item == tree->getCurrentItem())
		{
		  if (item->getURL() != "")
			emit itemSelected(item->getURL());
		  return;
		}
	}
  for (item = manualItems.first(); item != 0; item = manualItems.next())
	{
	  if (item == tree->getCurrentItem())
		{
		  if (item->getURL() != "")
			emit itemSelected(item->getURL());
		  return;
		}
	}
  for (item = pluginItems.first(); item != 0; item = pluginItems.next())
	{
	  if (item == tree->getCurrentItem())
		{
		  if (item->getURL() != "")
			emit itemSelected(item->getURL());
		  return;
		}
	}
}

bool HTabView::appendEntries(const char *dirName, HTreeListItem *parent, QList<HTreeListItem> *appendList)
{
  QDir fileDir(dirName, "*.kdelnk", 0, QDir::Files | QDir::Hidden | QDir::Readable);

  if (!fileDir.exists())
	return false;

  const QStrList *fileList = fileDir.entryList();
  QStrListIterator itFile( *fileList );

  for ( ; itFile.current(); ++itFile )
	{
	  QString filename = dirName;
	  filename += "/";
	  filename += itFile.current();

	  HTreeListItem *entry = new HTreeListItem;

	  if (entry->readKDElnk(filename))
		{
		  entry->insertInTree(tree, parent);
		  appendList->append(entry);
		}
	  else
		delete entry;
	}
  
  return true;
}


bool HTabView::processDir( const char *dirName, HTreeListItem *parent,  QList<HTreeListItem> *appendList)
{
  QString folderName;

  QDir dirDir( dirName, "*", 0, QDir::Dirs );

  if (!dirDir.exists()) return false;
  
  const QStrList *dirList = dirDir.entryList();
  QStrListIterator itDir( *dirList );
  
  for ( ; itDir.current(); ++itDir )
	{
	  if ( itDir.current()[0] == '.' )
		continue;


	  QString filename = dirDir.path();
	  filename += "/";
	  filename += itDir.current();

	  if (!containsDocuments(filename))
		continue;

	  QString dirFile = filename;
	  dirFile += "/.directory";
	  QString icon;
	  
	  if ( QFile::exists( dirFile ) )
		{
		  KSimpleConfig sc( dirFile, true );
		  sc.setGroup( "KDE Desktop Entry" );
		  folderName = sc.readEntry("Name");

		  icon = //sc.readEntry("MiniIcon");
			//if (icon.isEmpty())
			icon = "helpbook.xpm";
		}
	  else
		{
		  folderName = itDir.current();
		  icon = "helpbook.xpm";
		}
	  
	  HTreeListItem *dirItem = new HTreeListItem(folderName, icon);
	  dirItem->insertInTree(tree, parent);
	  appendList->append(dirItem);
	  
	  
	  // read and append child items
	  appendEntries(filename, dirItem, appendList);	
	  processDir(filename, dirItem, appendList);
	}
  return true;
}

bool HTabView::containsDocuments(QString dir)
{
  QDir fileDir(dir, "*.kdelnk", 0, QDir::Files | QDir::Hidden | QDir::Readable);

  if (!fileDir.exists())
	return false;

  // does dir contain files
  if (fileDir.count() > 0)
	{
	  // does at least one kdelnk contain a docPath
	  const QStrList *fileList = fileDir.entryList();
	  QStrListIterator itFile( *fileList );
	  for ( ; itFile.current(); ++itFile )
		{
		  QString filename = dir;
		  filename += "/";
		  filename += itFile.current();

		  KSimpleConfig sc( filename, true );
		  sc.setGroup( "KDE Desktop Entry" );
		  QString docpath = sc.readEntry("DocPath");
		  
		  if (!docpath.isEmpty())
			return true;
		}
	}

  // does it contain subdirs
  QDir dirDir( dir, "*", 0, QDir::Dirs );
  if (dirDir.count() < 1)
	return false;

  // go through subdirs and search for files
  const QStrList *dirList = dirDir.entryList();
  QStrListIterator itDir( *dirList );
  
  for (; itDir.current(); ++itDir)
	{
	  if ( itDir.current()[0] == '.' )
		continue;

	  if (containsDocuments(dir + "/" + itDir.current()))
		  return true;
	}
  return false;
}
