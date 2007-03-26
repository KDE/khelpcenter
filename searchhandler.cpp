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
#include <k3process.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kstandarddirs.h>

#include <stdlib.h>

using namespace KHC;

SearchHandler::SearchHandler()
{
  mLang = KGlobal::locale()->language().left( 2 );
}

SearchHandler *SearchHandler::initFromFile( const QString &filename )
{
  SearchHandler *handler = new SearchHandler;

  KDesktopFile file( filename );

  handler->mSearchCommand = file.readEntry( "SearchCommand" );
  handler->mSearchUrl = file.readEntry( "SearchUrl" );
  handler->mIndexCommand = file.readEntry( "IndexCommand" );
  handler->mDocumentTypes = file.readEntry( "DocumentTypes" , QStringList() );

  return handler;
}

QStringList SearchHandler::documentTypes() const
{
  return mDocumentTypes;
}

QString SearchHandler::indexCommand( const QString &identifier )
{
  QString cmd = mIndexCommand;
  cmd.replace( "%i", identifier );
  cmd.replace( "%d", Prefs::indexDirectory() );
  cmd.replace( "%l", mLang );
  return cmd;
}

bool SearchHandler::checkPaths() const
{
  if ( !mSearchCommand.isEmpty() && !checkBinary( mSearchCommand ) )
    return false;

  if ( !mIndexCommand.isEmpty() && !checkBinary( mIndexCommand ) )
    return false;

  return true;
}

bool SearchHandler::checkBinary( const QString &cmd ) const
{
  QString binary;

  int pos = cmd.indexOf( ' ' );
  if ( pos < 0 ) binary = cmd;
  else binary = cmd.left( pos );

  return !KStandardDirs::findExe( binary ).isEmpty();
}

void SearchHandler::search( DocEntry *entry, const QStringList &words,
  int maxResults,
  SearchEngine::Operation operation )
{
  kDebug() << "SearchHandler::search(): " << entry->identifier() << endl;

  if ( !mSearchCommand.isEmpty() ) {
    QString cmdString = SearchEngine::substituteSearchQuery( mSearchCommand,
      entry->identifier(), words, maxResults, operation, mLang );

    kDebug() << "SearchHandler::search() CMD: " << cmdString << endl;

    K3Process *proc = new K3Process();

    QStringList cmd = cmdString.split( " ");
    QStringList::ConstIterator it;
    for( it = cmd.begin(); it != cmd.end(); ++it ) {
      QString arg = *it;
      if ( arg.left( 1 ) == "\"" && arg.right( 1 ) =="\"" ) {
        arg = arg.mid( 1, arg.length() - 2 );
      }
      *proc << arg.toUtf8();
    }

    connect( proc, SIGNAL( receivedStdout( K3Process *, char *, int ) ),
             SLOT( searchStdout( K3Process *, char *, int ) ) );
    connect( proc, SIGNAL( receivedStderr( K3Process *, char *, int ) ),
             SLOT( searchStderr( K3Process *, char *, int ) ) );
    connect( proc, SIGNAL( processExited( K3Process * ) ),
             SLOT( searchExited( K3Process * ) ) );

    SearchJob *searchJob = new SearchJob;
    searchJob->mEntry = entry;
    searchJob->mProcess = proc;
    searchJob->mCmd = cmdString;

    mProcessJobs.insert( proc, searchJob );

    if ( !proc->start( K3Process::NotifyOnExit, K3Process::All ) ) {
      QString txt = i18n("Error executing search command '%1'.", cmdString );
      emit searchFinished( this, entry, txt );
    }
  } else if ( !mSearchUrl.isEmpty() ) {
    QString urlString = SearchEngine::substituteSearchQuery( mSearchUrl,
      entry->identifier(), words, maxResults, operation, mLang );

    kDebug() << "SearchHandler::search() URL: " << urlString << endl;

    KIO::TransferJob *job = KIO::get( KUrl( urlString ) );
    connect( job, SIGNAL( result( KJob * ) ),
             SLOT( slotJobResult( KJob * ) ) );
    connect( job, SIGNAL( data( KIO::Job *, const QByteArray & ) ),
             SLOT( slotJobData( KIO::Job *, const QByteArray & ) ) );

    SearchJob *searchJob = new SearchJob;
    searchJob->mEntry = entry;
    searchJob->mKioJob = job;
    mKioJobs.insert( job, searchJob );
  } else {
    QString txt = i18n("No search command or URL specified.");
    emit searchFinished( this, entry, txt );
    return;
  }
}

void SearchHandler::searchStdout( K3Process *proc, char *buffer, int len )
{
  if ( !buffer || len == 0 )
    return;

  QString bufferStr;
  char *p;
  p = (char*) malloc( sizeof(char) * ( len + 1 ) );
  p = strncpy( p, buffer, len );
  p[len] = '\0';

  QMap<K3Process *, SearchJob *>::ConstIterator it = mProcessJobs.find( proc );
  if ( it != mProcessJobs.end() ) {
    (*it)->mResult += bufferStr.fromUtf8( p );
  }

  free( p );
}

void SearchHandler::searchStderr( K3Process *proc, char *buffer, int len )
{
  if ( !buffer || len == 0 )
    return;

  QMap<K3Process *, SearchJob *>::ConstIterator it = mProcessJobs.find( proc );
  if ( it != mProcessJobs.end() ) {
    (*it)->mError += QString::fromUtf8( buffer, len );
  }
}

void SearchHandler::searchExited( K3Process *proc )
{
//  kDebug() << "SearchHandler::searchExited()" << endl;

  QString result;
  QString error;
  DocEntry *entry = 0;

  QMap<K3Process *, SearchJob *>::ConstIterator it = mProcessJobs.find( proc );
  if ( it != mProcessJobs.end() ) {
    SearchJob *j = *it;
    entry = j->mEntry;
    result = j->mResult;
    error = QLatin1String("<em>") + j->mCmd + QLatin1String("</em>\n") + j->mError;

    mProcessJobs.remove( proc );
    delete j;
  } else {
    kError() << "No search job for exited process found." << endl;
  }

  if ( proc->normalExit() && proc->exitStatus() == 0 ) {
    emit searchFinished( this, entry, result );
  } else {
    emit searchError( this, entry, error );
  }
}

void SearchHandler::slotJobResult( KJob *job )
{
  QString result;
  DocEntry *entry = 0;

  QMap<KJob *, SearchJob *>::ConstIterator it = mKioJobs.find( job );
  if ( it != mKioJobs.end() ) {
    SearchJob *j = *it;

    entry = j->mEntry;
    result = j->mResult;

    mKioJobs.remove( job );
    delete j;
  }

  if ( job->error() ) {
    emit searchError( this, entry, i18n("Error: %1", job->errorString() ) );
  } else {
    emit searchFinished( this, entry, result );
  }
}

void SearchHandler::slotJobData( KIO::Job *job, const QByteArray &data )
{
//  kDebug() << "SearchHandler::slotJobData()" << endl;

  QMap<KJob *, SearchJob *>::ConstIterator it = mKioJobs.find( job );
  if ( it != mKioJobs.end() ) {
    (*it)->mResult += data.data();
  }
}

#include "searchhandler.moc"
