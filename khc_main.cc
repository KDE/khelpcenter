 /*
 *  khc_main.cpp - part of the KDE Help Center
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

#include <stdlib.h>

#include <kapp.h>
#include <dcopclient.h>
#include <kglobal.h>
#include <klocale.h>
#include <kstddirs.h>
#include <kcmdlineargs.h>

#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>

#include "khc_main.h"

#include <kaboutdata.h>
#include <kdebug.h>

#include "version.h"
#include <khtml_part.h>
#include <qlayout.h>
#include <kaction.h>

KHMainWindow::KHMainWindow(const KURL &url)
    : KMainWindow(0, "khmainwindow")
{
    doc = new KHTMLPart( this, 0,
                         this, 0, KHTMLPart::BrowserViewGUI );
    QWidget::connect(doc, SIGNAL(setWindowCaption(const QString &)),
                     doc->widget(), SLOT(setCaption(const QString &)));

    setCentralWidget( doc->widget() );
    setGeometry(366, 0, 640, 800);
    (*actionCollection()) += *doc->actionCollection();
    createGUI( "khelpcenterui.rc" );

    if (url.isEmpty())
        doc->openURL(KURL("help:/khelpcenter/index.html"));
    else
        doc->openURL( url );

}

KHMainWindow::~KHMainWindow()
{
    delete doc;
}

static KCmdLineOptions options[] =
{
   { "+[url]", I18N_NOOP("An URL to display"), "" },
   { 0,0,0 }
};


extern "C" int kdemain(int argc, char *argv[])
{
    KAboutData aboutData( "khelpcenter", I18N_NOOP("KDE HelpCenter"),
                          HELPCENTER_VERSION,
                          I18N_NOOP("The KDE Help Center"),
                          KAboutData::License_GPL,
                          "(c) 1999-2000, Matthias Elter");
    aboutData.addAuthor("Matthias Elter",0, "me@kde.org");

    KCmdLineArgs::init( argc, argv, &aboutData );
    KCmdLineArgs::addCmdLineOptions( options );
    KApplication::addCmdLineOptions();

    KApplication app;
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    KURL url;

    if (args->count())
        url = args->url(0);
    KHMainWindow *mw = new KHMainWindow(url);
    mw->show();

    return app.exec();
}

#include "khc_main.moc"
