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
#include <kstdaction.h>

#include "version.h"
#include <khtml_part.h>
#include <qlayout.h>
#include <kaction.h>
#include <qsplitter.h>
#include "khc_navigator.h"
#include <qhbox.h>

KHMainWindow::KHMainWindow(const KURL &url)
    : KMainWindow(0, "khmainwindow")
{
    splitter = new QSplitter(this);

    doc = new KHTMLPart( splitter, 0,
                         this, 0, KHTMLPart::BrowserViewGUI );
    connect(doc, SIGNAL(setWindowCaption(const QString &)),
            SLOT(setCaption(const QString &)));
    connect(doc, SIGNAL(setStatusBarText(const QString &)),
            statusBar(), SLOT(message(const QString &)));
    connect(doc, SIGNAL(onURL(const QString &)),
            statusBar(), SLOT(message(const QString &)));
    connect(doc, SIGNAL(started(KIO::Job *)),
            SLOT(slotStarted(KIO::Job *)));

    if (url.isEmpty())
        doc->openURL(KURL("help:/khelpcenter/index.html"));
    else
        doc->openURL( url );

    statusBar()->message(i18n("Preparing Index"));

    connect(doc->browserExtension(),
            SIGNAL(openURLRequest( const KURL &,
                                   const KParts::URLArgs &)),
            SLOT(slotOpenURLRequest( const KURL &,
                                     const KParts::URLArgs &)));

    nav = new khcNavigator(splitter, this, "nav");
    connect(nav->widget(), SIGNAL(itemSelected(const QString &)),
            SLOT(openURL(const QString &)));
    connect(nav->widget(), SIGNAL(glossSelected(const QString &)),
            SLOT(slotGlossSelected(const QString &)));

    splitter->moveToFirst(nav->widget());
    splitter->setResizeMode(nav->widget(), QSplitter::KeepSize);
    setCentralWidget( splitter );
    QValueList<int> sizes;
    sizes << 220 << 580;
    splitter->setSizes(sizes);
    setGeometry(366, 0, 800, 600);

    (*actionCollection()) += *doc->actionCollection();
    (void)KStdAction::close(this, SLOT(close()), actionCollection());

    createGUI( "khelpcenterui.rc" );

    statusBar()->message(i18n("Ready"));
}

void KHMainWindow::slotStarted(KIO::Job *job)
{
    kdDebug() << "slotStarted\n";

    connect(job, SIGNAL(infoMessage( KIO::Job *, const QString &)),
            this, SLOT(slotInfoMessage(KIO::Job *, const QString &)));
}

void KHMainWindow::slotOpenURLRequest( const KURL &url,
                                       const KParts::URLArgs &)
{
    if (url.protocol() == QString::fromLatin1("glossentry"))
        slotGlossSelected(KURL::decode_string(url.encodedPathAndQuery()));
    else
        doc->openURL(url);
}

void KHMainWindow::slotInfoMessage(KIO::Job *, const QString &m)
{
    statusBar()->message(m);
}

void KHMainWindow::openURL(const QString &url)
{
    doc->openURL(KURL(url));
}

void KHMainWindow::slotGlossSelected(const QString &term)
{
	kdDebug() << "slotGlossSelection: term = " << term << endl;

    QFile glossFile("/home/frerich/tmp/glossary.docbook");
    if (!glossFile.open(IO_ReadOnly))
        return;

    QDomDocument glossDom;
    if (!glossDom.setContent(&glossFile))
        return;

    QDomNodeList glossEntries = glossDom.documentElement().elementsByTagName(QString::fromLatin1("glossentry"));
		QDomNode glossEntry;
    
    for (unsigned int i = 0; i < glossEntries.count(); i++) {
		    glossEntry = glossEntries.item(i); 
        if (term == glossEntry.namedItem(QString::fromLatin1("glossterm")).toElement().text().simplifyWhiteSpace())
          break;
		}
		
    QString definition = glossEntry.namedItem(QString::fromLatin1("glossdef")).firstChild().toElement().text().simplifyWhiteSpace();

    QString seeAlso, seeAlsos;
		QDomNodeList seeAlsoNodes = glossEntry.toElement().elementsByTagName(QString::fromLatin1("glossseealso"));
    for (unsigned int i = 0; i < seeAlsoNodes.count(); i++) {
		    seeAlso = seeAlsoNodes.item(i).toElement().text().simplifyWhiteSpace();
				if (seeAlso.isEmpty())
				    continue;
		    if (seeAlsos.isEmpty())
		      seeAlsos = i18n("See also: ");
		    seeAlsos += QString::fromLatin1("<a href=\"glossentry:%1\">%2</a>, ").arg(seeAlso).arg(seeAlso);
    }
    if (!seeAlsos.isEmpty())
        seeAlsos = seeAlsos.left(seeAlsos.length() - 2);
    
    QFile htmlFile("/home/frerich/src/kde-cvs/kdebase/doc/khelpcenter/glossary.html");
    if (!htmlFile.open(IO_ReadOnly))
      return;
    QTextStream htmlStream(&htmlFile);
    QString htmlSrc = htmlStream.read().arg(term).arg(definition).arg(seeAlsos);
    
    doc->begin("/home/frerich/src/kde-cvs/kdebase/doc/khelpcenter/");
    doc->write(htmlSrc);
    doc->end(); 
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
