 /*
 *  khc_main.cpp - part of the KDE Help Center
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

#include <kapp.h>
#include <dcopclient.h>
#include <kglobal.h>
#include <klocale.h>
#include <kstddirs.h>
#include <kcmdlineargs.h>

#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>
#include <assert.h>

#include "khc_main.h"

#include <kaboutdata.h>
#include <kdebug.h>
#include <kstdaction.h>
#include <qtimer.h>

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
        prepareAbout();
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

    backAction = new KToolBarPopupAction( i18n( "&Back" ), "back", ALT+Key_Left,
                                          this, SLOT( slotBack() ),
                                          actionCollection(), "back" );
    connect( backAction->popupMenu(), SIGNAL( activated( int ) ), this, SLOT( slotBackActivated( int ) ) );

    forwardAction = new KToolBarPopupAction( i18n( "&Forward" ), "forward",
                                             ALT+Key_Right, this,
                                             SLOT( slotForward() ),
                                             actionCollection(), "forward" );
    connect( forwardAction->popupMenu(), SIGNAL( activated( int ) ), this, SLOT( slotForwardActivated( int ) ) );

    createGUI( "khelpcenterui.rc" );

    statusBar()->message(i18n("Ready"));
}

void KHMainWindow::prepareAbout()
{
    QString file = locate( "data", "khelpcenter/intro.html.in" );
    if ( file.isEmpty() )
        return;

    QFile f( file );

    if ( !f.open( IO_ReadOnly ) )
	return;

    QTextStream t( &f );

    QString res = t.read();

    file = langLookup( "khelpcenter/index.docbook" );

    // otherwise all embedded objects are referenced as about:/...
    QString basehref = QString::fromLatin1("<BASE HREF=\"file:") +
                       file.left( file.findRev( '/' ) )
		       + QString::fromLatin1("/\">\n");
    res.prepend( basehref );
    res = res.arg( i18n("Conquer your Desktop!") )
          .arg( i18n( "Welcome to the K Desktop Environment" ) )
          .arg( i18n( "The KDE team welcomes you to user-friendly UNIX computing" ) )
          .arg( i18n( "KDE is a powerful graphical desktop environment for Unix workstations.  A\n"
                      "KDE desktop combines ease of use, contemporary functionality and outstanding\n"
                      "graphical design with the technological superiority of the Unix operating\n"
                      "system." ) )
          .arg( i18n( "What is the K Desktop Environment?" ) )
          .arg( i18n( "Contacting the KDE Project" ) )
          .arg( i18n( "Supporting the KDE Project" ) )
          .arg( i18n( "Useful links" ) )
          .arg( i18n( "Getting the most out of KDE" ) )
          .arg( i18n( "General Documentation" ) )
          .arg( i18n( "A Quick Start Guide to the Desktop" ) )
          .arg( i18n( "KDE User's guide" ) )
          .arg( i18n( "Frequently asked questions" ) )
          .arg( i18n( "Basic Applications" ) )
          .arg( i18n( "The Kicker Desktop Panel" ) )
          .arg( i18n( "The KDE Control Center" ) )
          .arg( i18n( "The Konqueror File manager and Web Browser" ) );

    doc->begin( "about:khelpcenter" );
    doc->write( res );
    doc->end();
}

void KHMainWindow::slotStarted(KIO::Job *job)
{
    kdDebug() << "slotStarted\n";

    connect(job, SIGNAL(infoMessage( KIO::Job *, const QString &)),
            this, SLOT(slotInfoMessage(KIO::Job *, const QString &)));
}

void KHMainWindow::createHistoryEntry()
{
    // First, remove any forward history
    HistoryEntry * current = m_lstHistory.current();
    if (current)
    {
        //kdDebug(1202) << "Truncating history" << endl;
        m_lstHistory.at( m_lstHistory.count() - 1 ); // go to last one
        for ( ; m_lstHistory.current() != current ; )
        {
            if ( !m_lstHistory.removeLast() ) // and remove from the end (faster and easier)
                assert(0);
        }
        // Now current is the current again.
    }
    // Append a new entry
    //kdDebug(1202) << "Append a new entry" << endl;
    m_lstHistory.append( new HistoryEntry ); // made current
    //kdDebug(1202) << "at=" << m_lstHistory.at() << " count=" << m_lstHistory.count() << endl;
    assert( m_lstHistory.at() == (int) m_lstHistory.count() - 1 );
}

void KHMainWindow::slotOpenURLRequest( const KURL &url,
                                       const KParts::URLArgs &)
{
    if (url.protocol() == QString::fromLatin1("glossentry"))
        slotGlossSelected(static_cast<khcNavigatorWidget *>(nav->widget())->glossEntry(KURL::decode_string(url.encodedPathAndQuery())));
    else
        doc->openURL(url);
}

void KHMainWindow::slotBack()
{
    slotGoHistoryActivated( -1 );
}

void KHMainWindow::slotBackActivated( int id )
{
    slotGoHistoryActivated( -(backAction->popupMenu()->indexOf( id ) + 1) );
}

void KHMainWindow::slotForward()
{
    slotGoHistoryActivated( 1 );
}

void KHMainWindow::slotForwardActivated( int id )
{
    slotGoHistoryActivated( forwardAction->popupMenu()->indexOf( id ) + 1 );
}

void KHMainWindow::slotGoHistoryActivated( int steps )
{
    if (!m_goBuffer)
    {
        // Only start 1 timer.
        m_goBuffer = steps;
        QTimer::singleShot( 0, this, SLOT(slotGoHistoryDelayed()));
    }
}

void KHMainWindow::slotGoHistoryDelayed()
{
  if (!m_goBuffer) return;
  int steps = m_goBuffer;
  m_goBuffer = 0;
  // m_currentView->go( steps );
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
