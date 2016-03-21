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
#include "khc_debug.h"
#include "navigator.h"
#include "grantleeformatter.h"
#include "bookmarkowner.h"

#include <QAction>
#include <KActionCollection>
#include <KConfig>
#include <QIcon>
#include <KRun>
#include <KHTMLView>
#include <KHTMLSettings>
#include <QDialog>
#include <KStandardAction>
#include <KStartupInfo>
#include <KConfigGroup>
#include <KWindowConfig>
#include <KBookmarkManager>
#include <KBookmarkMenu>
#include <KActionMenu>

#include <QtDBus/QDBusConnection>
#include <QSplitter>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QList>
#include <QMimeDatabase>
#include <QStatusBar>
#include <QDir>

#include <QDialogButtonBox>
#include <QPushButton>

#include <kio/job.h>

#include <prefs.h>

using namespace KHC;

class LogDialog : public QDialog
{
  public:
    LogDialog( QWidget *parent = 0 )
      : QDialog( parent )
    {
      setWindowTitle( i18n("Search Error Log") );

      QVBoxLayout *mainLayout = new QVBoxLayout( this );

      mTextView = new QTextEdit( this );
      mTextView->setReadOnly( true );
      mTextView->setWordWrapMode( QTextOption::NoWrap );
      mainLayout->addWidget( mTextView );

      QDialogButtonBox *buttonBox = new QDialogButtonBox( QDialogButtonBox::Close );
      connect( buttonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
      connect( buttonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );
      mainLayout->addWidget( buttonBox );

      KConfigGroup cg = KSharedConfig::openConfig()->group( "logdialog" );
      KWindowConfig::restoreWindowSize( windowHandle(), cg );
    }

    ~LogDialog()
    {
      KConfigGroup cg = KSharedConfig::openConfig()->group( "logdialog" );
      KWindowConfig::saveWindowSize(windowHandle(), cg);
    }

    void setLog( const QString &log )
    {
      mTextView->setText( log );
    }

  private:
    QTextEdit *mTextView;
};


MainWindow::MainWindow()
    : KXmlGuiWindow(0),
      mLogDialog( 0 )
{
    setObjectName( QLatin1String( "MainWindow" ) );

    QDBusConnection::sessionBus().registerObject("/KHelpCenter", this, QDBusConnection::ExportScriptableSlots);
    mSplitter = new QSplitter( this );

    mDoc = new View( mSplitter, this, KHTMLPart::DefaultGUI, actionCollection() );
    connect( mDoc, SIGNAL( setWindowCaption( const QString & ) ),
             SLOT( setWindowTitle( const QString & ) ) );
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

    statusBar()->showMessage(i18n("Preparing Index"));

    connect( mDoc->browserExtension(),
             SIGNAL( openUrlRequest( const QUrl &,
                                     const KParts::OpenUrlArguments &, const KParts::BrowserArguments & ) ),
             SLOT( slotOpenURLRequest( const QUrl &,
                                       const KParts::OpenUrlArguments &, const KParts::BrowserArguments & ) ) );

    mNavigator = new Navigator( mDoc, mSplitter );
    mNavigator->setObjectName( "nav" );
    connect( mNavigator, SIGNAL( itemSelected( const QString & ) ),
             SLOT( viewUrl( const QString & ) ) );
    connect( mNavigator, SIGNAL( glossSelected( const GlossaryEntry & ) ),
             SLOT( slotGlossSelected( const GlossaryEntry & ) ) );
    connect( mNavigator, SIGNAL( setStatusBarText( const QString & ) ),
             SLOT( statusBarMessage( const QString & ) ) );

    mSplitter->insertWidget(0, mNavigator);
    mSplitter->setStretchFactor(mSplitter->indexOf(mNavigator), 0);
    setCentralWidget( mSplitter );
    QList<int> sizes;
    sizes << 220 << 580;
    mSplitter->setSizes(sizes);

    {
      if ( Prefs::useKonqSettings() ) {
        KConfig konqCfg( "konquerorrc" );
        const_cast<KHTMLSettings *>( mDoc->settings() )->init( &konqCfg );
      }
      const int fontScaleFactor = Prefs::fontzoomfactor();
      mDoc->setFontScaleFactor( fontScaleFactor );
    }

    setupActions();

    foreach (QAction *act, mDoc->actionCollection()->actions())
        actionCollection()->addAction(act->objectName(), act);

    setupBookmarks();

    setupGUI(QSize(800, 600), ToolBar | Keys | StatusBar | Create);
    setAutoSaveSettings();

    History::self().installMenuBarHook( this );

    connect( &History::self(), SIGNAL( goInternalUrl( const QUrl & ) ),
             mNavigator, SLOT( openInternalUrl( const QUrl & ) ) );
    connect( &History::self(), SIGNAL( goUrl( const QUrl & ) ),
             mNavigator, SLOT( selectItem( const QUrl & ) ) );

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

void MainWindow::saveProperties( KConfigGroup &config )
{
    config.writePathEntry( "URL" , mDoc->baseURL().url() );
}

void MainWindow::readProperties( const KConfigGroup &config )
{
    mDoc->slotReload( QUrl( config.readPathEntry( "URL", QString() ) ) );
}

void MainWindow::readConfig()
{
    QList<int> sizes = Prefs::splitter();
    if ( sizes.count() == 2 ) {
        mSplitter->setSizes( sizes );
    }

    mNavigator->readConfig();
}

void MainWindow::writeConfig()
{
    Prefs::setSplitter( mSplitter->sizes() );

    mNavigator->writeConfig();

    Prefs::self()->save();
}

void MainWindow::setupActions()
{
    actionCollection()->addAction( KStandardAction::Quit, this, SLOT( close() ) );
    actionCollection()->addAction( KStandardAction::Print, this, SLOT( print() ) );

    QAction *prevPage  = actionCollection()->addAction( "prevPage" );
    prevPage->setText( i18n( "Previous Page" ) );
    actionCollection()->setDefaultShortcut(prevPage, Qt::CTRL+Qt::Key_PageUp );
    prevPage->setWhatsThis( i18n( "Moves to the previous page of the document" ) );
    connect( prevPage, SIGNAL( triggered() ), mDoc, SLOT( prevPage() ) );

    QAction *nextPage  = actionCollection()->addAction( "nextPage" );
    nextPage->setText( i18n( "Next Page" ) );
    actionCollection()->setDefaultShortcut(nextPage, Qt::CTRL + Qt::Key_PageDown );
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
/*
    QAction *action = actionCollection()->addAction( QLatin1String("build_index") );
    action->setText( i18n("Build Search Index...") );
    connect( action, SIGNAL( triggered() ), mNavigator, SLOT( showIndexDialog() ) );

    KConfigGroup debugGroup( KSharedConfig::openConfig(), "Debug" );
    if ( debugGroup.readEntry( "SearchErrorLog", false) ) {
        action = actionCollection()->addAction(QLatin1String("show_search_stderr"));
        action->setText( i18n("Show Search Error Log") );
        connect( action, SIGNAL( triggered() ), this, SLOT( showSearchStderr() ) );
    }
*/
    History::self().setupActions( actionCollection() );

    QAction *action = actionCollection()->addAction(QLatin1String("configure_fonts" ));
    action->setText( i18n( "Configure Fonts..." ) );
    connect( action, SIGNAL( triggered() ), this, SLOT( slotConfigureFonts() ) );

    action = actionCollection()->addAction(QLatin1String("incFontSizes"));
    action->setText( i18n( "Increase Font Sizes" ) );
    action->setIcon( QIcon::fromTheme( QLatin1String("zoom-in") ) );
    connect( action, SIGNAL( triggered() ), this, SLOT( slotIncFontSizes() ) );

    action = actionCollection()->addAction(QLatin1String("decFontSizes"));
    action->setText( i18n( "Decrease Font Sizes" ) );
    action->setIcon( QIcon::fromTheme( QLatin1String("zoom-out") ) );
    connect( action, SIGNAL( triggered() ), this, SLOT( slotDecFontSizes() ) );
}

void MainWindow::setupBookmarks()
{
    const QString location = QStandardPaths::writableLocation( QStandardPaths::DataLocation );
    QDir().mkpath( location );
    const QString file = location + QStringLiteral( "/bookmarks.xml" );

    KBookmarkManager *manager = KBookmarkManager::managerForFile( file, QStringLiteral( "khelpcenter" ) );
    manager->setParent( this );
    BookmarkOwner *owner = new BookmarkOwner( mDoc, manager );
    connect( owner, SIGNAL( openUrl( const QUrl & ) ), this, SLOT( openUrl( const QUrl & ) ) );
    KActionMenu *actmenu = actionCollection()->add<KActionMenu>( QStringLiteral( "bookmarks" ) );
    actmenu->setText( i18nc( "@title:menu", "&Bookmarks" ) );
    KBookmarkMenu *bookmenu = new KBookmarkMenu( manager, owner, actmenu->menu(), actionCollection() );
    bookmenu->setParent( owner );
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

void MainWindow::goInternalUrl( const QUrl &url )
{
  mDoc->closeUrl();
  slotOpenURLRequest( url );
}

void MainWindow::slotOpenURLRequest( const QUrl &url,
                                     const KParts::OpenUrlArguments &args,
                                     const KParts::BrowserArguments &browserArgs )
{
  khcDebug() << url.url();

  mNavigator->selectItem( url );
  viewUrl( url, args, browserArgs );
}

void MainWindow::viewUrl( const QString &url )
{
  viewUrl( QUrl( url ) );
}

void MainWindow::viewUrl( const QUrl &url, const KParts::OpenUrlArguments &args, const KParts::BrowserArguments &browserArgs )
{
    stop();

    QString proto = url.scheme().toLower();

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
        QMimeDatabase db;
        QMimeType mime = db.mimeTypeForUrl( url );
        if ( mime.inherits("text/html") )
            own = true;
    }

    if ( !own ) {
        new KRun( url,this );
        return;
    }

    History::self().createEntry();

    mDoc->setArguments( args );
    mDoc->browserExtension()->setBrowserArguments( browserArgs );

    if ( proto == QLatin1String("glossentry") ) {
        QString decodedEntryId = QUrl::fromPercentEncoding( QUrl::toPercentEncoding(url.path()) );
        slotGlossSelected( mNavigator->glossEntry( decodedEntryId ) );
        mNavigator->slotSelectGlossEntry( decodedEntryId );
    } else {
        mDoc->openUrl( url );
    }
}

void MainWindow::documentCompleted()
{
    History::self().updateCurrentEntry( mDoc );
    History::self().updateActions();
}

void MainWindow::slotInfoMessage(KJob *, const QString &m)
{
    statusBarMessage(m);
}

void MainWindow::statusBarMessage(const QString &m)
{
    statusBar()->showMessage(m);
}

void MainWindow::openUrl( const QString &url )
{
    openUrl( QUrl( url ) );
}

void MainWindow::openUrl( const QString &url, const QByteArray& startup_id )
{
    KStartupInfo::setNewStartupId( this, startup_id );
    openUrl( QUrl( url ) );
}

void MainWindow::openUrl( const QUrl &url )
{
    if ( url.isEmpty() ) slotShowHome();
    else {
      mNavigator->selectItem( url );
      viewUrl( url );
    }
}

void MainWindow::slotGlossSelected(const GlossaryEntry &entry)
{
    stop();
    History::self().createEntry();
    mDoc->begin( QUrl( "glossentry:" + entry.id() ) );
    mDoc->write( mDoc->grantleeFormatter()->formatGlossaryEntry( entry ) );
    mDoc->end();
}

void MainWindow::stop()
{
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
  updateFontScaleActions();
}

void MainWindow::slotDecFontSizes()
{
  mDoc->slotDecFontSizes();
  updateFontScaleActions();
}

void MainWindow::updateFontScaleActions()
{
  actionCollection()->action( QLatin1String("incFontSizes") )->setEnabled( mDoc->fontScaleFactor() + mDoc->fontScaleStepping() <= 300 );
  actionCollection()->action( QLatin1String("decFontSizes") )->setEnabled( mDoc->fontScaleFactor() - mDoc->fontScaleStepping() >= 20 );

  Prefs::setFontzoomfactor( mDoc->fontScaleFactor() );
  Prefs::self()->save();
}

void MainWindow::slotConfigureFonts()
{
  FontDialog dlg( this );
  if ( dlg.exec() == QDialog::Accepted )
  {
    if (mDoc->baseURL().isEmpty())
    {
        const_cast<KHTMLSettings *>( mDoc->settings() )->init( KSharedConfig::openConfig().data() );
        slotShowHome();
    }
    else mDoc->slotReload();
  }
}


// vim:ts=2:sw=2:et
