/**
 *  kcmhtmlsearch.cpp
 *
 *  Copyright (c) 2000 Matthias Hölzer-Klüpfel <hoelzer@kde.org>
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

#include <QLayout>
#include <kstandarddirs.h>
#include <klocale.h>
#include <kurllabel.h>
#include <kapplication.h>
#include <QCheckBox>
#include <QWhatsThis>
#include <kfiledialog.h>
#include <k3process.h>
#include "klangcombo.h"
#include <kurlrequester.h>
#include <klineedit.h>
#include <ktoolinvocation.h>
#include <kgenericfactory.h>

#include "kcmhtmlsearch.moc"

typedef KGenericFactory<KHTMLSearchConfig> KHTMLSearchConfigFactory;
K_EXPORT_COMPONENT_FACTORY(htmlsearch, KHTMLSearchConfigFactory("kcmhtmlsearch"))

KHTMLSearchConfig::KHTMLSearchConfig(QWidget *parent, const QStringList &)
  : KCModule(KHTMLSearchConfigFactory::componentData(), parent), indexProc(0)
{
  QVBoxLayout *vbox = new QVBoxLayout(this);
  vbox->setSpacing(5);


  QGroupBox *gb = new QGroupBox(i18n("ht://dig"), this);
  vbox->addWidget(gb);

  QGridLayout *grid = new QGridLayout(gb);
  grid->setSpacing(6);
  grid->setMargin(6);

  grid->addRowSpacing(0, gb->fontMetrics().lineSpacing());

  QLabel *l = new QLabel(i18n("The fulltext search feature makes use of the "
                  "ht://dig HTML search engine. "
                  "You can get ht://dig at the"), gb);
  l->setAlignment(QLabel::WordBreak);
  l->setMinimumSize(l->sizeHint());
  grid->addWidget(l, 1, 0, 1, 2 );
  QWhatsThis::add( gb, i18n( "Information about where to get the ht://dig package." ) );

  KUrlLabel *url = new KUrlLabel(gb);
  url->setURL("http://www.htdig.org");
  url->setText(i18n("ht://dig home page"));
  url->setAlignment(QLabel::AlignHCenter);
  grid->addWidget(url, 2, 0, 1, 2 );
  connect(url, SIGNAL(leftClickedUrl(const QString&)),
      this, SLOT(urlClicked(const QString&)));

  gb = new QGroupBox(i18n("Program Locations"), this);

  vbox->addWidget(gb);
  grid = new QGridLayout(gb);
  grid->setSpacing(6);
  grid->setMargin(6);
  grid->addRowSpacing(0, gb->fontMetrics().lineSpacing());

  htdigBin = new KUrlRequester(gb);
  l = new QLabel(i18n("ht&dig"),gb);
  l->setBuddy(htdigBin);
  grid->addWidget(l, 1,0);
  grid->addWidget(htdigBin, 1,1);
  connect(htdigBin->lineEdit(), SIGNAL(textChanged(const QString&)), this, SLOT(configChanged()));
  QString wtstr = i18n( "Enter the path to your htdig program here, e.g. /usr/local/bin/htdig" );
  QWhatsThis::add( htdigBin, wtstr );
  QWhatsThis::add( l, wtstr );

  htsearchBin = new KUrlRequester(gb);
  l = new QLabel(i18n("ht&search"),gb);
  l->setBuddy(htsearchBin);
  grid->addWidget(l, 2,0);
  grid->addWidget(htsearchBin, 2,1);
  connect(htsearchBin->lineEdit(), SIGNAL(textChanged(const QString&)), this, SLOT(configChanged()));
  wtstr = i18n( "Enter the path to your htsearch program here, e.g. /usr/local/bin/htsearch" );
  QWhatsThis::add( htsearchBin, wtstr );
  QWhatsThis::add( l, wtstr );

  htmergeBin = new KUrlRequester(gb);
  l = new QLabel(i18n("ht&merge"),gb);
  l->setBuddy(htmergeBin);
  grid->addWidget(l, 3,0);
  grid->addWidget(htmergeBin, 3,1);
  connect(htmergeBin->lineEdit(), SIGNAL(textChanged(const QString&)), this, SLOT(configChanged()));
  wtstr = i18n( "Enter the path to your htmerge program here, e.g. /usr/local/bin/htmerge" );
  QWhatsThis::add( htmergeBin, wtstr );
  QWhatsThis::add( l, wtstr );

  QHBoxLayout *hbox = new QHBoxLayout(vbox);

  gb = new QGroupBox(i18n("Scope"), this);
  hbox->addWidget(gb);
  QWhatsThis::add( gb, i18n( "Here you can select which parts of the documentation should be included in the fulltext search index. Available options are the KDE Help pages, the installed man pages, and the installed info pages. You can select any number of these." ) );

  QVBoxLayout *vvbox = new QVBoxLayout(gb);
  vvbox->setSpacing(2);
  vvbox->setMargin(6);
  vvbox->addSpacing(gb->fontMetrics().lineSpacing());

  indexKDE = new QCheckBox(i18n("&KDE help"), gb);
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

  gb = new QGroupBox(i18n("Additional Search Paths"), this);
  hbox->addWidget(gb);
  QWhatsThis::add( gb, i18n( "Here you can add additional paths to search for documentation. To add a path, click on the <em>Add...</em> button and select the folder from where additional documentation should be searched. You can remove folders by clicking on the <em>Delete</em> button." ) );

  grid = new QGridLayout(gb);
  grid->setSpacing(2);
  grid->setMargin(6);
  grid->addRowSpacing(0, gb->fontMetrics().lineSpacing());

  addButton = new QPushButton(i18n("Add..."), gb);
  grid->addWidget(addButton, 1,0);

  delButton = new QPushButton(i18n("Delete"), gb);
  grid->addWidget(delButton, 2,0);

  searchPaths = new KListWidget(gb);
  grid->addWidget(searchPaths, 1, 1,3, 1);
  grid->setRowStretch(2,2);

  gb = new QGroupBox(i18n("Language Settings"), this);
  vbox->addWidget(gb);
  QWhatsThis::add(gb, i18n("Here you can select the language you want to create the index for."));
  language = new KLanguageCombo(gb);
  l = new QLabel(i18n("&Language"),gb);
  l->setBuddy(language);
  vvbox = new QVBoxLayout(gb);
  vvbox->setSpacing(2);
  vvbox->setMargin(6);
  vvbox->addSpacing(gb->fontMetrics().lineSpacing());
  hbox = new QHBoxLayout(vvbox);
  hbox->setSpacing(6);
  hbox->addWidget(l);
  hbox->addWidget(language,1);
  hbox->addStretch(1);

  loadLanguages();

  vbox->addStretch(1);

  runButton = new QPushButton(i18n("Generate Index..."), this);
  QWhatsThis::add( runButton, i18n( "Click this button to generate the index for the fulltext search." ) );
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


void KHTMLSearchConfig::loadLanguages()
{
  // clear the list
  language->clear();

  // add all languages to the list
  QStringList langs = KGlobal::dirs()->findAllResources("locale",
							QLatin1String("*/entry.desktop"));
  langs.sort();

  for (QStringList::ConstIterator it = langs.begin(); it != langs.end(); ++it)
    {
      KSimpleConfig entry(*it);
      entry.setGroup(QLatin1String("KCM Locale"));
      QString name = entry.readEntry(QLatin1String("Name"), i18n("without name"));

      QString path = *it;
      int index = path.lastIndexOf('/');
      path = path.left(index);
      index = path.lastIndexOf('/');
      path = path.mid(index+1);
      language->insertLanguage(path, name);
    }
}


QString KHTMLSearchConfig::quickHelp() const
{
    return i18n( "<h1>Help Index</h1> This configuration module lets you configure the ht://dig engine which can be used for fulltext search in the KDE documentation as well as other system documentation like man and info pages." );
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
  htdigBin->lineEdit()->setText(config->readPathEntry("htdig", kapp->dirs()->findExe("htdig")));
  htsearchBin->lineEdit()->setText(config->readPathEntry("htsearch", kapp->dirs()->findExe("htsearch")));
  htmergeBin->lineEdit()->setText(config->readPathEntry("htmerge", kapp->dirs()->findExe("htmerge")));

  config->setGroup("Scope");
  indexKDE->setChecked(config->readEntry("KDE", true));
  indexMan->setChecked(config->readEntry("Man", false));
  indexInfo->setChecked(config->readEntry("Info", false));

  QStringList l = config->readPathEntry("Paths", QStringList());
  searchPaths->clear();
  QStringList::Iterator it;
  for (it=l.begin(); it != l.end(); ++it)
    searchPaths->insertItem(*it);

  config->setGroup("Locale");
  QString lang = config->readEntry("Search Language", KGlobal::locale()->language());
  language->setCurrentItem(lang);

  emit changed(false);
}


void KHTMLSearchConfig::save()
{
  KConfig *config= new KConfig("khelpcenterrc", false);

  config->setGroup("htdig");
  config->writePathEntry("htdig", htdigBin->lineEdit()->text());
  config->writePathEntry("htsearch", htsearchBin->lineEdit()->text());
  config->writePathEntry("htmerge", htmergeBin->lineEdit()->text());

  config->setGroup("Scope");
  config->writeEntry("KDE", indexKDE->isChecked());
  config->writeEntry("Man", indexMan->isChecked());
  config->writeEntry("Info", indexInfo->isChecked());

  QStringList l;
  for (uint i=0; i<searchPaths->count(); ++i)
    l.append(searchPaths->text(i));
  config->writePathEntry("Paths", l);

  config->setGroup("Locale");
  config->writeEntry("Search Language", language->currentTag());

  config->sync();
  delete config;

  emit changed(false);
}


void KHTMLSearchConfig::defaults()
{
  htdigBin->lineEdit()->setText(kapp->dirs()->findExe("htdig"));
  htsearchBin->lineEdit()->setText(kapp->dirs()->findExe("htsearch"));
  htmergeBin->lineEdit()->setText(kapp->dirs()->findExe("htmerge"));

  indexKDE->setChecked(true);
  indexMan->setChecked(false);
  indexInfo->setChecked(false);

  searchPaths->clear();

  language->setCurrentItem(KGlobal::locale()->language());

  emit changed(true);
}


void KHTMLSearchConfig::urlClicked(const QString &url)
{
  KToolInvocation::invokeBrowser(url);
}


void KHTMLSearchConfig::generateIndex()
{
  save();

  QString exe = kapp->dirs()->findExe("khtmlindex");
  if (exe.isEmpty())
    return;

  delete indexProc;

  indexProc = new K3Process;
  *indexProc << exe << "--lang" << language->currentTag();

  connect(indexProc, SIGNAL(processExited(K3Process *)),
      this, SLOT(indexTerminated(K3Process *)));

  runButton->setEnabled(false);

  indexProc->start();
}


void KHTMLSearchConfig::indexTerminated(K3Process *)
{
  runButton->setEnabled(true);
}
