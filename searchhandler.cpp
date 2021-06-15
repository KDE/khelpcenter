/*
    SPDX-FileCopyrightText: 2005 Cornelius Schumacher <schumacher@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "searchhandler.h"

#include "docentry.h"
#include "khc_debug.h"

#include <KConfigGroup>
#include <KDesktopFile>
#include <KIO/Job>
#include <KLocalizedString>
#include <KProcess>
#include <KShell>

#include <prefs.h>

using namespace KHC;

SearchJob::SearchJob(DocEntry *entry) : mEntry( entry )
{
}

bool SearchJob::startLocal(const QString &cmdString)
{
    mProcess = new KProcess;
    *mProcess << KShell::splitArgs(cmdString);

    connect(mProcess, QOverload<int, QProcess::ExitStatus>::of(&KProcess::finished), this, &SearchJob::searchExited);

    mProcess->setOutputChannelMode(KProcess::SeparateChannels);
    mProcess->start();
    if (!mProcess->waitForStarted()) {
        const QString txt = i18n("Error executing search command '%1'.", cmdString );
        Q_EMIT searchError( this, mEntry, txt );
        return false;
    }
    return true;
}

bool SearchJob::startRemote(const QString &urlString)
{
    KIO::TransferJob *job  = KIO::get( QUrl( urlString ) );
    connect(job, &KIO::TransferJob::result, this, &SearchJob::slotJobResult);
    connect(job, &KIO::TransferJob::data, this, &SearchJob::slotJobData);

    mKioJob = job;
    return true;
}

SearchJob::~SearchJob()
{
    delete mProcess;
    delete mKioJob;
}

void SearchJob::searchExited( int exitCode, QProcess::ExitStatus exitStatus )
{
    if ( exitStatus == QProcess::NormalExit && exitCode == 0 ) {
        mResult = QString::fromUtf8(mProcess->readAllStandardOutput());
        Q_EMIT searchFinished( this, mEntry, mResult );
    } else {
        mError = QString::fromUtf8(mProcess->readAllStandardError());
        QString error = QLatin1String("<em>") + mCmd + QLatin1String("</em>\n") + mError;
        Q_EMIT searchError( this, mEntry, error );
    }
}

void SearchJob::slotJobResult( KJob *job )
{
    if ( job->error() ) {
        Q_EMIT searchError( this, mEntry, i18n("Error: %1", job->errorString() ) );
    } else {
        Q_EMIT searchFinished( this, mEntry, mResult );
    }
}

void SearchJob::slotJobData( KIO::Job *job, const QByteArray &data )
{
    Q_UNUSED(job);
    mResult += QString::fromUtf8(data.data());
}


SearchHandler::SearchHandler( const KConfigGroup &cg )
{
  mLang = QLocale().bcp47Name().left( 2 );
  mDocumentTypes = cg.readEntry( "DocumentTypes" , QStringList() );
}

SearchHandler::~SearchHandler()
{
}

SearchHandler *SearchHandler::initFromFile( const QString &filename )
{
  KDesktopFile file( filename );
  KConfigGroup dg = file.desktopGroup();

  SearchHandler *handler = nullptr;

  handler = new ExternalProcessSearchHandler( dg );

  return handler;
}

QStringList SearchHandler::documentTypes() const
{
  return mDocumentTypes;
}


ExternalProcessSearchHandler::ExternalProcessSearchHandler( const KConfigGroup &cg )
  : SearchHandler( cg )
{
  mSearchCommand = cg.readEntry( "SearchCommand" );
  mSearchUrl = cg.readEntry( "SearchUrl" );
  mIndexCommand = cg.readEntry( "IndexCommand" );
  mTryExec = cg.readEntry( "TryExec" );
  mSearchBinary = cg.readEntry( "SearchBinary" );
  const QStringList searchBinaryPaths = cg.readEntry( "SearchBinaryPaths", QStringList() );
  mSearchBinary = QStandardPaths::findExecutable(mSearchBinary, searchBinaryPaths);
}

QString ExternalProcessSearchHandler::indexCommand( const QString &identifier ) const
{
  QString cmd = mIndexCommand;
  cmd.replace( QStringLiteral("%i"), identifier );
  cmd.replace( QStringLiteral("%d"), Prefs::indexDirectory() );
  cmd.replace( QStringLiteral("%l"), mLang );
  return cmd;
}

bool ExternalProcessSearchHandler::checkPaths(QString* error) const
{
  if ( !mSearchCommand.isEmpty() && !checkBinary( mSearchCommand ) ) {
    *error = i18n("'%1' not found, check your installation", mSearchCommand);
    return false;
  }

  if ( !mIndexCommand.isEmpty() && !checkBinary( mIndexCommand ) ) {
    *error = i18n("'%1' not found, check your installation", mIndexCommand);
    return false;
  }

  if ( !mTryExec.isEmpty() && !checkBinary( mTryExec ) ) {
    *error = i18n("'%1' not found, install the package containing it", mTryExec);
    return false;
  }

  return true;
}

bool ExternalProcessSearchHandler::checkBinary( const QString &cmd ) const
{
  QString binary;

  int pos = cmd.indexOf( QLatin1Char(' ') );
  if ( pos < 0 ) binary = cmd;
  else binary = cmd.left( pos );

  return !QStandardPaths::findExecutable( binary ).isEmpty();
}

void ExternalProcessSearchHandler::search( DocEntry *entry, const QStringList &words,
  int maxResults,
  SearchEngine::Operation operation )
{
  qCDebug(KHC_LOG) << entry->identifier();

  if ( !mSearchCommand.isEmpty() ) {
    const QString cmdString = SearchEngine::substituteSearchQuery( mSearchCommand,
      entry->identifier(), words, maxResults, operation, mLang, mSearchBinary );

    qCDebug(KHC_LOG) << "CMD:" << cmdString;

    SearchJob *searchJob = new SearchJob(entry);
    connect(searchJob, &SearchJob::searchFinished, this, &ExternalProcessSearchHandler::slotSearchFinished);
    connect(searchJob, &SearchJob::searchError, this, &ExternalProcessSearchHandler::slotSearchError);
    searchJob->startLocal(cmdString);

  } else if ( !mSearchUrl.isEmpty() ) {
    const QString urlString = SearchEngine::substituteSearchQuery( mSearchUrl,
      entry->identifier(), words, maxResults, operation, mLang, mSearchBinary );

    qCDebug(KHC_LOG) << "URL:" << urlString;

    SearchJob *searchJob = new SearchJob(entry);
    connect(searchJob, &SearchJob::searchFinished, this, &ExternalProcessSearchHandler::slotSearchFinished);
    connect(searchJob, &SearchJob::searchError, this, &ExternalProcessSearchHandler::slotSearchError);
    searchJob->startRemote(urlString);

  } else {
    const QString txt = i18n("No search command or URL specified.");
    Q_EMIT searchFinished( this, entry, txt );
  }
}

void ExternalProcessSearchHandler::slotSearchFinished( SearchJob *job, DocEntry *entry, const QString &result )
{
    Q_EMIT searchFinished( this, entry, result);
    job->deleteLater();
}

void ExternalProcessSearchHandler::slotSearchError( SearchJob *job, DocEntry *entry, const QString &error )
{
    Q_EMIT searchError(this, entry, error);
    job->deleteLater();
}


