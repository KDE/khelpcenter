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

#include "htmlsearchconfig.h"

#include <QLayout>
//Added by qt3to4:
#include <QLabel>
#include <QVBoxLayout>
#include <QGridLayout>

#include <kdebug.h>
#include <kstandarddirs.h>
#include <klocale.h>
#include <kurllabel.h>
#include <kapplication.h>
#include <kfiledialog.h>
#include <kurlrequester.h>
#include <klineedit.h>
#include <ktoolinvocation.h>

namespace KHC {

HtmlSearchConfig::HtmlSearchConfig(QWidget *parent, const char *name)
  : QWidget(parent)
{
  setObjectName( name );

  QVBoxLayout *vbox = new QVBoxLayout(this);
  vbox->setMargin( 5 );


  QGroupBox *gb = new QGroupBox(i18n("ht://dig"), this);
  vbox->addWidget(gb);

  QGridLayout *grid = new QGridLayout(gb);
  grid->setMargin( 6 );
  grid->setSpacing( 6 );

  grid->addItem( new QSpacerItem( 0, gb->fontMetrics().lineSpacing() ), 0, 0 );

  QLabel *l = new QLabel(i18n("The fulltext search feature makes use of the "
                  "ht://dig HTML search engine. "
                  "You can get ht://dig at the"), gb);
  l->setMinimumSize(l->sizeHint());
  grid->addWidget(l, 1, 1, 0, 1);
  gb->setWhatsThis( i18n( "Information about where to get the ht://dig package." ) );

  KUrlLabel *url = new KUrlLabel(gb);
  url->setUrl(QLatin1String("http://www.htdig.org"));
  url->setText(i18n("ht://dig home page"));
  url->setAlignment(Qt::AlignHCenter);
  grid->addWidget(url, 2,2, 0, 1);
  connect(url, SIGNAL(leftClickedUrl(const QString&)),
      this, SLOT(urlClicked(const QString&)));

  gb = new QGroupBox(i18n("Program Locations"), this);

  vbox->addWidget(gb);
  grid = new QGridLayout(gb);
  grid->setMargin( 6 );
  grid->setSpacing( 6 );
  grid->addItem( new QSpacerItem( 0, gb->fontMetrics().lineSpacing() ), 0, 0 );

  mHtsearchUrl = new KUrlRequester(gb);
  l = new QLabel(i18n("htsearch:"), gb);
  l->setBuddy( mHtsearchUrl );
  grid->addWidget(l, 1,0);
  grid->addWidget(mHtsearchUrl, 1,1);
  connect( mHtsearchUrl->lineEdit(), SIGNAL( textChanged( const QString & ) ),
           SIGNAL( changed() ) );
  QString wtstr = i18n( "Enter the URL of the htsearch CGI program." );
  mHtsearchUrl->setWhatsThis( wtstr );
  l->setWhatsThis( wtstr );

  mIndexerBin = new KUrlRequester(gb);
  l = new QLabel(i18n("Indexer:"), gb);
  l->setBuddy( mIndexerBin );
  grid->addWidget(l, 2,0);
  grid->addWidget(mIndexerBin, 2,1);
  connect( mIndexerBin->lineEdit(), SIGNAL( textChanged( const QString & ) ),
           SIGNAL( changed() ) );
  wtstr = i18n( "Enter the path to your htdig indexer program here." );
  mIndexerBin->setWhatsThis( wtstr );
  l->setWhatsThis( wtstr );

  mDbDir = new KUrlRequester(gb);
  mDbDir->setMode( KFile::Directory | KFile::LocalOnly );
  l = new QLabel(i18n("htdig database:"), gb);
  l->setBuddy( mDbDir );
  grid->addWidget(l, 3,0);
  grid->addWidget(mDbDir, 3,1);
  connect( mDbDir->lineEdit(), SIGNAL( textChanged( const QString & ) ),
           SIGNAL( changed() ) );
  wtstr = i18n( "Enter the path to the htdig database folder." );
  mDbDir->setWhatsThis( wtstr );
  l->setWhatsThis( wtstr );
}

HtmlSearchConfig::~HtmlSearchConfig()
{
  kDebug() << "~HtmlSearchConfig()";
}

void HtmlSearchConfig::makeReadOnly()
{
    mHtsearchUrl->setEnabled( false );
    mIndexerBin->setEnabled( false );
    mDbDir->setEnabled( false );
}

void HtmlSearchConfig::load( KConfig *config )
{
  config->setGroup("htdig");

  mHtsearchUrl->lineEdit()->setText(config->readPathEntry("htsearch", KGlobal::mainComponent().dirs()->findExe("htsearch")));
  mIndexerBin->lineEdit()->setText(config->readPathEntry("indexer"));
  mDbDir->lineEdit()->setText(config->readPathEntry("dbdir", "/opt/www/htdig/db/" ) );
}

void HtmlSearchConfig::save( KConfig *config )
{
  config->setGroup("htdig");

  config->writePathEntry("htsearch", mHtsearchUrl->lineEdit()->text());
  config->writePathEntry("indexer", mIndexerBin->lineEdit()->text());
  config->writePathEntry("dbdir", mDbDir->lineEdit()->text());
}

void HtmlSearchConfig::defaults()
{
    mHtsearchUrl->lineEdit()->setText(KGlobal::mainComponent().dirs()->findExe("htsearch"));
    mIndexerBin->lineEdit()->setText("");
    mDbDir->lineEdit()->setText(QLatin1String("/opt/www/htdig/db/") );
}

void HtmlSearchConfig::urlClicked(const QString &url)
{
  KToolInvocation::invokeBrowser(url);
}

} // End namespace KHC
// vim:ts=2:sw=2:et

#include "htmlsearchconfig.moc"

