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
#include <qdbusconnection.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kicon.h>
#include <kiconloader.h>
#include <kmimetype.h>
#include <krun.h>
#include <kaboutdata.h>
#include <kdebug.h>
#include <khtmlview.h>
#include <khtml_settings.h>
#include <kstatusbar.h>
#include <kstandardshortcut.h>
#include <kdialog.h>
#include <klocale.h>
#include <kstandardaction.h>
#include <kxmlguifactory.h>

#include <QSplitter>
#include <QTextEdit>
#include <QLayout>
//Added by qt3to4:
#include <QVBoxLayout>
#include <QFrame>
#include <QList>
#include <QBoxLayout>

#include <stdlib.h>
#include <assert.h>
#include <QList>
#include <kglobal.h>

using namespace KHC;

class LogDialog : public KDialog
{
  public:
    LogDialog( QWidget *parent = 0 )
      : KDialog( parent )
    {
      setCaption( i18n("Search Error Log") );
      setButtons( Ok );

      QFrame *topFrame = new QFrame( this );
      setMainWidget( topFrame );

      QBoxLayout *topLayout = new QVBoxLayout( topFrame );

      mTextView = new QTextEdit( topFrame );
      mTextView->setTextFormat( Qt::LogText );
      topLayout->addWidget( mTextView );

      KConfig *cfg = KGlobal::config();
      cfg->setGroup( "logdialog" );
      restoreDialogSize( cfg );
    }

    ~LogDialog()
    {
      KConfig *cfg = KGlobal::config();
      cfg->setGroup( "logdialog" );
      KDialog::saveDialogSize( cfg );
    }

    void setLog( const QString &log )
    {
      mTextView->setText( log );
    }

  private:
    QTextEdit *mTextView;
};


MainWindow::MainWindow()
    : KMainWindow(0),
      mLogDialog( 0 )
{
    setObjectName( "MainWindow" );

    QDBusConnection::sessionBus().registerObject("/KHelpCenter", this, QDBusConnection::ExportScriptableSlots);
    mSplitter = new QSplitter( this );

    mDoc = new View( mSplitter, this, KHTMLPart::DefaultGUI, actionCollection() );
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
             SIGNAL( openUrlRequest( const KUrl &,
                                     const KParts::URLArgs & ) ),
             SLOT( slotOpenURLRequest( const KUrl &,
                                       const KParts::URLArgs & ) ) );

    mNavigator = new Navigator( mDoc, mSplitter, "nav" );
    connect( mNavigator, SIGNAL( itemSelected( const QString & ) ),
             SLOT( viewUrl( const QString & ) ) );
    connect( mNavigator, SIGNAL( glossSelected( const GlossaryEntry & ) ),
             SLOT( slotGlossSelected( const GlossaryEntry & ) ) );

    mSplitter->insertWidget(0, mNavigator);
    mSplitter->setStretchFactor(mSplitter->indexOf(mNavigator), 0);
    setCentralWidget( mSplitter );
    QList<int> sizes;
    sizes << 220 << 580;
    mSplitter->setSizes(sizes);
    setGeometry(366, 0, 800, 600);

    KConfig *cfg = KGlobal::config();
    {
      KConfigGroup configGroup( cfg, "General" );
      if ( configGroup.readEntry( "UseKonqSettings", QVariant(true )).toBool() ) {
        KConfig konqCfg( "konquerorrc" );
        const_cast<KHTMLSettings *>( mDoc->settings() )->init( &konqCfg );
      }
      const int zoomFactor = configGroup.readEntry( "Font zoom factor", 100 );
      mDoc->setZoomFactor( zoomFactor );
    }

    setupActions();

    actionCollection()->addDocCollection( mDoc->actionCollection() );

    setupGUI(ToolBar | Keys | StatusBar | Create);
    setAutoSaveSettings();

    History::self().installMenuBarHook( this );

    connect( &History::self(), SIGNAL( goInternalUrl( const KUrl & ) ),
             mNavigator, SLOT( openInternalUrl( const KUrl & ) ) );
    connect( &History::self(), SIGNAL( goUrl( const KUrl & ) ),
             mNavigator, SLOT( selectItem( const KUrl & ) ) );

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
    kDebug()<<"void MainWindow::saveProperties( KConfig *config )" << endl;
    config->writePathEntry( "URL" , mDoc->baseURL().url() );
}

void MainWindow::readProperties( KConfig *config )
{
    kDebug()<<"void MainWindow::readProperties( KConfig *config )" << endl;
    mDoc->slotReload( KUrl( config->readPathEntry( "URL" ) ) );
}

void MainWindow::readConfig()
{
    KConfig *config = KGlobal::config();
    config->setGroup( "MainWindowState" );
    QList<int> sizes = config->readEntry( "Splitter", QList<int>() );
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
    actionCollection()->addAction( KStandardAction::Quit, this, SLOT( close() ) );
    actionCollection()->addAction( KStandardAction::Print, "printFrame", this, SLOT( print() ) );

    QAction *prevPage  = actionCollection()->addAction( "prevPage" );
    prevPage->setText( i18n( "Previous Page" ) );
    prevPage->setShortcut( Qt::CTRL+Qt::Key_PageUp );
    prevPage->setWhatsThis( i18n( "Moves to the previous page of the document" ) );
    connect( prevPage, SIGNAL( triggered() ), mDoc, SLOT( prevPage() ) );

    QAction *nextPage  = actionCollection()->addAction( "nextPage" );
    nextPage->setText( i18n( "Next Page" ) );
    nextPage->setShortcut( Qt::CTRL + Qt::Key_PageDown );
    nextPage->setWhatsThis( i18n( "Moves to the next page of the document" ) );
    connect( nextPage, SIGNAL( triggered() ), mDoc, SLOT( nextPage() ) );

    QAction *home = KStandardAction::home( this, SLOT( slotShowHome() ), this );
    actionCollection()->addAction( home->objectName(), home );
    home->setText(i18n("Table of &Contents"));
    home->setToolTip(i18n("Table of contents"));
    home->setWhatsThis(i18n("Go back to the table of contents"));

    mCopyText = KStandardAction::copy( this, SLOT(slotCopySelectedText()), this );
    actionCollection()->addAction( "copy_text", mCopyText );

    mLastSearchAction = actionCollection()->addAction( QLatin1String("lastsearch") );
    mLastSearchAction->setText( i18n("&Last Search Result") );
    mLastSearchAction->setEnabled( false );
    connect( mLastSearchAction, SIGNAL( triggered() ), this, SLOT( slotLastSearch() ) );

    QAction *action = actionCollection()->addAction( QLatin1String("build_index") );
    action->setText( i18n("Build Search Index...") );
    connect( action, SIGNAL( triggered() ), mNavigator, SLOT( showIndexDialog() ) );

    actionCollection()->addAction( KStandardAction::KeyBindings, guiFactory(), SLOT( configureShortcuts() ) );

    KConfigGroup debugGroup( KGlobal::config(), "Debug" );
    if ( debugGroup.readEntry( "SearchErrorLog", QVariant(false )).toBool() ) {
        action = actionCollection()->addAction(QLatin1String("show_search_stderr"));
        action->setText( i18n("Show Search Error Log") );
        connect( action, SIGNAL( triggered() ), this, SLOT( showSearchStderr() ) );
    }

    History::self().setupActions( actionCollection() );

    action = actionCollection()->addAction(QLatin1String("configure_fonts" ));
    action->setText( i18n( "Configure Fonts..." ) );
    connect( action, SIGNAL( triggered() ), this, SLOT( slotConfigureFonts() ) );

    action = actionCollection()->addAction(QLatin1String("incFontSizes"));
    action->setText( i18n( "Increase Font Sizes" ) );
    action->setIcon( KIcon( QLatin1String("viewmag+") ) );
    connect( action, SIGNAL( triggered() ), this, SLOT( slotIncFontSizes() ) );

    action = actionCollection()->addAction(QLatin1String("decFontSizes"));
    action->setText( i18n( "Decrease Font Sizes" ) );
    action->setIcon( KIcon( QLatin1String("viewmag-") ) );
    connect( action, SIGNAL( triggered() ), this, SLOT( slotDecFontSizes() ) );
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
       connect(job, SIGNAL(infoMessage( KJob *, const QString &, const QString &)),
               SLOT(slotInfoMessage(KJob *, const QString &)));

    History::self().updateActions();
}

void MainWindow::goInternalUrl( const KUrl &url )
{
  mDoc->closeUrl();
  slotOpenURLRequest( url, KParts::URLArgs() );
}

void MainWindow::slotOpenURLRequest( const KUrl &url,
                                     const KParts::URLArgs &args )
{
  kDebug( 1400 ) << "MainWindow::slotOpenURLRequest(): " << url.url() << endl;

  mNavigator->selectItem( url );
  viewUrl( url, args );
}

void MainWindow::viewUrl( const QString &url )
{
  viewUrl( KUrl( url ) );
}

void MainWindow::viewUrl( const KUrl &url, const KParts::URLArgs &args )
{
    stop();

    QString proto = url.protocol().toLower();

    if ( proto == "khelpcenter" ) {
      History::self().createEntry();
      mNavigator->openInternalUrl( url );
      return;
    }

    bool own = false;

    if ( proto == QLatin1String("help")
        || proto == QLatin1String("glossentry")
        || proto == QLatin1String("about")
        || proto == QLatin1String("man")
        || proto == QLatin1String("info")
        || proto == QLatin1String("cgi")
        || proto == QLatin1String("ghelp"))
        own = true;
    else if ( url.isLocalFile() ) {
        KMimeType::Ptr mime = KMimeType::findByPath( url.path() );
        if ( mime->is("text/html") )
            own = true;
    }

    if ( !own ) {
        new KRun( url,this );
        return;
    }

    History::self().createEntry();

    mDoc->browserExtension()->setUrlArgs( args );

    if ( proto == QLatin1String("glossentry") ) {
        QString decodedEntryId = QUrl::fromPercentEncoding( url.encodedPathAndQuery().toAscii() );
        slotGlossSelected( mNavigator->glossEntry( decodedEntryId ) );
        mNavigator->slotSelectGlossEntry( decodedEntryId );
    } else {
        mDoc->openUrl( url );
    }
}

void MainWindow::documentCompleted()
{
    kDebug() << "MainWindow::documentCompleted" << endl;

    History::self().updateCurrentEntry( mDoc );
    History::self().updateActions();
}

void MainWindow::slotInfoMessage(KJob *, const QString &m)
{
    statusBarMessage(m);
}

void MainWindow::statusBarMessage(const QString &m)
{
    statusBar()->changeItem(m, 0);
}

void MainWindow::openUrl( const QString &url )
{
    openUrl( KUrl( url ) );
}

void MainWindow::openUrl( const QString &url, const QByteArray& startup_id )
{
    KStartupInfo::setNewStartupId( this, startup_id );
    openUrl( KUrl( url ) );
}

void MainWindow::openUrl( const KUrl &url )
{
    if ( url.isEmpty() ) slotShowHome();
    else {
      mNavigator->selectItem( url );
      viewUrl( url );
    }
}

void MainWindow::slotGlossSelected(const GlossaryEntry &entry)
{
    kDebug() << "MainWindow::slotGlossSelected()" << endl;

    stop();
    History::self().createEntry();
    mDoc->begin( KUrl( "help:/khelpcenter/glossary" ) );
    mDoc->write( Glossary::entryToHtml( entry ) );
    mDoc->end();
}

void MainWindow::stop()
{
    kDebug() << "MainWindow::stop()" << endl;

    mDoc->closeUrl();
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
  actionCollection()->action( QLatin1String("incFontSizes") )->setEnabled( mDoc->zoomFactor() + mDoc->zoomStepping() <= 300 );
  actionCollection()->action( QLatin1String("decFontSizes") )->setEnabled( mDoc->zoomFactor() - mDoc->zoomStepping() >= 20 );

  KConfigGroup configGroup( KGlobal::config(), QLatin1String("General") );
  configGroup.writeEntry( QLatin1String("Font zoom factor"), mDoc->zoomFactor() );
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
