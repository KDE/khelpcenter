/*
  This file is part of KHelpcenter.

  Copyright (C) 2002 Cornelius Schumacher <schumacher@kde.org>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public
  License version 2 as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; see the file COPYING.  If not, write to
  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
  Boston, MA 02111-1307, USA.
*/

#include "kcmhelpcenter.h"

#include "htmlsearchconfig.h"
#include "docmetainfo.h"
#include "prefs.h"
#include "searchhandler.h"
#include "searchengine.h"

#include <kconfig.h>
#include <kdebug.h>
#include <klocale.h>
#include <kglobal.h>
#include <kaboutdata.h>
#include <kdialog.h>
#include <kstandarddirs.h>
#include <kprocess.h>
#include <kapplication.h>
#include <dcopclient.h>
#include <ktempfile.h>
#include <kurlrequester.h>
#include <kmessagebox.h>
#include <klistview.h>

#include <qlayout.h>
#include <qpushbutton.h>
#include <qdir.h>
#include <qtabwidget.h>
#include <qprogressbar.h>
#include <qfile.h>
#include <qlabel.h>
#include <qvbox.h>
#include <qtextedit.h>
#include <qregexp.h>

#include <unistd.h>
#include <sys/types.h>

using namespace KHC;

IndexDirDialog::IndexDirDialog( QWidget *parent )
  : KDialogBase( parent, 0, true, i18n("Change Index Folder"), Ok | Cancel )
{
  QFrame *topFrame = makeMainWidget();

  QBoxLayout *urlLayout = new QHBoxLayout( topFrame );
  
  QLabel *label = new QLabel( i18n("Index Folder:"), topFrame );
  urlLayout->addWidget( label );
  
  mIndexUrlRequester = new KURLRequester( topFrame );
  mIndexUrlRequester->setMode( KFile::Directory | KFile::ExistingOnly |
                               KFile::LocalOnly );
  urlLayout->addWidget( mIndexUrlRequester );

  mIndexUrlRequester->setURL( Prefs::indexDirectory() );
}

void IndexDirDialog::slotOk()
{
  Prefs::setIndexDirectory( mIndexUrlRequester->url() );
  accept();
}


IndexProgressDialog::IndexProgressDialog( QWidget *parent )
  : KDialog( parent, "IndexProgressDialog", true ),
    mFinished( true )
{
  setCaption( i18n("Build Search Indices") );

  QBoxLayout *topLayout = new QVBoxLayout( this );
  topLayout->setMargin( marginHint() );
  topLayout->setSpacing( spacingHint() );

  mLabel = new QLabel( this );
  mLabel->setAlignment( AlignHCenter );
  topLayout->addWidget( mLabel );

  mProgressBar = new QProgressBar( this );
  topLayout->addWidget( mProgressBar );

  mLogLabel = new QLabel( i18n("Index creation log:"), this );
  topLayout->addWidget( mLogLabel );

  mLogView = new QTextEdit( this );
  mLogView->setTextFormat( LogText );
  mLogView->setMinimumHeight( 200 );
  topLayout->addWidget( mLogView, 1 );

  QBoxLayout *buttonLayout = new QHBoxLayout( topLayout );

  buttonLayout->addStretch( 1 );

  mDetailsButton = new QPushButton( this );
  connect( mDetailsButton, SIGNAL( clicked() ), SLOT( toggleDetails() ) );
  buttonLayout->addWidget( mDetailsButton );

  toggleDetails();

  mEndButton = new QPushButton( this );
  connect( mEndButton, SIGNAL( clicked() ), SLOT( slotEnd() ) );
  buttonLayout->addWidget( mEndButton );

  setFinished( false );
}

void IndexProgressDialog::setTotalSteps( int steps )
{
  mProgressBar->setTotalSteps( steps );
  mProgressBar->setProgress( 0 );
  setFinished( false );
  mLogView->clear();
}

void IndexProgressDialog::advanceProgress()
{
  mProgressBar->setProgress( mProgressBar->progress() + 1 );
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
    mEndButton->setText( i18n("Close") );
    mLabel->setText( i18n("Index creation finished.") );
  } else {
    mEndButton->setText( i18n("Stop") );
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
  if ( mLogView->isHidden() ) {
    mLogLabel->show();
    mLogView->show();
    mDetailsButton->setText( i18n("Details <<") );
  } else {
    mLogLabel->hide();
    mLogView->hide();
    mDetailsButton->setText( i18n("Details >>") );
    layout()->activate();
    adjustSize();
  }
}


KCMHelpCenter::KCMHelpCenter( KHC::SearchEngine *engine, QWidget *parent,
  const char *name)
  : DCOPObject( "kcmhelpcenter" ),
    KDialogBase( parent, name, false, i18n("Build Search Index"),
      Ok | Cancel, Ok, true ),
    mEngine( engine ), mProgressDialog( 0 ), mCurrentEntry( 0 ), mCmdFile( 0 ),
    mProcess( 0 ), mIsClosing( false ), mRunAsRoot( false )
{
  QWidget *widget = makeMainWidget();

  setupMainWidget( widget );

  setButtonOK( i18n("Build Index") );

  mConfig = KGlobal::config();

  DocMetaInfo::self()->scanMetaInfo();

  load();

  bool success = kapp->dcopClient()->connectDCOPSignal( "khc_indexbuilder",
      0, "buildIndexProgress()", "kcmhelpcenter",
      "slotIndexProgress()", false );
  if ( !success ) kdError() << "connect DCOP signal failed" << endl;

  success = kapp->dcopClient()->connectDCOPSignal( "khc_indexbuilder",
      0, "buildIndexError(QString)", "kcmhelpcenter",
      "slotIndexError(QString)", false );
  if ( !success ) kdError() << "connect DCOP signal failed" << endl;

  resize( configDialogSize( "IndexDialog" ) );
}

KCMHelpCenter::~KCMHelpCenter()
{
  saveDialogSize( "IndexDialog" );
}

void KCMHelpCenter::setupMainWidget( QWidget *parent )
{
  QVBoxLayout *topLayout = new QVBoxLayout( parent );
  topLayout->setSpacing( KDialog::spacingHint() );

  QString helpText =
    i18n("To be able to search a document there needs to exist a search\n"
         "index. This status column of the list below shows, if an index.\n"
         "for a document exists.\n") + 
    i18n("To create an index check the box in the list and press the\n"
         "\"Build Index\" button.\n");

  QLabel *label = new QLabel( helpText, parent );
  topLayout->addWidget( label );

  mListView = new KListView( parent );
  mListView->setFullWidth( true );
  mListView->addColumn( i18n("Search Scope") );
  mListView->addColumn( i18n("Status") );
  mListView->setColumnAlignment( 1, AlignCenter );
  topLayout->addWidget( mListView );

  QBoxLayout *urlLayout = new QHBoxLayout( topLayout );

  QLabel *urlLabel = new QLabel( i18n("Index folder:"), parent );
  urlLayout->addWidget( urlLabel );

  mIndexDirLabel = new QLabel( parent );
  urlLayout->addWidget( mIndexDirLabel, 1 );
  
  QPushButton *button = new QPushButton( i18n("Change..."), parent );
  connect( button, SIGNAL( clicked() ), SLOT( showIndexDirDialog() ) );
  urlLayout->addWidget( button );

  QBoxLayout *buttonLayout = new QHBoxLayout( topLayout );

  buttonLayout->addStretch( 1 );
}

void KCMHelpCenter::defaults()
{
}

bool KCMHelpCenter::save()
{
  kdDebug(1401) << "KCMHelpCenter::save()" << endl;

  if ( !QFile::exists( Prefs::indexDirectory() ) ) {
    KMessageBox::sorry( this,
      i18n("<qt>The folder <b>%1</b> does not exist; unable to create index.</qt>")
      .arg( Prefs::indexDirectory() ) );
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

  DocEntry::List entries = DocMetaInfo::self()->docEntries();
  DocEntry::List::ConstIterator it;
  for( it = entries.begin(); it != entries.end(); ++it ) {
//    kdDebug(1401) << "Entry: " << (*it)->name() << " Indexer: '"
//              << (*it)->indexer() << "'" << endl;
    if ( (*it)->docExists() && !(*it)->indexer().isEmpty() ) {
      ScopeItem *item = new ScopeItem( mListView, *it );
      item->setOn( (*it)->searchEnabled() );
    }
  }

  updateStatus();
}

void KCMHelpCenter::updateStatus()
{
  QListViewItemIterator it( mListView );
  while ( it.current() != 0 ) {
    ScopeItem *item = static_cast<ScopeItem *>( it.current() );
    QString status;
    if ( item->entry()->indexExists( Prefs::indexDirectory() ) ) {
      status = i18n("OK");
      item->setOn( false );
    } else {
      status = i18n("Missing");
    }
    item->setText( 1, status );

    ++it;
  }
}

bool KCMHelpCenter::buildIndex()
{
  kdDebug(1401) << "Build Index" << endl;

  kdDebug() << "IndexPath: '" << Prefs::indexDirectory() << "'" << endl;

  if ( mProcess ) {
    kdError() << "Error: Index Process still running." << endl;
    return false;
  }

  mIndexQueue.clear();

  QFontMetrics fm( font() );
  int maxWidth = 0;

  mCmdFile = new KTempFile;
  mCmdFile->setAutoDelete( true );
  QTextStream *ts = mCmdFile->textStream();
  if ( !ts ) {
    kdError() << "Error opening command file." << endl;
    deleteCmdFile();
    return false;
  } else {
    kdDebug() << "Writing to file '" << mCmdFile->name() << "'" << endl;
  }

  bool hasError = false;

  QListViewItemIterator it( mListView );
  while ( it.current() != 0 ) {
    ScopeItem *item = static_cast<ScopeItem *>( it.current() );
    if ( item->isOn() ) {
      DocEntry *entry = item->entry();

      QString docText = i18n("Document '%1' (%2):\n")
        .arg( entry->identifier() )
        .arg( entry->name() );
      if ( entry->documentType().isEmpty() ) {
        KMessageBox::sorry( this, docText + 
          i18n("No document type.") );
        hasError = true;
      } else {
        SearchHandler *handler = mEngine->handler( entry->documentType() );
        if ( !handler ) {
          KMessageBox::sorry( this, docText +  
            i18n("No search handler available for document type '%1'.")
            .arg( entry->documentType() ) ); 
          hasError = true;
        } else {
          QString indexer = handler->indexCommand( entry->identifier() );
          if ( indexer.isEmpty() ) {
            KMessageBox::sorry( this, docText +
              i18n("No indexing command specified for document type '%1'.")
              .arg( entry->documentType() ) );
            hasError = true;
          } else {
            indexer.replace( QRegExp( "%i" ), entry->identifier() );
            indexer.replace( QRegExp( "%d" ), Prefs::indexDirectory() );
            indexer.replace( QRegExp( "%p" ), entry->url() );
            kdDebug() << "INDEXER: " << indexer << endl;
            *ts << indexer << endl;

            int width = fm.width( entry->name() );
            if ( width > maxWidth ) maxWidth = width;

            mIndexQueue.append( entry );
          }
        }
      }
    }
    ++it;
  }

  mCmdFile->close();

  if ( mIndexQueue.isEmpty() ) {
    deleteCmdFile();
    return !hasError;
  }

  mCurrentEntry = mIndexQueue.begin();
  QString name = (*mCurrentEntry)->name();

  if ( !mProgressDialog ) {
    mProgressDialog = new IndexProgressDialog( this );
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
  kdDebug() << "KCMHelpCenter::startIndexProcess()" << endl;

  mProcess = new KProcess;

  if ( mRunAsRoot ) {
    *mProcess << "kdesu" << "--nonewdcop";
    kdDebug() << "Run as root" << endl;
  }
  
  *mProcess << "khc_indexbuilder";
  *mProcess << mCmdFile->name();
  *mProcess << Prefs::indexDirectory();

  connect( mProcess, SIGNAL( processExited( KProcess * ) ),
           SLOT( slotIndexFinished( KProcess * ) ) );
  connect( mProcess, SIGNAL( receivedStdout( KProcess *, char *, int ) ),
           SLOT( slotReceivedStdout(KProcess *, char *, int ) ) );
  connect( mProcess, SIGNAL( receivedStderr( KProcess *, char *, int ) ),
           SLOT( slotReceivedStderr( KProcess *, char *, int ) ) );

  if ( !mProcess->start( KProcess::NotifyOnExit, KProcess::AllOutput ) ) {
    kdError() << "KCMHelpcenter::startIndexProcess(): Failed to start process."
      << endl;
  }
}

void KCMHelpCenter::cancelBuildIndex()
{
  kdDebug() << "cancelBuildIndex()" << endl;

  deleteProcess();
  deleteCmdFile();
  mIndexQueue.clear();

  if ( mIsClosing ) {
    mIsClosing = false;
  }
}

void KCMHelpCenter::slotIndexFinished( KProcess *proc )
{
  kdDebug() << "KCMHelpCenter::slotIndexFinished()" << endl;

  if ( proc == 0 ) {
    kdWarning() << "Process null." << endl;
    return;
  }

  if ( proc != mProcess ) {
    kdError() << "Unexpected Process finished." << endl;
    return;
  }

  if ( mProcess->normalExit() && mProcess->exitStatus() == 2 ) {
    if ( mRunAsRoot ) {
      kdError() << "Insufficient permissions." << endl;
    } else {
      kdDebug() << "Insufficient permissions. Trying again as root." << endl;
      mRunAsRoot = true;
      deleteProcess();
      startIndexProcess();
      return;
    }
  } else if ( !mProcess->normalExit() || mProcess->exitStatus() != 0 ) {
    kdDebug() << "KProcess reported an error." << endl;
    KMessageBox::error( this, i18n("Failed to build index.") );
  } else {
    mConfig->setGroup( "Search" );
    mConfig->writeEntry( "IndexExists", true );
    emit searchIndexUpdated();
  }

  deleteProcess();
  deleteCmdFile();

  mCurrentEntry = 0;

  if ( mProgressDialog ) {
    mProgressDialog->setFinished( true );
  }

  mStdOut = QString();
  mStdErr = QString();

  if ( mIsClosing ) {
    if ( !mProgressDialog->isVisible() ) {
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
  kdDebug() << "KCMHelpCenter::slotIndexProgress()" << endl;

  updateStatus();

  advanceProgress();
}

void KCMHelpCenter::slotIndexError( const QString &str )
{
  kdDebug() << "KCMHelpCenter::slotIndexError()" << endl;

  KMessageBox::sorry( this, i18n("Error executing indexing build command:\n%1")
    .arg( str ) );

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

void KCMHelpCenter::slotReceivedStdout( KProcess *, char *buffer, int buflen )
{
  QString text = QString::fromLocal8Bit( buffer, buflen );
  int pos = text.findRev( '\n' );
  if ( pos < 0 ) {
    mStdOut.append( text );
  } else {
    if ( mProgressDialog ) {
      mProgressDialog->appendLog( mStdOut + text.left( pos ) );
      mStdOut = text.mid( pos + 1 );
    }
  }
}

void KCMHelpCenter::slotReceivedStderr( KProcess *, char *buffer, int buflen )
{
  QString text = QString::fromLocal8Bit( buffer, buflen );
  int pos = text.findRev( '\n' );
  if ( pos < 0 ) {
    mStdErr.append( text );
  } else {
    if ( mProgressDialog ) {
      mProgressDialog->appendLog( "<i>" + mStdErr + text.left( pos ) +
                                  "</i>");
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
  kdDebug() << "KCMHelpCenter::slotProgressClosed()" << endl;

  if ( mIsClosing ) accept();
}

void KCMHelpCenter::showIndexDirDialog()
{
  IndexDirDialog dlg( this );
  if ( dlg.exec() == QDialog::Accepted ) {
    load();
  }
}

#include "kcmhelpcenter.moc"

// vim:ts=2:sw=2:et
