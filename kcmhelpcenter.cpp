/*
  This file is part of KHelpcenter.

  Copyright (C) 2002 Cornelius Schumacher <schumacher@kde.org>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; see the file COPYING.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/

#include "kcmhelpcenter.h"

#include "htmlsearchconfig.h"
#include "docmetainfo.h"
#include "prefs.h"
#include "searchhandler.h"
#include "searchengine.h"


#include "kcmhelpcenteradaptor.h"

#include <kconfig.h>
#include <kdebug.h>
#include <klocale.h>
#include <kglobal.h>
#include <kaboutdata.h>
#include <kdialog.h>
#include <kstandarddirs.h>
#include <kprocess.h>
#include <kapplication.h>
#include <ktemporaryfile.h>
#include <kurlrequester.h>
#include <kmessagebox.h>
#include <k3listview.h>
#include <klineedit.h>

#include <QtDBus/QtDBus>
#include <QLabel>
#include <QLayout>
#include <QProgressBar>
#include <QTextEdit>

#include <unistd.h>
#include <sys/types.h>

using namespace KHC;

IndexDirDialog::IndexDirDialog( QWidget *parent )
  : KDialog( parent )
{
  setModal( true );
  setCaption( i18n("Change Index Folder") );
  setButtons( Ok | Cancel );

  QFrame *topFrame = new QFrame( this );
  setMainWidget( topFrame );

  QBoxLayout *urlLayout = new QHBoxLayout( topFrame );

  QLabel *label = new QLabel( i18n("Index folder:"), topFrame );
  urlLayout->addWidget( label );

  mIndexUrlRequester = new KUrlRequester( topFrame );
  mIndexUrlRequester->setMode( KFile::Directory | KFile::ExistingOnly |
                               KFile::LocalOnly );
  urlLayout->addWidget( mIndexUrlRequester );

  mIndexUrlRequester->setUrl( Prefs::indexDirectory() );
  connect(mIndexUrlRequester->lineEdit(),SIGNAL(textChanged ( const QString & )), this, SLOT(slotUrlChanged( const QString &)));
  slotUrlChanged( mIndexUrlRequester->lineEdit()->text() );

  connect( this, SIGNAL( okClicked() ), SLOT( slotOk() ) );
}

void IndexDirDialog::slotUrlChanged( const QString &_url )
{
  enableButtonOk( !_url.isEmpty() );
}


void IndexDirDialog::slotOk()
{
  Prefs::setIndexDirectory( mIndexUrlRequester->url().url() );
  accept();
}


IndexProgressDialog::IndexProgressDialog( QWidget *parent )
  : KDialog( parent ),
    mFinished( true )
{
  setCaption( i18n("Build Search Indices") );

  QVBoxLayout *topLayout = new QVBoxLayout( mainWidget() );
  topLayout->setMargin( marginHint() );
  topLayout->setSpacing( spacingHint() );

  mLabel = new QLabel( mainWidget() );
  mLabel->setAlignment( Qt::AlignHCenter );
  topLayout->addWidget( mLabel );

  mProgressBar = new QProgressBar( mainWidget() );
  topLayout->addWidget( mProgressBar );

  mLogLabel = new QLabel( i18n("Index creation log:"), mainWidget() );
  topLayout->addWidget( mLogLabel );

  mLogView = new QTextEdit( mainWidget() );
  mLogView->setReadOnly( true );
  mLogView->setTextFormat( Qt::LogText );
  mLogView->setMinimumHeight( 200 );
  topLayout->addWidget( mLogView );

  setButtons( User1 | Close );
  connect( this, SIGNAL( closeClicked() ), SLOT( slotEnd() ) );
  connect( this, SIGNAL( user1Clicked() ), SLOT( toggleDetails() ) );

  hideDetails();

  setFinished( false );
}

IndexProgressDialog::~IndexProgressDialog()
{
  if ( !mLogView->isHidden() ) {
    KConfigGroup cfg(KGlobal::config(), "indexprogressdialog");
    cfg.writeEntry( "size", size() );
  }
}

void IndexProgressDialog::setTotalSteps( int steps )
{
  mProgressBar->setRange( 0, steps );
  mProgressBar->setValue( 0 );
  setFinished( false );
  mLogView->clear();
}

void IndexProgressDialog::advanceProgress()
{
  mProgressBar->setValue( mProgressBar->value() + 1 );
}

void IndexProgressDialog::setLabelText( const QString &text )
{
  mLabel->setText( text );
}

void IndexProgressDialog::setMinimumLabelWidth( int width )
{
  mLabel->setMinimumWidth( width );
}

void IndexProgressDialog::setFinished( bool finished )
{
  if ( finished == mFinished ) return;

  mFinished = finished;

  if ( mFinished ) {
    setButtonText( Close, i18nc("Label for button to close search index progress dialog after successful completion", "Close") );
    mLabel->setText( i18n("Index creation finished.") );
    mProgressBar->setValue( mProgressBar->maximum() );
  } else {
    setButtonText( Close, i18nc("Label for stopping search index generation before completion", "Stop") );
  }
}

void IndexProgressDialog::appendLog( const QString &text )
{
  mLogView->append( text );
}

void IndexProgressDialog::slotEnd()
{
  if ( mFinished ) {
    emit closed();
    accept();
  } else {
    emit cancelled();
    reject();
  }
}

void IndexProgressDialog::toggleDetails()
{
  KConfigGroup cfg(KGlobal::config(), "indexprogressdialog");
  if ( mLogView->isHidden() ) {
    mLogLabel->show();
    mLogView->show();
    setButtonText( User1, i18n("Details &lt;&lt;") );
    QSize size = cfg.readEntry( "size", QSize() );
    if ( !size.isEmpty() ) resize( size );
  } else {
    cfg.writeEntry( "size", size() );
    hideDetails();
  }
}

void IndexProgressDialog::hideDetails()
{
  mLogLabel->hide();
  mLogView->hide();
  setButtonText( User1, i18n("Details &gt;&gt;") );

  // causes bug 166343
  //layout()->activate();
  adjustSize();
}


KCMHelpCenter::KCMHelpCenter( KHC::SearchEngine *engine, QWidget *parent,
  const char *name)
  : KDialog( parent ),
    mEngine( engine ), mProgressDialog( 0 ), mCmdFile( 0 ),
    mProcess( 0 ), mIsClosing( false ), mRunAsRoot( false )
{
  new KcmhelpcenterAdaptor(this);
  QDBusConnection::sessionBus().registerObject(QLatin1String("/kcmhelpcenter"), this);
  setObjectName( name );
  setCaption( i18n("Build Search Index") );
  setButtons( Ok | Cancel );
  showButtonSeparator( true );

  QWidget *widget = new QWidget( this );
  setMainWidget( widget );

  setupMainWidget( widget );

  setButtonGuiItem( KDialog::Ok, KGuiItem(i18n("Build Index")) );

  mConfig = KGlobal::config();

  DocMetaInfo::self()->scanMetaInfo();

  load();
  const QString dbusInterface = "org.kde.khelpcenter.kcmhelpcenter";
  QDBusConnection dbus = QDBusConnection::sessionBus();
  bool success = dbus.connect(QString(), "/kcmhelpcenter", dbusInterface, "buildIndexProgress", this, SLOT(slotIndexProgress()));
  if ( !success )
    kError() << "connect D-Bus signal failed" << endl;
  success = dbus.connect(QString(), "/kcmhelpcenter", dbusInterface, "buildIndexError", this, SLOT(slotIndexError(const QString&)));
  if ( !success )
    kError() << "connect D-Bus signal failed" << endl;
  KConfigGroup id( mConfig, "IndexDialog" );
  restoreDialogSize( id );
}

KCMHelpCenter::~KCMHelpCenter()
{
  KConfigGroup cg( KGlobal::config(), "IndexDialog" );
  KDialog::saveDialogSize( cg );
}

void KCMHelpCenter::setupMainWidget( QWidget *parent )
{
  QVBoxLayout *topLayout = new QVBoxLayout( parent );
  topLayout->setSpacing( KDialog::spacingHint() );

  QString helpText =
    i18n("To be able to search a document, a search\n"
         "index needs to exist. The status column of the list below shows whether an index\n"
         "for a document exists.\n") +
    i18n("To create an index, check the box in the list and press the\n"
         "\"Build Index\" button.\n");

  QLabel *label = new QLabel( helpText, parent );
  topLayout->addWidget( label );

  mListView = new K3ListView( parent );
  mListView->setFullWidth( true );
  mListView->addColumn( i18n("Search Scope") );
  mListView->addColumn( i18n("Status") );
  mListView->setColumnAlignment( 1, Qt::AlignCenter );
  topLayout->addWidget( mListView );
  connect( mListView, SIGNAL( clicked( Q3ListViewItem * ) ),
    SLOT( checkSelection() ) );

  QBoxLayout *urlLayout = new QHBoxLayout();
  topLayout->addLayout( urlLayout );

  QLabel *urlLabel = new QLabel( i18n("Index folder:"), parent );
  urlLayout->addWidget( urlLabel );

  mIndexDirLabel = new QLabel( parent );
  urlLayout->addWidget( mIndexDirLabel, 1 );

  QPushButton *button = new QPushButton( i18n("Change..."), parent );
  connect( button, SIGNAL( clicked() ), SLOT( showIndexDirDialog() ) );
  urlLayout->addWidget( button );

  QBoxLayout *buttonLayout = new QHBoxLayout();
  topLayout->addLayout( buttonLayout );

  buttonLayout->addStretch( 1 );

  connect( this, SIGNAL( okClicked() ), SLOT( slotOk() ) );
}

void KCMHelpCenter::defaults()
{
}

bool KCMHelpCenter::save()
{
  kDebug(1401) << "KCMHelpCenter::save()";

  if ( !QFile::exists( Prefs::indexDirectory() ) ) {
    KMessageBox::sorry( this,
      i18n("<qt>The folder <b>%1</b> does not exist; unable to create index.</qt>",
        Prefs::indexDirectory() ) );
    return false;
  } else {
    return buildIndex();
  }

  return true;
}

void KCMHelpCenter::load()
{
  mIndexDirLabel->setText( Prefs::indexDirectory() );

  mListView->clear();

  const DocEntry::List &entries = DocMetaInfo::self()->docEntries();
  DocEntry::List::ConstIterator it;
  for( it = entries.begin(); it != entries.end(); ++it ) {
//    kDebug(1401) << "Entry: " << (*it)->name() << " Indexer: '"
//              << (*it)->indexer() << "'" << endl;
    if ( mEngine->needsIndex( *it ) ) {
      ScopeItem *item = new ScopeItem( mListView, *it );
      item->setOn( (*it)->searchEnabled() );
    }
  }

  updateStatus();
}

void KCMHelpCenter::updateStatus()
{
  Q3ListViewItemIterator it( mListView );
  while ( it.current() != 0 ) {
    ScopeItem *item = static_cast<ScopeItem *>( it.current() );
    QString status;
    if ( item->entry()->indexExists( Prefs::indexDirectory() ) ) {
      status = i18nc("Describes the status of a documentation index that is present", "OK");
      item->setOn( false );
    } else {
      status = i18nc("Describes the status of a documentation index that is missing", "Missing");
    }
    item->setText( 1, status );

    ++it;
  }

  checkSelection();
}

bool KCMHelpCenter::buildIndex()
{
  kDebug(1401) << "Build Index";

  kDebug() << "IndexPath: '" << Prefs::indexDirectory() << "'";

  if ( mProcess ) {
    kError() << "Error: Index Process still running." << endl;
    return false;
  }

  mIndexQueue.clear();

  QFontMetrics fm( font() );
  int maxWidth = 0;

  mCmdFile = new KTemporaryFile;
  if ( !mCmdFile->open() ) {
    kError() << "Error opening command file." << endl;
    deleteCmdFile();
    return false;
  }

  QTextStream ts ( mCmdFile );
  kDebug() << "Writing to file '" << mCmdFile->fileName() << "'";

  bool hasError = false;

  Q3ListViewItemIterator it( mListView );
  while ( it.current() != 0 ) {
    ScopeItem *item = static_cast<ScopeItem *>( it.current() );
    if ( item->isOn() ) {
      DocEntry *entry = item->entry();

      QString docText = i18nc(" Generic prefix label for error messages when creating documentation index, first arg is the document's identifier, second is the document's name", "Document '%1' (%2):\n",
          entry->identifier() ,
          entry->name() );
      if ( entry->documentType().isEmpty() ) {
        KMessageBox::sorry( this, docText +
          i18n("No document type.") );
        hasError = true;
      } else {
        SearchHandler *handler = mEngine->handler( entry->documentType() );
        if ( !handler ) {
          KMessageBox::sorry( this, docText +
            i18n("No search handler available for document type '%1'.",
              entry->documentType() ) );
          hasError = true;
        } else {
          QString indexer = handler->indexCommand( entry->identifier() );
          if ( indexer.isEmpty() ) {
            KMessageBox::sorry( this, docText +
              i18n("No indexing command specified for document type '%1'.",
                entry->documentType() ) );
            hasError = true;
          } else {
            indexer.replace( QLatin1String("%i" ), entry->identifier() );
            indexer.replace( QLatin1String( "%d" ), Prefs::indexDirectory() );
            indexer.replace( QLatin1String( "%p" ), entry->url() );
            kDebug() << "INDEXER: " << indexer;
            ts << indexer << endl;

            int width = fm.width( entry->name() );
            if ( width > maxWidth ) maxWidth = width;

            mIndexQueue.append( entry );
          }
        }
      }
    }
    ++it;
  }

  ts.flush();

  if ( mIndexQueue.isEmpty() ) {
    deleteCmdFile();
    return !hasError;
  }

  mCurrentEntry = mIndexQueue.begin();
  QString name = (*mCurrentEntry)->name();

  if ( !mProgressDialog ) {
    mProgressDialog = new IndexProgressDialog( parentWidget() );
    connect( mProgressDialog, SIGNAL( cancelled() ),
             SLOT( cancelBuildIndex() ) );
    connect( mProgressDialog, SIGNAL( closed() ),
             SLOT( slotProgressClosed() ) );
  }
  mProgressDialog->setLabelText( name );
  mProgressDialog->setTotalSteps( mIndexQueue.count() );
  mProgressDialog->setMinimumLabelWidth( maxWidth );
  mProgressDialog->show();

  startIndexProcess();

  return true;
}

void KCMHelpCenter::startIndexProcess()
{
  kDebug() << "KCMHelpCenter::startIndexProcess()";

  mProcess = new KProcess;
  if ( mRunAsRoot ) {
    QString kdesu = KStandardDirs::findExe("kdesu"); 
    if(kdesu.isEmpty()) {
      kError() << "Failed to run index process as root - could not find kdesu";
    } else {
      *mProcess << kdesu;
      if(parentWidget()) {
        *mProcess << "--attach" <<  QString::number(parentWidget()->window()->winId());
        kDebug() << "Run as root, attaching kdesu to winid " << QString::number(parentWidget()->window()->winId());
      }
      *mProcess << "--";
    }
  }

  *mProcess << KStandardDirs::findExe("khc_indexbuilder");
  *mProcess << mCmdFile->fileName();
  *mProcess << Prefs::indexDirectory();

  mProcess->setOutputChannelMode(KProcess::SeparateChannels);
  connect( mProcess, SIGNAL( readyReadStandardError() ),
           SLOT( slotReceivedStdout() ) );
  connect( mProcess, SIGNAL( readyReadStandardOutput() ),
           SLOT( slotReceivedStderr() ) );
  connect( mProcess, SIGNAL( finished(int, QProcess::ExitStatus) ),
           SLOT( slotIndexFinished(int, QProcess::ExitStatus) ) );

  mProcess->start();
  if (!mProcess->waitForStarted()) {
    kError() << "KCMHelpcenter::startIndexProcess(): Failed to start process.";
    deleteProcess();
    deleteCmdFile();
  }
}

void KCMHelpCenter::cancelBuildIndex()
{
  kDebug() << "cancelBuildIndex()";

  deleteProcess();
  deleteCmdFile();
  mIndexQueue.clear();

  if ( mIsClosing ) {
    mIsClosing = false;
  }
}

void KCMHelpCenter::slotIndexFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
  kDebug() << "KCMHelpCenter::slotIndexFinished()";

  if ( exitStatus == QProcess::NormalExit && exitCode == 2 ) {
    if ( mRunAsRoot ) {
      kError() << "Insufficient permissions." << endl;
    } else {
      kDebug() << "Insufficient permissions. Trying again as root.";
      mRunAsRoot = true;
      deleteProcess();
      startIndexProcess();
      return;
    }
  } else if ( exitStatus != QProcess::NormalExit || exitCode != 0 ) {
    kDebug() << "KProcess reported an error.";
    KMessageBox::error( this, i18n("Failed to build index.") );
  } else {
    mConfig->group( "Search" ).writeEntry( "IndexExists", true );
    emit searchIndexUpdated();
  }

  deleteProcess();
  deleteCmdFile();

  if ( mProgressDialog ) {
    mProgressDialog->setFinished( true );
  }

  mStdOut.clear();
  mStdErr.clear();

  if ( mIsClosing ) {
    if ( !mProgressDialog || !mProgressDialog->isVisible() ) {
      mIsClosing = false;
      accept();
    }
  }
}

void KCMHelpCenter::deleteProcess()
{
  delete mProcess;
  mProcess = 0;
}

void KCMHelpCenter::deleteCmdFile()
{
  delete mCmdFile;
  mCmdFile = 0;
}

void KCMHelpCenter::slotIndexProgress()
{
  if( !mProcess )
    return;

  kDebug() << "KCMHelpCenter::slotIndexProgress()";

  updateStatus();

  advanceProgress();
}

void KCMHelpCenter::slotIndexError( const QString &str )
{
  if( !mProcess )
    return;

  kDebug() << "KCMHelpCenter::slotIndexError()";

  KMessageBox::sorry( this, i18n("Error executing indexing build command:\n%1",
      str ) );

  if ( mProgressDialog ) {
    mProgressDialog->appendLog( "<i>" + str + "</i>" );
  }

  advanceProgress();
}

void KCMHelpCenter::advanceProgress()
{
  if ( mProgressDialog && mProgressDialog->isVisible() ) {
    mProgressDialog->advanceProgress();
    mCurrentEntry++;
    if ( mCurrentEntry != mIndexQueue.end() ) {
      QString name = (*mCurrentEntry)->name();
      mProgressDialog->setLabelText( name );
    }
  }
}

void KCMHelpCenter::slotReceivedStdout()
{
  QByteArray text= mProcess->readAllStandardOutput();
  int pos = text.lastIndexOf( '\n' );
  if ( pos < 0 ) {
    mStdOut.append( text );
  } else {
    if ( mProgressDialog ) {
      mProgressDialog->appendLog( mStdOut + text.left( pos ) );
      mStdOut = text.mid( pos + 1 );
    }
  }
}

void KCMHelpCenter::slotReceivedStderr( )
{
  QByteArray text = mProcess->readAllStandardError();
  int pos = text.lastIndexOf( '\n' );
  if ( pos < 0 ) {
    mStdErr.append( text );
  } else {
    if ( mProgressDialog ) {
      mProgressDialog->appendLog( QLatin1String("<i>") + mStdErr + text.left( pos ) +
                                  QLatin1String("</i>"));
      mStdErr = text.mid( pos + 1 );
    }
  }
}

void KCMHelpCenter::slotOk()
{
  if ( buildIndex() ) {
    if ( !mProcess ) accept();
    else mIsClosing = true;
  }
}

void KCMHelpCenter::slotProgressClosed()
{
  kDebug() << "KCMHelpCenter::slotProgressClosed()";

  if ( mIsClosing ) accept();
}

void KCMHelpCenter::showIndexDirDialog()
{
  IndexDirDialog dlg( this );
  if ( dlg.exec() == QDialog::Accepted ) {
    load();
  }
}

void KCMHelpCenter::checkSelection()
{
  int count = 0;

  Q3ListViewItemIterator it( mListView );
  while ( it.current() != 0 ) {
    ScopeItem *item = static_cast<ScopeItem *>( it.current() );
    if ( item->isOn() ) {
      ++count;
    }
    ++it;
  }

  enableButtonOk( count != 0 );
}

#include "kcmhelpcenter.moc"

// vim:ts=2:sw=2:et
