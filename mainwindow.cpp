 /*
 *  This file is part of the KDE Help Center
 *
 *  Copyright (C) 1999 Matthias Elter (me@kde.org)
 *                2001 Stephan Kulow (coolo@kde.org)
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
#include <assert.h>

#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qtimer.h>
#include <qlayout.h>
#include <qsplitter.h>
#include <qhbox.h>

#include <kapplication.h>
#include <kconfig.h>
#include <dcopclient.h>
#include <kglobal.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kcmdlineargs.h>
#include <kpopupmenu.h>
#include <kstringhandler.h>
#include <kmimemagic.h>
#include <krun.h>
#include <kaboutdata.h>
#include <kdebug.h>
#include <kstdaction.h>
#include <kstdguiitem.h>
#include <khtmlview.h>
#include <khtml_part.h>
#include <kaction.h>
#include <kstatusbar.h>
#include <kdebugclasses.h>

#include "version.h"

#include "history.h"
#include "view.h"
#include "navigator.h"

using namespace KHC;

#include "mainwindow.h"
#include "mainwindow.moc"

MainWindow::MainWindow(const KURL &url)
    : KMainWindow(0, "MainWindow")
{
    splitter = new QSplitter(this);

    doc = new View( splitter, 0, this, 0, KHTMLPart::DefaultGUI );
    connect(doc, SIGNAL(setWindowCaption(const QString &)),
            SLOT(setCaption(const QString &)));
    connect(doc, SIGNAL(setStatusBarText(const QString &)),
            this, SLOT(statusBarMessage(const QString &)));
    connect(doc, SIGNAL(onURL(const QString &)),
            this, SLOT(statusBarMessage(const QString &)));
    connect(doc, SIGNAL(started(KIO::Job *)),
            SLOT(slotStarted(KIO::Job *)));
    connect(doc, SIGNAL(completed()),
            SLOT(documentCompleted()));

    statusBar()->insertItem(i18n("Preparing Index"), 0, 1);
    statusBar()->setItemAlignment(0, AlignLeft | AlignVCenter);

    connect(doc->browserExtension(),
            SIGNAL(openURLRequest( const KURL &,
                                   const KParts::URLArgs &)),
            SLOT(slotOpenURLRequest( const KURL &,
                                     const KParts::URLArgs &)));

    nav = new Navigator( doc, splitter, "nav");
    connect(nav, SIGNAL(itemSelected(const QString &)),
            SLOT(openURL(const QString &)));
    connect(nav, SIGNAL(glossSelected(const GlossaryEntry &)),
            SLOT(slotGlossSelected(const GlossaryEntry &)));

    splitter->moveToFirst(nav);
    splitter->setResizeMode(nav, QSplitter::KeepSize);
    setCentralWidget( splitter );
    QValueList<int> sizes;
    sizes << 220 << 580;
    splitter->setSizes(sizes);
    setGeometry(366, 0, 800, 600);

    (void)KStdAction::quit(this, SLOT(close()), actionCollection());
    (void)KStdAction::print(this, SLOT(print()), actionCollection(), "printFrame");

    History::self().setupActions( actionCollection() );

    insertChildClient( doc );
    createGUI( "khelpcenterui.rc" );

    History::self().installMenuBarHook( this );

    KURL u;
    if ( url.isEmpty() ) {
      KConfig *cfg = KGlobal::config();
      cfg->setGroup( "General" );
      u = cfg->readEntry( "StartUrl",
                          "help:/khelpcenter/index.html?anchor=welcome" );
    } else {
      u = url;
    }
      
    openURL( u );
    nav->selectItem( u );

    statusBarMessage(i18n("Ready"));
}

void MainWindow::print()
{
    doc->view()->print();
}

void MainWindow::slotStarted(KIO::Job *job)
{
    if (job)
       connect(job, SIGNAL(infoMessage( KIO::Job *, const QString &)),
               SLOT(slotInfoMessage(KIO::Job *, const QString &)));

    History::self().updateActions();
}

void MainWindow::slotOpenURLRequest( const KURL &url,
                                     const KParts::URLArgs &args)
{
    kdDebug() << "MainWindow::slotOpenURLRequest(): " << url.url() << endl;

    bool own = false;
    
    QString proto = url.protocol().lower();
    if ( proto == "help" || proto == "glossentry" || proto == "about" ||
	 proto == "man" || proto == "info" || proto == "cgi" ||
         proto == "http" )
	own = true;
    else if (url.isLocalFile()) {
	static const QString &html = KGlobal::staticQString("text/html");
	KMimeMagicResult *res = KMimeMagic::self()->findFileType(url.path());
	if (res->isValid() && res->accuracy() > 40 && res->mimeType() == html)
	    own = true;
    }
    
    if (!own) {
	new KRun( url );
	return;
    }
    
    stop();

    doc->browserExtension()->setURLArgs( args );

    if (proto == QString::fromLatin1("glossentry"))
        slotGlossSelected(nav->glossEntry(KURL::decode_string(url.encodedPathAndQuery())));
    else {
		History::self().createEntry();
	doc->openURL( url );
    }
}

void MainWindow::documentCompleted()
{
    History::self().updateCurrentEntry( doc );
    History::self().updateActions();
}

void MainWindow::slotInfoMessage(KIO::Job *, const QString &m)
{
    statusBarMessage(m);
}

void MainWindow::statusBarMessage(const QString &m)
{
    statusBar()->changeItem(m, 0);
}

void MainWindow::openURL(const QString &url)
{
    openURL( KURL( url ) );
}

void MainWindow::openURL(const KURL &url)
{
    stop();
    KParts::URLArgs args;
    slotOpenURLRequest(url, args);
}

void MainWindow::slotGlossSelected(const GlossaryEntry &entry)
{
    stop();
    History::self().createEntry();
    doc->showGlossaryEntry( entry );
}

void MainWindow::stop()
{
    doc->closeURL();
    History::self().updateCurrentEntry( doc );
}

MainWindow::~MainWindow()
{
    delete doc;
}
