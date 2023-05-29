/*
    SPDX-FileCopyrightText: 1999 Matthias Elter <me@kde.org>
    SPDX-FileCopyrightText: 2001 Stephan Kulow <coolo@kde.org>
    SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "mainwindow.h"

#include "history.h"
#include "view.h"
#include "searchengine.h"
#include "khc_debug.h"
#include "navigator.h"
#include "grantleeformatter.h"
#include "bookmarkowner.h"
#include "printdoc.h"

#include <QAction>
#include <QDBusConnection>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDir>
#include <QList>
#include <QMimeDatabase>
#include <QPushButton>
#include <QSplitter>
#include <QStatusBar>
#include <QTextDocument>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QMenu>

#include <KActionCollection>
#include <KActionMenu>
#include <KBookmarkManager>
#include <KBookmarkMenu>
#include <KConfig>
#include <KConfigGroup>
#include <KStandardAction>
#include <KLocalizedString>
#include <KStartupInfo>
#include <KWindowConfig>

#include <KIO/JobUiDelegate>
#include <KIO/JobUiDelegateFactory>
#include <KIO/OpenUrlJob>
#include <KIO/Job>
#include <kio_version.h>

#include <prefs.h>

using namespace KHC;

class LogDialog : public QDialog
{
  public:
    explicit LogDialog( QWidget *parent = nullptr )
      : QDialog( parent )
    {
      setWindowTitle( i18n("Search Error Log") );

      QVBoxLayout *mainLayout = new QVBoxLayout( this );

      mTextView = new QTextEdit( this );
      mTextView->setReadOnly( true );
      mTextView->setWordWrapMode( QTextOption::NoWrap );
      mainLayout->addWidget( mTextView );

      QDialogButtonBox *buttonBox = new QDialogButtonBox( QDialogButtonBox::Close );
      connect( buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept );
      connect( buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject );
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
    QTextEdit *mTextView = nullptr;
};


MainWindow::MainWindow()
    : KXmlGuiWindow(nullptr)
{
    setObjectName( QStringLiteral( "MainWindow" ) );

    QDBusConnection::sessionBus().registerObject(QStringLiteral("/KHelpCenter"), this, QDBusConnection::ExportScriptableSlots);
    mSplitter = new QSplitter( this );

    mDoc = new View( mSplitter, actionCollection() );
    connect( mDoc, &View::titleChanged, this, &MainWindow::setWindowTitle );
    connect( mDoc->page(), &QWebEnginePage::linkHovered, this, &MainWindow::statusBarMessage );
    connect( mDoc, &QWebEngineView::loadStarted, this, &MainWindow::slotStarted );
    connect( mDoc, &View::loadFinished, this, &MainWindow::documentCompleted );
    connect( mDoc, &View::searchResultCacheAvailable, this, &MainWindow::enableLastSearchAction );

    connect( mDoc, &View::selectionChanged, this, &MainWindow::enableCopyTextAction );

    statusBar()->showMessage(i18n("Preparing Index"));

    mNavigator = new Navigator( mDoc, mSplitter );
    mNavigator->setObjectName( QStringLiteral("nav") );
    connect( mNavigator, &Navigator::itemSelected, this, QOverload<const QString &>::of(&MainWindow::viewUrl) );
    connect( mNavigator, &Navigator::glossSelected, this, &MainWindow::slotGlossSelected );
    connect( mNavigator, &Navigator::setStatusBarText, this, &MainWindow::statusBarMessage );

    mSplitter->insertWidget(0, mNavigator);
    mSplitter->setStretchFactor(mSplitter->indexOf(mNavigator), 0);
    setCentralWidget( mSplitter );
    QList<int> sizes;
    sizes << 220 << 580;
    mSplitter->setSizes(sizes);

    setupActions();

    setupBookmarks();

    setupGUI(QSize(800, 600), ToolBar | Keys | StatusBar | Create);
    setAutoSaveSettings();

    History::self().installMenuBarHook( this );

    connect( &History::self(), &History::goInternalUrl, mNavigator, &Navigator::openInternalUrl );
    connect( &History::self(), &History::goUrl, mNavigator, &Navigator::selectItem );

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
    config.writePathEntry( "URL" , mDoc->url().toString() );
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
    actionCollection()->addAction( KStandardAction::Quit, this, SLOT(close()) );
    actionCollection()->addAction( KStandardAction::Print, this, SLOT(slotPrint()) );
    actionCollection()->addAction( KStandardAction::PrintPreview, this, SLOT(slotPrintPreview()) );

    KStandardAction::fullScreen( this, SLOT( slotFullScreen() ), this, actionCollection() );

    QAction *home = KStandardAction::home( this, SLOT(slotShowHome()), this );
    actionCollection()->addAction( home->objectName(), home );
    home->setText(i18n("Table of &Contents"));
    home->setToolTip(i18n("Table of contents"));
    home->setWhatsThis(i18n("Go back to the table of contents"));

    mCopyText = KStandardAction::copy( this, SLOT(slotCopySelectedText()), this );
    actionCollection()->addAction( QStringLiteral("copy_text"), mCopyText );

    mLastSearchAction = actionCollection()->addAction( QStringLiteral("lastsearch") );
    mLastSearchAction->setText( i18n("&Last Search Result") );
    mLastSearchAction->setEnabled( false );
    connect( mLastSearchAction, &QAction::triggered, this, &MainWindow::slotLastSearch );
/*
    QAction *action = actionCollection()->addAction( QLatin1String("build_index") );
    action->setText( i18n("Build Search Index...") );
    connect( action, &QAction::triggered, mNavigator, &Navigator::showIndexDialog );

    KConfigGroup debugGroup( KSharedConfig::openConfig(), "Debug" );
    if ( debugGroup.readEntry( "SearchErrorLog", false) ) {
        action = actionCollection()->addAction(QLatin1String("show_search_stderr"));
        action->setText( i18n("Show Search Error Log") );
        connect( action, &QAction::triggered, this, &View::showSearchStderr );
    }
*/
    History::self().setupActions( actionCollection() );

}

void MainWindow::setupBookmarks()
{
    const QString location = QStandardPaths::writableLocation( QStandardPaths::AppDataLocation );
    QDir().mkpath( location );
    const QString file = location + QStringLiteral( "/bookmarks.xml" );

    KBookmarkManager *manager = KBookmarkManager::managerForFile( file, QStringLiteral( "khelpcenter" ) );
    manager->setParent( this );
    BookmarkOwner *owner = new BookmarkOwner( mDoc, manager );
    connect( owner, &BookmarkOwner::openUrl, this, QOverload<const QUrl &>::of(&MainWindow::openUrl) );
    KActionMenu *actmenu = actionCollection()->add<KActionMenu>( QStringLiteral( "bookmarks" ) );
    actmenu->setText( i18nc( "@title:menu", "&Bookmarks" ) );
    auto *bookmenu = new KBookmarkMenu( manager, owner, actmenu->menu());
    actionCollection()->addActions(actmenu->menu()->actions());
    bookmenu->setParent( owner );
}

void MainWindow::slotCopySelectedText()
{
  mDoc->copySelectedText();
}

void MainWindow::slotStarted()
{
    History::self().updateActions();
}

void MainWindow::goInternalUrl( const QUrl &url )
{
  mDoc->stop();
  slotOpenURLRequest( url );
}

void MainWindow::slotOpenURLRequest( const QUrl &url)
{
  qCDebug(KHC_LOG) << url.url();

  mNavigator->selectItem( url );
  viewUrl( url );
}

void MainWindow::viewUrl( const QString &url )
{
  viewUrl( QUrl( url ) );
}

void MainWindow::viewUrl( const QUrl &url )
{
    stop();

    QString proto = url.scheme().toLower();

    if ( proto == QLatin1String("khelpcenter") ) {
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
        if ( mime.inherits(QStringLiteral("text/html")) )
            own = true;
    }

    if ( !own ) {
        auto *job = new KIO::OpenUrlJob(QUrl(url));
        job->setUiDelegate(KIO::createDefaultJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled, this));
        job->start();
        return;
    }

    History::self().createEntry();

    if ( proto == QLatin1String("glossentry") ) {
        QString decodedEntryId = QUrl::fromPercentEncoding( QUrl::toPercentEncoding(url.path()) );
        slotGlossSelected( mNavigator->glossEntry( decodedEntryId ) );
        mNavigator->slotSelectGlossEntry( decodedEntryId );
    } else {
        mDoc->load( url );
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

void MainWindow::statusBarRichTextMessage(const QString &m)
{
    QTextDocument richTextHolder;
    richTextHolder.setHtml(m);
    statusBar()->showMessage(richTextHolder.toPlainText());
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
    setAttribute(Qt::WA_NativeWindow, true);
    KStartupInfo::setNewStartupId(windowHandle(), startup_id);
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
    mDoc->setInternalHtml( mDoc->grantleeFormatter()->formatGlossaryEntry( entry ), QUrl( QStringLiteral("glossentry:") + entry.id() ) );
}

void MainWindow::stop()
{
    mDoc->stop();
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

void MainWindow::slotFullScreen()
{
  if (!mFullScreen) {
    KToggleFullScreenAction::setFullScreen(this, true);
    mFullScreen = true;
  } else {
    KToggleFullScreenAction::setFullScreen(this, false);
    mFullScreen = false;
  }
}

void MainWindow::slotPrint()
{
    auto printDoc = new PrintDoc(this);
    printDoc->setParentWidget(this);
    printDoc->setView(mDoc);
    printDoc->print();
}

void MainWindow::slotPrintPreview()
{
    auto printDoc = new PrintDoc(this);
    printDoc->setParentWidget(this);
    printDoc->setView(mDoc);
    printDoc->printPreview();
}


// vim:ts=2:sw=2:et
