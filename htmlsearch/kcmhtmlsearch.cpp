/**
 *  kcmhtmlsearch.cpp
 *
 *  Copyright (c) 2000 Matthias H�lzer-Kl�pfel <hoelzer@kde.org>
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

#include <qlayout.h>
#include <qvbox.h>
#include <qbuttongroup.h>
#include <kconfig.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <klocale.h>
#include <qgroupbox.h>
#include <kurllabel.h>
#include <qfontmetrics.h>
#include <qlineedit.h>
#include <kapp.h>
#include <qcheckbox.h>
#include <qframe.h>
#include <qpushbutton.h>
#include <klistbox.h>
#include <kfiledialog.h>
#include <kprocess.h>


#include "kcmhtmlsearch.moc"


KHTMLSearchConfig::KHTMLSearchConfig(QWidget *parent, const char *name)
  : KCModule(parent, name), indexProc(0)
{
  QVBoxLayout *vbox = new QVBoxLayout(this, 5);


  QGroupBox *gb = new QGroupBox(i18n("ht://dig"), this);
  vbox->addWidget(gb);

  QGridLayout *grid = new QGridLayout(gb, 3,2, 6,6);

  grid->addRowSpacing(0, gb->fontMetrics().lineSpacing());

  QLabel *l = new QLabel(i18n("The fulltext search feature makes use of the "
			      "ht://dig HTML search engine. "
			      "You can get ht://dig at the"), gb);
  l->setAlignment(QLabel::WordBreak);
  grid->addMultiCellWidget(l, 1, 1, 0, 1);

  KURLLabel *url = new KURLLabel(gb);
  url->setURL("http://www.htdig.org");
  url->setText(i18n("ht://dig home page"));
  url->setAlignment(QLabel::AlignHCenter);
  grid->addMultiCellWidget(url, 2,2, 0, 1);
  connect(url, SIGNAL(leftClickedURL(const QString&)),
	  this, SLOT(urlClicked(const QString&)));

  gb = new QGroupBox(i18n("Program locations"), this);

  vbox->addWidget(gb);
  grid = new QGridLayout(gb, 4,2, 6,6);
  grid->addRowSpacing(0, gb->fontMetrics().lineSpacing());
  
  htdigBin = new QLineEdit(gb);
  l = new QLabel(htdigBin, i18n("ht&dig"), gb);
  grid->addWidget(l, 1,0);
  grid->addWidget(htdigBin, 1,1);
  connect(htdigBin, SIGNAL(textChanged(const QString&)), this, SLOT(configChanged()));

  htsearchBin = new QLineEdit(gb);
  l = new QLabel(htdigBin, i18n("ht&search"), gb);
  grid->addWidget(l, 2,0);
  grid->addWidget(htsearchBin, 2,1);
  connect(htsearchBin, SIGNAL(textChanged(const QString&)), this, SLOT(configChanged()));


  QHBoxLayout *hbox = new QHBoxLayout(vbox);

  gb = new QGroupBox(i18n("Scope"), this);
  hbox->addWidget(gb);

  QVBoxLayout *vvbox = new QVBoxLayout(gb, 6,2);
  vvbox->addSpacing(gb->fontMetrics().lineSpacing());
  
  indexKDE = new QCheckBox(i18n("&KDE Help"), gb);
  vvbox->addWidget(indexKDE);
  connect(indexKDE, SIGNAL(clicked()), this, SLOT(configChanged()));

  indexMan = new QCheckBox(i18n("&Man pages"), gb);
  vvbox->addWidget(indexMan);
  indexMan->setEnabled(false),
  connect(indexMan, SIGNAL(clicked()), this, SLOT(configChanged()));

  indexInfo = new QCheckBox(i18n("&Info pages"), gb);
  vvbox->addWidget(indexInfo);
  indexInfo->setEnabled(false);
  connect(indexInfo, SIGNAL(clicked()), this, SLOT(configChanged()));

  gb = new QGroupBox(i18n("Additional search paths"), this);
  hbox->addWidget(gb);
  
  grid = new QGridLayout(gb, 4,3, 6,2);
  grid->addRowSpacing(0, gb->fontMetrics().lineSpacing());
  
  addButton = new QPushButton(i18n("Add..."), gb);
  grid->addWidget(addButton, 1,0);
  
  delButton = new QPushButton(i18n("Delete"), gb);
  grid->addWidget(delButton, 2,0);

  searchPaths = new KListBox(gb);
  grid->addMultiCellWidget(searchPaths, 1,3, 1,1);
  grid->setRowStretch(2,2);

  vbox->addStretch(1);

  runButton = new QPushButton(i18n("Generate index..."), this);
  runButton->setFixedSize(runButton->sizeHint());
  vbox->addWidget(runButton, AlignRight);
  connect(runButton, SIGNAL(clicked()), this, SLOT(generateIndex()));

  connect(addButton, SIGNAL(clicked()), this, SLOT(addClicked()));
  connect(delButton, SIGNAL(clicked()), this, SLOT(delClicked()));
  connect(searchPaths, SIGNAL(highlighted(const QString &)), 
	  this, SLOT(pathSelected(const QString &)));

  checkButtons();

  load();
}


void KHTMLSearchConfig::pathSelected(const QString &)
{
  checkButtons();
}


void KHTMLSearchConfig::checkButtons()
{

  delButton->setEnabled(searchPaths->currentItem() >= 0);
}


void KHTMLSearchConfig::addClicked()
{
  QString dir = KFileDialog::getExistingDirectory();

  if (!dir.isEmpty())
    {
      for (uint i=0; i<searchPaths->count(); ++i)
	if (searchPaths->text(i) == dir)
	  return;
      searchPaths->insertItem(dir);
      configChanged();
    }
}


void KHTMLSearchConfig::delClicked()
{
  searchPaths->removeItem(searchPaths->currentItem());
  checkButtons();
  configChanged();
}


KHTMLSearchConfig::~KHTMLSearchConfig() 
{
}


void KHTMLSearchConfig::configChanged()
{
  emit changed(true);
}


void KHTMLSearchConfig::load()
{
  KConfig *config = new KConfig("khelpcenterrc", true);
  
  config->setGroup("htdig");
  htdigBin->setText(config->readEntry("htdig", kapp->dirs()->findExe("htdig")));
  htsearchBin->setText(config->readEntry("htdig", kapp->dirs()->findExe("htsearch")));
  
  config->setGroup("Scope");
  indexKDE->setChecked(config->readBoolEntry("KDE", true));
  indexMan->setChecked(config->readBoolEntry("Man", false));
  indexInfo->setChecked(config->readBoolEntry("Info", false));
  
  QStringList l = config->readListEntry("Paths");
  searchPaths->clear();
  QStringList::Iterator it;
  for (it=l.begin(); it != l.end(); ++it)
    searchPaths->insertItem(*it);

  emit changed(false);
}


void KHTMLSearchConfig::save()
{
  KConfig *config= new KConfig("khelpcenterrc", false);

  config->setGroup("htdig");
  config->writeEntry("htdig", htdigBin->text());
  config->writeEntry("htsearch", htsearchBin->text());
  
  config->setGroup("Scope");
  config->writeEntry("KDE", indexKDE->isChecked());
  config->writeEntry("Man", indexMan->isChecked());
  config->writeEntry("Info", indexInfo->isChecked());

  QStringList l;
  for (uint i=0; i<searchPaths->count(); ++i)
    l.append(searchPaths->text(i));
  config->writeEntry("Paths", l);

  config->sync();
  delete config;

  emit changed(false);
}


void KHTMLSearchConfig::defaults()
{
  htdigBin->setText(kapp->dirs()->findExe("htdig"));
  htsearchBin->setText(kapp->dirs()->findExe("htsearch"));
  
  indexKDE->setChecked(true);
  indexMan->setChecked(false);
  indexInfo->setChecked(false);

  searchPaths->clear();

  emit changed(true);
}


void KHTMLSearchConfig::urlClicked(const QString &url)
{
  kapp->invokeBrowser(url);
}


void KHTMLSearchConfig::generateIndex()
{
  save();
  
  QString exe = kapp->dirs()->findExe("khtmlindex");
  if (exe.isEmpty())
    return;

  delete indexProc;

  indexProc = new KProcess;
  *indexProc << exe;

  connect(indexProc, SIGNAL(processExited(KProcess *)), 
	  this, SLOT(indexTerminated(KProcess *)));
  
  runButton->setEnabled(false);

  indexProc->start();
}


void KHTMLSearchConfig::indexTerminated(KProcess *)
{
  runButton->setEnabled(true);
}


extern "C"
{
  KCModule *create_htmlsearch(QWidget *parent, const char *name)
  {
    KGlobal::locale()->insertCatalogue("kcmhtmlsearch");
    return new KHTMLSearchConfig(parent, name);
  };
}
