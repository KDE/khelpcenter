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
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "searchhandler.h"

#include "searchengine.h"
#include "prefs.h"

#include <kdesktopfile.h>
#include <kprocess.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <klocale.h>

#include <stdlib.h>

using namespace KHC;

SearchHandler::SearchHandler()
  : mSearchRunning( false )
{
}

SearchHandler *SearchHandler::initFromFile( const QString &filename )
{
  SearchHandler *handler = new SearchHandler;

  KDesktopFile file( filename );

  handler->mSearchCommand = file.readEntry( "SearchCommand" );
  handler->mSearchUrl = file.readEntry( "SearchUrl" );
  handler->mIndexCommand = file.readEntry( "IndexCommand" );
  handler->mDocumentTypes = file.readListEntry( "DocumentTypes" );

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
  return cmd;
}

void SearchHandler::search( const QString &identifier, const QStringList &words,
  int maxResults,
  SearchEngine::Operation operation )
{
  if ( mSearchRunning ) {
    kdError() << "SearchHandler::search(): Search still is running." << endl;
    return;
  }

  mSearchResult = "";

  if ( !mSearchCommand.isEmpty() ) {
    QString cmdString = SearchEngine::substituteSearchQuery( mSearchCommand,
      identifier, words, maxResults, operation );

    kdDebug() << "SearchHandler::search() CMD: " << cmdString << endl;

    KProcess *proc = new KProcess();

    QStringList cmd = QStringList::split( " ", cmdString );
    QStringList::ConstIterator it;
    for( it = cmd.begin(); it != cmd.end(); ++it ) {
      QString arg = *it;
      if ( arg.left( 1 ) == "\"" && arg.right( 1 ) =="\"" ) {
        arg = arg.mid( 1, arg.length() - 2 );
      }
      *proc << arg.utf8();
    }

    connect( proc, SIGNAL( receivedStdout( KProcess *, char *, int ) ),
             SLOT( searchStdout( KProcess *, char *, int ) ) );
    connect( proc, SIGNAL( receivedStderr( KProcess *, char *, int ) ),
             SLOT( searchStderr( KProcess *, char *, int ) ) );
    connect( proc, SIGNAL( processExited( KProcess * ) ),
             SLOT( searchExited( KProcess * ) ) );

    mStderr = "";

    if ( !proc->start( KProcess::NotifyOnExit, KProcess::All ) ) {
      QString txt = i18n("Error executing search command '%1'.").arg( cmdString );
      emit searchFinished( txt );
    }
  } else if ( !mSearchUrl.isEmpty() ) {
    QString urlString = SearchEngine::substituteSearchQuery( mSearchUrl,
      identifier, words, maxResults, operation );
  
    kdDebug() << "SearchHandler::search() URL: " << urlString << endl;
  
    KIO::TransferJob *job = KIO::get( KURL( urlString ) );
    connect( job, SIGNAL( result( KIO::Job * ) ),
             SLOT( slotJobResult( KIO::Job * ) ) );
    connect( job, SIGNAL( data( KIO::Job *, const QByteArray & ) ),
             SLOT( slotJobData( KIO::Job *, const QByteArray & ) ) );
  } else {
    QString txt = i18n("No search command or URL specified.");
    emit searchFinished( txt );
    return;
  }

  mSearchRunning = true;
}

void SearchHandler::searchStdout( KProcess *, char *buffer, int len )
{
  if ( !buffer || len == 0 )
    return;

  QString bufferStr;
  char *p;
  p = (char*) malloc( sizeof(char) * ( len + 1 ) );
  p = strncpy( p, buffer, len );
  p[len] = '\0';

  mSearchResult += bufferStr.fromUtf8( p );

  free( p );
}

void SearchHandler::searchStderr( KProcess *, char *buffer, int len )
{
  if ( !buffer || len == 0 )
    return;

  mStderr.append( QString::fromUtf8( buffer, len ) );
}

void SearchHandler::searchExited( KProcess * )
{
  kdDebug() << "SearchHandler::searchExited()" << endl;

  emit searchFinished( mSearchResult );

  mSearchRunning = false;
}

void SearchHandler::slotJobResult( KIO::Job *job )
{
  if ( job->error() ) {
    emit searchFinished( i18n("Error: %1").arg( job->errorString() ) );
  } else {
    emit searchFinished( mSearchResult );
  }

  mSearchRunning = false;
}
 
void SearchHandler::slotJobData( KIO::Job *, const QByteArray &data )
{
//  kdDebug() << "SearchHandler::slotJobData()" << endl;
 
  mSearchResult.append( data.data() );
}

#include "searchhandler.moc"
