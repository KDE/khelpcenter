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
    connect(nav->widget(), SIGNAL(glossSelected(const khcNavigatorWidget::GlossaryEntry &)),
            SLOT(slotGlossSelected(const khcNavigatorWidget::GlossaryEntry &)));

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
        slotGlossSelected(static_cast<khcNavigatorWidget *>(nav->widget())->glossEntry(KURL::decode_string(url.encodedPathAndQuery())));
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

void KHMainWindow::slotGlossSelected(const khcNavigatorWidget::GlossaryEntry &entry)
{
    QFile htmlFile(langLookup(QString::fromLatin1("khelpcenter/glossary.html")));
    if (!htmlFile.open(IO_ReadOnly))
        return;

    QString seeAlso;
    if (!entry.seeAlso.isEmpty()) {
        seeAlso = i18n("See also: ");
        QStringList seeAlsos = entry.seeAlso;
        QStringList::Iterator it = seeAlsos.begin();
        QStringList::Iterator end = seeAlsos.end();
        for (; it != end; ++it) {
            seeAlso += QString::fromLatin1("<a href=\"glossentry:");
            seeAlso += (*it).latin1();
            seeAlso += QString::fromLatin1("\">") + (*it).latin1();
            seeAlso += QString::fromLatin1("</a>, ");
        }
        seeAlso = seeAlso.left(seeAlso.length() - 2);
    }

    QTextStream htmlStream(&htmlFile);
    QString htmlSrc = htmlStream.read().arg(entry.term).arg(entry.definition).arg(seeAlso);

    KURL dataDir = langLookup(QString::fromLatin1("khelpcenter/glossary.html"));
    
    doc->begin(dataDir.path());
    doc->write(htmlSrc);
    doc->end(); 
}

QString KHMainWindow::langLookup(const QString &fname)
{
    QStringList search;

    // assemble the local search paths
    const QStringList localDoc = KGlobal::dirs()->resourceDirs("html");

    // look up the different languages
    for (int id=localDoc.count()-1; id >= 0; --id)
    {
        QStringList langs = KGlobal::locale()->languageList();
        langs.append("default");
        langs.append("en");
        QStringList::ConstIterator lang;
        for (lang = langs.begin(); lang != langs.end(); ++lang)
            search.append(QString("%1%2/%3").arg(localDoc[id]).arg(*lang).arg(fname));
    }

    // try to locate the file
    QStringList::Iterator it;
    for (it = search.begin(); it != search.end(); ++it)
    {
        kdDebug() << "Looking for help in: " << *it << endl;

        QFileInfo info(*it);
        if (info.exists() && info.isFile() && info.isReadable())
            return *it;

        QString file = (*it).left((*it).findRev('/')) + "/index.docbook";
        kdDebug() << "Looking for help in: " << file << endl;
        info.setFile(file);
        if (info.exists() && info.isFile() && info.isReadable())
            return *it;
    }

    return QString::null;
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
