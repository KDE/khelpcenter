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

#include <unistd.h>
#include <sys/types.h>

#include <qlayout.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qdir.h>
#include <qtabwidget.h>
#include <qprogressbar.h>
#include <qfile.h>
#include <qlabel.h>
#include <qvbox.h>
#include <qtextedit.h>
#include <qregexp.h>

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

#include "htmlsearchconfig.h"

#include "docmetainfo.h"

#include "kcmhelpcenter.h"
#include "kcmhelpcenter.moc"

using namespace KHC;

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


KCMHelpCenter::KCMHelpCenter(QWidget *parent, const char *name)
  : DCOPObject( "kcmhelpcenter" ),
    KDialogBase( parent, name, false, i18n("Build Search Index") ),
    mProgressDialog( 0 ), mCurrentEntry( 0 ), mCmdFile( 0 ),
    mProcess( 0 ), mIsClosing( false ), mRunAsRoot( false )
{
  QTabWidget *tabWidget = new QTabWidget( this );

  setMainWidget( tabWidget );

  mScopeTab = createScopeTab( tabWidget );
  tabWidget->addTab( mScopeTab, i18n( "Index" ) );

  mHtmlSearchTab = new KHC::HtmlSearchConfig( tabWidget );
  tabWidget->addTab( mHtmlSearchTab, i18n("HTML Search") );

  mConfig = KGlobal::config();

  delete DocMetaInfo::self();
  DocMetaInfo::self()->scanMetaInfo();

  load();

  bool success = kapp->dcopClient()->connectDCOPSignal( "khc_indexbuilder",
      0, "buildIndexProgress()", "kcmhelpcenter",
      "slotIndexProgress()", false );
  if ( !success ) kdError() << "connect DCOP signal failed" << endl;
}

KCMHelpCenter::~KCMHelpCenter()
{
}

QWidget *KCMHelpCenter::createScopeTab( QWidget *parent )
{
  QWidget *scopeTab = new QWidget( parent );

  QVBoxLayout *topLayout = new QVBoxLayout( scopeTab );
  topLayout->setMargin( KDialog::marginHint() );
  topLayout->setSpacing( KDialog::spacingHint() );

  mListView = new QListView( scopeTab );
  mListView->addColumn( i18n("Search Scope") );
  mListView->addColumn( i18n("Status") );
  mListView->setColumnAlignment( 1, AlignCenter );
  topLayout->addWidget( mListView );

  QBoxLayout *urlLayout = new QHBoxLayout( topLayout );

  QLabel *urlLabel = new QLabel( i18n("Index directory:"), scopeTab );
  urlLayout->addWidget( urlLabel );

  mIndexUrlRequester = new KURLRequester( scopeTab );
  mIndexUrlRequester->setMode( KFile::Directory | KFile::ExistingOnly |
                               KFile::LocalOnly );
  urlLayout->addWidget( mIndexUrlRequester );

  QBoxLayout *buttonLayout = new QHBoxLayout( topLayout );

  buttonLayout->addStretch( 1 );

  return scopeTab;
}

void KCMHelpCenter::defaults()
{
  mHtmlSearchTab->defaults();
}

void KCMHelpCenter::save()
{
  kdDebug(1401) << "KCMHelpCenter::save()" << endl;

  mConfig->setGroup( "Search" );
  mConfig->writeEntry( "IndexDirectory", indexDir() );

  mHtmlSearchTab->save( mConfig );

  mConfig->sync();

  if ( !QFile::exists( indexDir() ) ) {
    KMessageBox::sorry( this,
      i18n("The directory '%1' doesn't exist. Unable to create index.")
      .arg( indexDir() ) );
  } else {
    buildIndex();
  }
}

void KCMHelpCenter::load()
{
  mConfig->setGroup( "Search" );
  QString indexUrl = mConfig->readEntry( "IndexDirectory" );
  mIndexUrlRequester->setURL( indexUrl );

  mHtmlSearchTab->load( mConfig );

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
    if ( item->entry()->indexExists( indexDir() ) ) {
      status = i18n("OK");
      item->setOn( false );
    } else {
      status = i18n("Missing");
    }
    item->setText( 1, status );

    ++it;
  }
}

void KCMHelpCenter::buildIndex()
{
  kdDebug(1401) << "Build Index" << endl;

  kdDebug() << "IndexPath: '" << indexDir() << "'" << endl;

  if ( mProcess ) {
    kdError() << "Error: Index Process still running." << endl;
    return;
  }

  mIndexQueue.clear();

  QFontMetrics fm( font() );
  int maxWidth = 0;

  QListViewItemIterator it( mListView );
  while ( it.current() != 0 ) {
    ScopeItem *item = static_cast<ScopeItem *>( it.current() );
    if ( item->isOn() ) {
      DocEntry *entry = item->entry();
      mIndexQueue.append( entry );
      int width = fm.width( entry->name() );
      if ( width > maxWidth ) maxWidth = width;
    }
    ++it;
  }

  if ( mIndexQueue.isEmpty() ) return;

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

  mCmdFile = new KTempFile;
  QTextStream *ts = mCmdFile->textStream();
  if ( !ts ) {
    kdError() << "Error opening command file." << endl;
  } else {
    kdDebug() << "Writing to file '" << mCmdFile->name() << "'" << endl;
    QValueList<KHC::DocEntry *>::ConstIterator it;
    for( it = mIndexQueue.begin(); it != mIndexQueue.end(); ++it ) {
      QString indexer = (*it)->indexer();
      indexer.replace( QRegExp( "%i" ), indexDir() );
      kdDebug() << "INDEXER: " << indexer << endl;
      *ts << indexer << endl;
    }
    mCmdFile->close();
  }

  startIndexProcess();
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
  *mProcess << indexDir();

  connect( mProcess, SIGNAL( processExited( KProcess * ) ),
           SLOT( slotIndexFinished( KProcess * ) ) );
  connect( mProcess, SIGNAL( receivedStdout( KProcess *, char *, int ) ),
           SLOT( slotReceivedStdout(KProcess *, char *, int ) ) );
  connect( mProcess, SIGNAL( receivedStderr( KProcess *, char *, int ) ),
           SLOT( slotReceivedStderr( KProcess *, char *, int ) ) );

  mProcess->start( KProcess::NotifyOnExit, KProcess::AllOutput );
}

void KCMHelpCenter::cancelBuildIndex()
{
  kdDebug() << "cancelBuildIndex()" << endl;

  if ( mProcess ) {
    delete mProcess; mProcess = 0;
  }
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
      delete mProcess; mProcess = 0;
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

  delete mProcess; mProcess = 0;
  
  delete mCmdFile; mCmdFile = 0;

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

void KCMHelpCenter::slotIndexProgress()
{
  kdDebug() << "KCMHelpCenter::slotIndexProgress()" << endl;

  updateStatus();

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
  slotApply();
  if ( !mProcess ) accept();
  else mIsClosing = true;
}

void KCMHelpCenter::slotApply()
{
  save();
}

QString KCMHelpCenter::indexDir()
{
  return mIndexUrlRequester->url();
}

void KCMHelpCenter::slotProgressClosed()
{
  kdDebug() << "KCMHelpCenter::slotProgressClosed()" << endl;

  if ( mIsClosing ) accept();
}


// vim:ts=2:sw=2:et
