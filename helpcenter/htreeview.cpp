#include "htreeview.h"

#include <qpixmap.h>
#include <qstring.h>
#include <kapp.h>

HTreeView::HTreeView(QWidget *parent, const char *name)
  : KTabCtl(parent,name)
{
  tree = new KTreeList(this);
  search = new QWidget(this);
  
  // add tree items:
  itemList.setAutoDelete(false);
  addTreeItems();
  tree->setSmoothScrolling(true);
  
  addTab(tree, "Contents");
  addTab(search, "Search");
  setBorder(false);
  show();

  connect(tree, SIGNAL(highlighted(int)),this,
		  SLOT(slotItemSelected(int)));
}

HTreeView::~HTreeView()
{
  delete tree;
  delete search;
}

void HTreeView::addTreeItems()
{
  QString pm_path = kapp->kde_datadir().copy() + "/khelpcenter/pics/";
  
  QPixmap *document_pm = new QPixmap(pm_path + "document.xpm");
  QPixmap *folder_pm = new QPixmap(pm_path + "folder.xpm");
	
  // contents page
  HTreeListItem *ti_intro = new HTreeListItem("Introduction", document_pm);
  tree->insertItem(ti_intro, -1, true);
  tree->setCurrentItem(tree->itemIndex(ti_intro));
  ti_intro->setURL(QString("file:" + kapp->kde_htmldir().copy()
						   +"/default/khelpcenter/main.html"));
  itemList.append(ti_intro);

  // tutorial folder
  HTreeListItem *ti_tutorials = new HTreeListItem("KDE Tutorials", folder_pm);
  tree->insertItem(ti_tutorials, -1, true);
  ti_tutorials->setURL(QString("file:" + kapp->kde_htmldir().copy()
						   +"/default/khelpcenter/tutorials/index.html"));
  itemList.append(ti_tutorials);

  // quickstart guide
  HTreeListItem *ti_tutorials_qs = new HTreeListItem("KDE quickstart guide", document_pm);
  ti_tutorials->appendChild(ti_tutorials_qs);
  ti_tutorials_qs->setURL(QString("file:" + kapp->kde_htmldir().copy()
								  +"/default/khelpcenter/tutorials/quickstart/index.html"));
  itemList.append(ti_tutorials_qs);
  
  // programming tutorial
  HTreeListItem *ti_tutorials_prog = new HTreeListItem("KDE programming", document_pm);
  ti_tutorials->appendChild(ti_tutorials_prog);
  ti_tutorials_prog->setURL(QString("file:" + kapp->kde_htmldir().copy()
									+"/default/khelpcenter/tutorials/programming/index.html"));
  itemList.append(ti_tutorials_prog);
  
  // app manual folder
  HTreeListItem *ti_manuals = new HTreeListItem("Application Manuals", document_pm);
  tree->insertItem(ti_manuals, -1, true);
  ti_manuals->setURL(QString("file:/cgi-bin/helpindex"));
  itemList.append(ti_manuals);

  // man folder
  HTreeListItem *ti_man = new HTreeListItem("Browse man pages", folder_pm);
  tree->insertItem(ti_man, -1, true);
  ti_man->setURL(QString("man:(index)"));
  itemList.append(ti_man);

  // man (1)
  HTreeListItem *ti_man_s1 = new HTreeListItem("(1) User commands", document_pm);
  ti_man->appendChild(ti_man_s1);

  ti_man_s1->setURL(QString("man:(1)"));
  itemList.append(ti_man_s1);
  
  // man(2)
  HTreeListItem *ti_man_s2 = new HTreeListItem("(2) System calls", document_pm);
  ti_man->appendChild(ti_man_s2);

  ti_man_s2->setURL(QString("man:(2)"));
  itemList.append(ti_man_s2);

  // man(3)
  HTreeListItem *ti_man_s3 = new HTreeListItem("(3) Subroutines", document_pm);
  ti_man->appendChild(ti_man_s3);

  ti_man_s3->setURL(QString("man:(3)"));
  itemList.append(ti_man_s3);

  // man(4)
  HTreeListItem *ti_man_s4 = new HTreeListItem("(4) Devices", document_pm);
  ti_man->appendChild(ti_man_s4);

  ti_man_s4->setURL(QString("man:(4)"));
  itemList.append(ti_man_s4);

  // man(5)
  HTreeListItem *ti_man_s5 = new HTreeListItem("(5) File Formats", document_pm);
  ti_man->appendChild(ti_man_s5);

  ti_man_s5->setURL(QString("man:(5)"));
  itemList.append(ti_man_s5);

  // man(6)
  HTreeListItem *ti_man_s6 = new HTreeListItem("(6) Games", document_pm);
  ti_man->appendChild(ti_man_s6);

  ti_man_s6->setURL(QString("man:(6)"));
  itemList.append(ti_man_s6);

  // man(7)
  HTreeListItem *ti_man_s7 = new HTreeListItem("(7) Miscellaneous", document_pm);
  ti_man->appendChild(ti_man_s7);

  ti_man_s7->setURL(QString("man:(7)"));
  itemList.append(ti_man_s7);

  // man(8)
  HTreeListItem *ti_man_s8 = new HTreeListItem("(8) Sys. Administration", document_pm);
  ti_man->appendChild(ti_man_s8);

  ti_man_s8->setURL(QString("man:(8)"));
  itemList.append(ti_man_s8);

  // man(9)
  HTreeListItem *ti_man_s9 = new HTreeListItem("(9) Kernel", document_pm);
  ti_man->appendChild(ti_man_s9);

  ti_man_s9->setURL(QString("man:(9)"));
  itemList.append(ti_man_s9);

  // man(n)
  HTreeListItem *ti_man_sn = new HTreeListItem("(n) New", document_pm);
  ti_man->appendChild(ti_man_sn);

  ti_man_sn->setURL(QString("man:(n)"));
  itemList.append(ti_man_sn);

  // info browser 
  HTreeListItem *ti_info = new HTreeListItem("Browse info pages", document_pm);
  tree->insertItem(ti_info, -1, true);

  ti_info->setURL(QString("file:/cgi-bin/info2html"));
  itemList.append(ti_info);

  // kde faq
  HTreeListItem *ti_faq = new HTreeListItem("KDE FAQ", document_pm);
  tree->insertItem(ti_faq, -1, true);

  ti_faq->setURL(QString("file:" + kapp->kde_htmldir().copy()
						   +"/default/khelpcenter/faq/index.html"));
  itemList.append(ti_faq);

  // kde links
  HTreeListItem *ti_links = new HTreeListItem("KDE related WWW links", document_pm);
  tree->insertItem(ti_links, -1, true);

  ti_links->setURL(QString("file:" + kapp->kde_htmldir().copy()
						   +"/default/khelpcenter/links.html"));
  itemList.append(ti_links);

  // kde contacts
  HTreeListItem *ti_contact = new HTreeListItem("Contact Information", document_pm);
  tree->insertItem(ti_contact, -1, true);

  ti_contact->setURL(QString("file:" + kapp->kde_htmldir().copy()
						   +"/default/khelpcenter/contact.html"));
  itemList.append(ti_contact);
}

void HTreeView::slotItemSelected(int index)
{
  HTreeListItem *item;
  KTreeListItem *currentItem;

  // get a pointer to the highlighted item
  currentItem = tree->getCurrentItem();

  // if currentItem has a child we collapse all other items and expand currentItem
  if (currentItem->hasChild())
	{
	  // collapse all visible items
	  for (int i = 0; i <= tree->visibleCount(); i++)
		tree->collapseItem(i);
	  
	  // expand currentItem and set is a current again
	  tree->expandItem(tree->itemIndex(currentItem));
	  tree->setCurrentItem(tree->itemIndex(currentItem));
	}

  // now find the highlighted item in our list
  for (item = itemList.first(); item != 0; item = itemList.next())
	{
	  if (item == tree->getCurrentItem())
		  break;
	}

  // if we found the item in our list and if it has a URL emit itemSelected
  if (item != 0 && item->getURL() != "")
	emit itemSelected(item->getURL());
}
