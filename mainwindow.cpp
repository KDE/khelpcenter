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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "mainwindow.h"

#include "history.h"
#include "view.h"
#include "searchengine.h"
#include "fontdialog.h"
#include "prefs.h"

#include <kapplication.h>
#include <kconfig.h>
#include <dcopclient.h>
#include <kiconloader.h>
#include <kmimemagic.h>
#include <krun.h>
#include <kaboutdata.h>
#include <kdebug.h>
#include <khtmlview.h>
#include <khtml_settings.h>
#include <kaction.h>
#include <kstatusbar.h>
#include <kstdaccel.h>
#include <kdialogbase.h>
#include <klocale.h>
#include <kxmlguifactory.h>

#include <qsplitter.h>
#include <q3textedit.h>
#include <qlayout.h>
//Added by qt3to4:
#include <QVBoxLayout>
#include <QFrame>
#include <QList>
#include <Q3CString>
#include <QBoxLayout>

#include <stdlib.h>
#include <assert.h>
#include <QList>

using namespace KHC;

class LogDialog : public KDialogBase
{
  public:
    LogDialog( QWidget *parent = 0 )
      : KDialogBase( Plain, i18n("Search Error Log"), Ok, Ok, parent, 0,
                     false )
    {
      QFrame *topFrame = plainPage();

      QBoxLayout *topLayout = new QVBoxLayout( topFrame );

      mTextView = new Q3TextEdit( topFrame );
      mTextView->setTextFormat( Qt::LogText );
      topLayout->addWidget( mTextView );

      resize( configDialogSize( "logdialog" ) );
    }

    ~LogDialog()
    {
      saveDialogSize( "logdialog" );
    }

    void setLog( const QString &log )
    {
      mTextView->setText( log );
    }

  private:
    Q3TextEdit *mTextView;
};


MainWindow::MainWindow()
    : KMainWindow(0, "MainWindow"), DCOPObject( "KHelpCenterIface" ),
      mLogDialog( 0 )
{
    mSplitter = new QSplitter( this );

    mDoc = new View( mSplitter, 0, this, 0, KHTMLPart::DefaultGUI, actionCollection() );
    connect( mDoc, SIGNAL( setWindowCaption( const QString & ) ),
             SLOT( setCaption( const QString & ) ) );
    connect( mDoc, SIGNAL( setStatusBarText( const QString & ) ),
             SLOT( statusBarMessage( const QString & ) ) );
    connect( mDoc, SIGNAL( onURL( const QString & ) ),
             SLOT( statusBarMessage( const QString & ) ) );
    connect( mDoc, SIGNAL( started( KIO::Job * ) ),
             SLOT( slotStarted( KIO::Job * ) ) );
    connect( mDoc, SIGNAL( completed() ),
             SLOT( documentCompleted() ) );
    connect( mDoc, SIGNAL( searchResultCacheAvailable() ),
             SLOT( enableLastSearchAction() ) );

    connect( mDoc, SIGNAL( selectionChanged() ),
             SLOT( enableCopyTextAction() ) );

    statusBar()->insertItem(i18n("Preparing Index"), 0, 1);
    statusBar()->setItemAlignment(0, Qt::AlignLeft | Qt::AlignVCenter);

    connect( mDoc->browserExtension(),
             SIGNAL( openURLRequest( const KURL &,
                                     const KParts::URLArgs & ) ),
             SLOT( slotOpenURLRequest( const KURL &,
                                       const KParts::URLArgs & ) ) );

    mNavigator = new Navigator( mDoc, mSplitter, "nav" );
    connect( mNavigator, SIGNAL( itemSelected( const QString & ) ),
             SLOT( viewUrl( const QString & ) ) );
    connect( mNavigator, SIGNAL( glossSelected( const GlossaryEntry & ) ),
             SLOT( slotGlossSelected( const GlossaryEntry & ) ) );

    mSplitter->moveToFirst(mNavigator);
    mSplitter->setResizeMode(mNavigator, QSplitter::KeepSize);
    setCentralWidget( mSplitter );
    QList<int> sizes;
    sizes << 220 << 580;
    mSplitter->setSizes(sizes);
    setGeometry(366, 0, 800, 600);

    KConfig *cfg = kapp->config();
    {
      KConfigGroup configGroup( cfg, "General" );
      if ( configGroup.readBoolEntry( "UseKonqSettings", true ) ) {
        KConfig konqCfg( "konquerorrc" );
        const_cast<KHTMLSettings *>( mDoc->settings() )->init( &konqCfg );
      }
      const int zoomFactor = configGroup.readNumEntry( "Font zoom factor", 100 );
      mDoc->setZoomFactor( zoomFactor );
    }

    setupActions();

    actionCollection()->addDocCollection( mDoc->actionCollection() );

    setupGUI(ToolBar | Keys | StatusBar | Create);
    setAutoSaveSettings();

    History::self().installMenuBarHook( this );

    connect( &History::self(), SIGNAL( goInternalUrl( const KURL & ) ),
             mNavigator, SLOT( openInternalUrl( const KURL & ) ) );
    connect( &History::self(), SIGNAL( goUrl( const KURL & ) ),
             mNavigator, SLOT( selectItem( const KURL & ) ) );

    statusBarMessage(i18n("Ready"));
    enableCopyTextAction();

    readConfig();
}

MainWindow::~MainWindow()
{
    writeConfig();
}

void MainWindow::enableCopyTextAction()
{
    mCopyText->setEnabled( mDoc->hasSelection() );
}

void MainWindow::saveProperties( KConfig *config )
{
    kdDebug()<<"void MainWindow::saveProperties( KConfig *config )" << endl;
    config->writePathEntry( "URL" , mDoc->baseURL().url() );
}

void MainWindow::readProperties( KConfig *config )
{
    kdDebug()<<"void MainWindow::readProperties( KConfig *config )" << endl;
    mDoc->slotReload( KURL( config->readPathEntry( "URL" ) ) );
}

void MainWindow::readConfig()
{
    KConfig *config = KGlobal::config();
    config->setGroup( "MainWindowState" );
    QList<int> sizes = config->readIntListEntry( "Splitter" );
    if ( sizes.count() == 2 ) {
        mSplitter->setSizes( sizes );
    }

    mNavigator->readConfig();
}

void MainWindow::writeConfig()
{
    KConfig *config = KGlobal::config();
    config->setGroup( "MainWindowState" );
    config->writeEntry( "Splitter", mSplitter->sizes() );

    mNavigator->writeConfig();

    Prefs::writeConfig();
}

void MainWindow::setupActions()
{
    KStdAction::quit( this, SLOT( close() ), actionCollection() );
    KStdAction::print( this, SLOT( print() ), actionCollection(),
                       "printFrame" );

    KAction *prevPage  = new KAction( i18n( "Previous Page" ), Qt::CTRL+Qt::Key_PageUp, mDoc, SLOT( prevPage() ),
                         actionCollection(), "prevPage" );
    prevPage->setWhatsThis( i18n( "Moves to the previous page of the document" ) );

    KAction *nextPage  = new KAction( i18n( "Next Page" ), Qt::CTRL + Qt::Key_PageDown, mDoc, SLOT( nextPage() ),
                         actionCollection(), "nextPage" );
    nextPage->setWhatsThis( i18n( "Moves to the next page of the document" ) );

    KAction *home = KStdAction::home( this, SLOT( slotShowHome() ), actionCollection() );
    home->setText(i18n("Table of &Contents"));
    home->setToolTip(i18n("Table of contents"));
    home->setWhatsThis(i18n("Go back to the table of contents"));

    mCopyText = KStdAction::copy( this, SLOT(slotCopySelectedText()), actionCollection(), "copy_text");

    mLastSearchAction = new KAction( i18n("&Last Search Result"), 0, this,
                                     SLOT( slotLastSearch() ),
                                     actionCollection(), "lastsearch" );
    mLastSearchAction->setEnabled( false );

    new KAction( i18n("Build Search Index..."), 0, mNavigator,
      SLOT( showIndexDialog() ), actionCollection(), "build_index" );
    KStdAction::keyBindings( guiFactory(), SLOT( configureShortcuts() ),
      actionCollection() );

    KConfigGroup debugGroup( KGlobal::config(), "Debug" );
    if ( debugGroup.readBoolEntry( "SearchErrorLog", false ) ) {
      new KAction( i18n("Show Search Error Log"), 0, this,
                   SLOT( showSearchStderr() ), actionCollection(),
                   "show_search_stderr" );
    }

    History::self().setupActions( actionCollection() );

    new KAction( i18n( "Configure Fonts..." ), KShortcut(), this, SLOT( slotConfigureFonts() ), actionCollection(), "configure_fonts" );
    new KAction( i18n( "Increase Font Sizes" ), "viewmag+", KShortcut(), this, SLOT( slotIncFontSizes() ), actionCollection(), "incFontSizes" );
    new KAction( i18n( "Decrease Font Sizes" ), "viewmag-", KShortcut(), this, SLOT( slotDecFontSizes() ), actionCollection(), "decFontSizes" );
}

void MainWindow::slotCopySelectedText()
{
  mDoc->copySelectedText();
}

void MainWindow::print()
{
    mDoc->view()->print();
}

void MainWindow::slotStarted(KIO::Job *job)
{
    if (job)
       connect(job, SIGNAL(infoMessage( KIO::Job *, const QString &)),
               SLOT(slotInfoMessage(KIO::Job *, const QString &)));

    History::self().updateActions();
}

void MainWindow::goInternalUrl( const KURL &url )
{
  mDoc->closeURL();
  slotOpenURLRequest( url, KParts::URLArgs() );
}

void MainWindow::slotOpenURLRequest( const KURL &url,
                                     const KParts::URLArgs &args )
{
  kdDebug( 1400 ) << "MainWindow::slotOpenURLRequest(): " << url.url() << endl;

  mNavigator->selectItem( url );
  viewUrl( url, args );
}

void MainWindow::viewUrl( const QString &url )
{
  viewUrl( KURL( url ) );
}

void MainWindow::viewUrl( const KURL &url, const KParts::URLArgs &args )
{
    stop();

    QString proto = url.protocol().lower();

    if ( proto == "khelpcenter" ) {
      History::self().createEntry();
      mNavigator->openInternalUrl( url );
      return;
    }

    bool own = false;

    if ( proto == "help" || proto == "glossentry" || proto == "about" ||
         proto == "man" || proto == "info" || proto == "cgi" ||
         proto == "ghelp" )
        own = true;
    else if ( url.isLocalFile() ) {
        KMimeMagicResult *res = KMimeMagic::self()->findFileType( url.path() );
        if ( res->isValid() && res->accuracy() > 40
             && res->mimeType() == "text/html" )
            own = true;
    }

    if ( !own ) {
        new KRun( url,this );
        return;
    }

    History::self().createEntry();

    mDoc->browserExtension()->setURLArgs( args );

    if ( proto == QLatin1String("glossentry") ) {
        QString decodedEntryId = KURL::decode_string( url.encodedPathAndQuery() );
        slotGlossSelected( mNavigator->glossEntry( decodedEntryId ) );
        mNavigator->slotSelectGlossEntry( decodedEntryId );
    } else {
        mDoc->openURL( url );
    }
}

void MainWindow::documentCompleted()
{
    kdDebug() << "MainWindow::documentCompleted" << endl;

    History::self().updateCurrentEntry( mDoc );
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

void MainWindow::openUrl( const QString &url )
{
    openUrl( KURL( url ) );
}

void MainWindow::openUrl( const QString &url, const Q3CString& startup_id )
{
    KStartupInfo::setNewStartupId( this, startup_id );
    openUrl( KURL( url ) );
}

void MainWindow::openUrl( const KURL &url )
{
    if ( url.isEmpty() ) slotShowHome();
    else {
      mNavigator->selectItem( url );
      viewUrl( url );
    }
}

void MainWindow::slotGlossSelected(const GlossaryEntry &entry)
{
    kdDebug() << "MainWindow::slotGlossSelected()" << endl;

    stop();
    History::self().createEntry();
    mDoc->begin( KURL( "help:/khelpcenter/glossary" ) );
    mDoc->write( Glossary::entryToHtml( entry ) );
    mDoc->end();
}

void MainWindow::stop()
{
    kdDebug() << "MainWindow::stop()" << endl;

    mDoc->closeURL();
    History::self().updateCurrentEntry( mDoc );
}

void MainWindow::showHome()
{
    slotShowHome();
}

void MainWindow::slotShowHome()
{
    viewUrl( mNavigator->homeURL() );
    mNavigator->clearSelection();
}

void MainWindow::lastSearch()
{
    slotLastSearch();
}

void MainWindow::slotLastSearch()
{
    mDoc->lastSearch();
}

void MainWindow::enableLastSearchAction()
{
    mLastSearchAction->setEnabled( true );
}

void MainWindow::showSearchStderr()
{
  QString log = mNavigator->searchEngine()->errorLog();

  if ( !mLogDialog ) {
    mLogDialog = new LogDialog( this );
  }

  mLogDialog->setLog( log );
  mLogDialog->show();
  mLogDialog->raise();
}

void MainWindow::slotIncFontSizes()
{
  mDoc->slotIncFontSizes();
  updateZoomActions();
}

void MainWindow::slotDecFontSizes()
{
  mDoc->slotDecFontSizes();
  updateZoomActions();
}

void MainWindow::updateZoomActions()
{
  actionCollection()->action( "incFontSizes" )->setEnabled( mDoc->zoomFactor() + mDoc->zoomStepping() <= 300 );
  actionCollection()->action( "decFontSizes" )->setEnabled( mDoc->zoomFactor() - mDoc->zoomStepping() >= 20 );

  KConfigGroup configGroup( KGlobal::config(), "General" );
  configGroup.writeEntry( "Font zoom factor", mDoc->zoomFactor() );
  configGroup.sync();
}

void MainWindow::slotConfigureFonts()
{
  FontDialog dlg( this );
  if ( dlg.exec() == QDialog::Accepted )
    mDoc->slotReload();
}

#include "mainwindow.moc"

// vim:ts=2:sw=2:et
