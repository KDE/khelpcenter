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
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <qlayout.h>
#include <qvbox.h>
#include <qbuttongroup.h>
#include <qgroupbox.h>
#include <qfontmetrics.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qframe.h>
#include <qpushbutton.h>
#include <qwhatsthis.h>

#include <kconfig.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kstandarddirs.h>
#include <klocale.h>
#include <kurllabel.h>
#include <kapplication.h>
#include <klistbox.h>
#include <kfiledialog.h>
#include <kprocess.h>
#include <kurlrequester.h>
#include <klineedit.h>

#include "htmlsearchconfig.h"
#include "htmlsearchconfig.moc"

namespace KHC {

HtmlSearchConfig::HtmlSearchConfig(QWidget *parent, const char *name)
  : QWidget(parent, name)
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
  l->setMinimumSize(l->sizeHint());
  grid->addMultiCellWidget(l, 1, 1, 0, 1);
  QWhatsThis::add( gb, i18n( "Information about where to get the ht://dig package." ) );

  KURLLabel *url = new KURLLabel(gb);
  url->setURL("http://www.htdig.org");
  url->setText(i18n("ht://dig home page"));
  url->setAlignment(QLabel::AlignHCenter);
  grid->addMultiCellWidget(url, 2,2, 0, 1);
  connect(url, SIGNAL(leftClickedURL(const QString&)),
      this, SLOT(urlClicked(const QString&)));

  gb = new QGroupBox(i18n("Program Locations"), this);

  vbox->addWidget(gb);
  grid = new QGridLayout(gb, 4,2, 6,6);
  grid->addRowSpacing(0, gb->fontMetrics().lineSpacing());

  mHtsearchUrl = new KURLRequester(gb);
  l = new QLabel(mHtsearchUrl, i18n("htsearch:"), gb);
  l->setBuddy( mHtsearchUrl );
  grid->addWidget(l, 1,0);
  grid->addWidget(mHtsearchUrl, 1,1);
  connect(mHtsearchUrl->lineEdit(), SIGNAL(textChanged(const QString&)), this, SLOT(configChanged()));
  QString wtstr = i18n( "Enter the URL of the htsearch CGI program." );
  QWhatsThis::add( mHtsearchUrl, wtstr );
  QWhatsThis::add( l, wtstr );

  mIndexerBin = new KURLRequester(gb);
  l = new QLabel(mIndexerBin, i18n("Indexer:"), gb);
  l->setBuddy( mIndexerBin );
  grid->addWidget(l, 2,0);
  grid->addWidget(mIndexerBin, 2,1);
  connect(mIndexerBin->lineEdit(), SIGNAL(textChanged(const QString&)), this, SLOT(configChanged()));
  wtstr = i18n( "Enter the path to your htdig indexer program here." );
  QWhatsThis::add( mIndexerBin, wtstr );
  QWhatsThis::add( l, wtstr );

  mDbDir = new KURLRequester(gb);
  l = new QLabel(mDbDir, i18n("htdig database:"), gb);
  l->setBuddy( mDbDir );
  grid->addWidget(l, 3,0);
  grid->addWidget(mDbDir, 3,1);
  connect(mDbDir->lineEdit(), SIGNAL(textChanged(const QString&)), this, SLOT(configChanged()));
  wtstr = i18n( "Enter the path to the htdig database directory." );
  QWhatsThis::add( mDbDir, wtstr );
  QWhatsThis::add( l, wtstr );
}

HtmlSearchConfig::~HtmlSearchConfig()
{
  kdDebug() << "~HtmlSearchConfig()" << endl;
}

void HtmlSearchConfig::configChanged()
{
  emit changed(true);
}

void HtmlSearchConfig::load( KConfig *config )
{
  config->setGroup("htdig");

  mHtsearchUrl->lineEdit()->setText(config->readEntry("htsearch", kapp->dirs()->findExe("htsearch")));
  mIndexerBin->lineEdit()->setText(config->readEntry("indexer"));
  mDbDir->lineEdit()->setText(config->readEntry("dbdir", "/opt/www/htdig/db/" ) );

  emit changed(false);
}

void HtmlSearchConfig::save( KConfig *config )
{
  config->setGroup("htdig");

  config->writeEntry("htsearch", mHtsearchUrl->lineEdit()->text());
  config->writeEntry("indexer", mIndexerBin->lineEdit()->text());
  config->writeEntry("dbdir", mDbDir->lineEdit()->text());

  emit changed(false);
}

void HtmlSearchConfig::defaults()
{
}

void HtmlSearchConfig::urlClicked(const QString &url)
{
  kapp->invokeBrowser(url);
}

} // End namespace KHC
// vim:ts=2:sw=2:et
