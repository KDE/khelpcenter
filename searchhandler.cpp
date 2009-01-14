/*
    This file is part of KHelpCenter.

    Copyright (c) 2005 Cornelius Schumacher <schumacher@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "searchhandler.h"

#include "searchengine.h"
#include "prefs.h"
#include "docentry.h"

#include <kdesktopfile.h>
#include <KProcess>
#include <kdebug.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <KShell>

#include <stdlib.h>

using namespace KHC;

SearchJob::SearchJob(DocEntry *entry) : mEntry( entry ), mProcess( 0 ), mKioJob( 0 )
{
}

bool SearchJob::startLocal(const QString &cmdString)
{
    mProcess = new KProcess;
    *mProcess << KShell::splitArgs(cmdString);

    connect( mProcess, SIGNAL( finished(int, QProcess::ExitStatus) ),
             this, SLOT( searchExited(int, QProcess::ExitStatus) ) );

    mProcess->setOutputChannelMode(KProcess::SeparateChannels);
    mProcess->start();
    if (!mProcess->waitForStarted()) {
        QString txt = i18n("Error executing search command '%1'.", cmdString );
        emit searchError( this, mEntry, txt );
        return false;
    }
    return true;
}

bool SearchJob::startRemote(const QString &urlString)
{
    KIO::TransferJob *job  = KIO::get( KUrl( urlString ) );
    connect( job, SIGNAL( result( KJob * ) ),
             this, SLOT( slotJobResult( KJob * ) ) );
    connect( job, SIGNAL( data( KIO::Job *, const QByteArray & ) ),
             this, SLOT( slotJobData( KIO::Job *, const QByteArray & ) ) );

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
        mResult = mProcess->readAllStandardOutput();
        emit searchFinished( this, mEntry, mResult );
    } else {
        mError = mProcess->readAllStandardError();
        QString error = QLatin1String("<em>") + mCmd + QLatin1String("</em>\n") + mError;
        emit searchError( this, mEntry, error );
    }
}

void SearchJob::slotJobResult( KJob *job )
{
    QString result;
    //DocEntry *entry = 0;

    if ( job->error() ) {
        emit searchError( this, mEntry, i18n("Error: %1", job->errorString() ) );
    } else {
        emit searchFinished( this, mEntry, mResult );
    }
}

void SearchJob::slotJobData( KIO::Job *job, const QByteArray &data )
{
    Q_UNUSED(job);
    mResult += data.data();
}


SearchHandler::SearchHandler( const KConfigGroup &cg )
{
  mLang = KGlobal::locale()->language().left( 2 );
  mDocumentTypes = cg.readEntry( "DocumentTypes" , QStringList() );
}

SearchHandler::~SearchHandler()
{
}

SearchHandler *SearchHandler::initFromFile( const QString &filename )
{
  KDesktopFile file( filename );
  KConfigGroup dg = file.desktopGroup();

  SearchHandler *handler = 0;

  const QString type = dg.readEntry( "Type" );
  if ( false ) {
  } else {
    handler = new ExternalProcessSearchHandler( dg );
  }

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
}

QString ExternalProcessSearchHandler::indexCommand( const QString &identifier )
{
  QString cmd = mIndexCommand;
  cmd.replace( "%i", identifier );
  cmd.replace( "%d", Prefs::indexDirectory() );
  cmd.replace( "%l", mLang );
  return cmd;
}

bool ExternalProcessSearchHandler::checkPaths() const
{
  if ( !mSearchCommand.isEmpty() && !checkBinary( mSearchCommand ) )
    return false;

  if ( !mIndexCommand.isEmpty() && !checkBinary( mIndexCommand ) )
    return false;

  return true;
}

bool ExternalProcessSearchHandler::checkBinary( const QString &cmd ) const
{
  QString binary;

  int pos = cmd.indexOf( ' ' );
  if ( pos < 0 ) binary = cmd;
  else binary = cmd.left( pos );

  return !KStandardDirs::findExe( binary ).isEmpty();
}

void ExternalProcessSearchHandler::search( DocEntry *entry, const QStringList &words,
  int maxResults,
  SearchEngine::Operation operation )
{
  kDebug() << entry->identifier();

  if ( !mSearchCommand.isEmpty() ) {
    QString cmdString = SearchEngine::substituteSearchQuery( mSearchCommand,
      entry->identifier(), words, maxResults, operation, mLang );

    kDebug() << "CMD:" << cmdString;

    SearchJob *searchJob = new SearchJob(entry);
    connect(searchJob, SIGNAL(searchFinished( SearchJob *, DocEntry *, const QString & )),
            this, SLOT(slotSearchFinished( SearchJob *, DocEntry *, const QString & )));
    connect(searchJob, SIGNAL(searchError( SearchJob *, DocEntry *, const QString & )),
            this, SLOT(slotSearchError( SearchJob *, DocEntry *, const QString & )));
    searchJob->startLocal(cmdString);

  } else if ( !mSearchUrl.isEmpty() ) {
    QString urlString = SearchEngine::substituteSearchQuery( mSearchUrl,
      entry->identifier(), words, maxResults, operation, mLang );

    kDebug() << "URL:" << urlString;

    SearchJob *searchJob = new SearchJob(entry);
    connect(searchJob, SIGNAL(searchFinished( SearchJob *, DocEntry *, const QString & )),
            this, SLOT(slotSearchFinished( SearchJob *, DocEntry *, const QString & )));
    connect(searchJob, SIGNAL(searchError( SearchJob *, DocEntry *, const QString & )),
            this, SLOT(slotSearchError( SearchJob *, DocEntry *, const QString & )));
    searchJob->startRemote(urlString);

  } else {
    QString txt = i18n("No search command or URL specified.");
    emit searchFinished( this, entry, txt );
  }
}

void ExternalProcessSearchHandler::slotSearchFinished( SearchJob *job, DocEntry *entry, const QString &result )
{
    emit searchFinished( this, entry, result);
    job->deleteLater();
}

void ExternalProcessSearchHandler::slotSearchError( SearchJob *job, DocEntry *entry, const QString &error )
{
    emit searchError(this, entry, error);
    job->deleteLater();
}

#include "searchhandler.moc"
