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
#ifndef KHC_SEARCHHANDLER_H
#define KHC_SEARCHHANDLER_H

#include "searchengine.h"

#include <qobject.h>
#include <qstringlist.h>

namespace KIO {
class Job;
}

namespace KHC {

class SearchJob
{
  public:
    SearchJob() : mProcess( 0 ), mKioJob( 0 ) {}

    DocEntry *mEntry;
  
    KProcess *mProcess;
    KIO::Job *mKioJob;
    
    QString mCmd;
    
    QString mResult;
    QString mError;
};

class SearchHandler : public QObject
{
    Q_OBJECT
  public:
    static SearchHandler *initFromFile( const QString &filename );

    void search( DocEntry *, const QStringList &words,
      int maxResults = 10,
      SearchEngine::Operation operation = SearchEngine::And );

    QString indexCommand( const QString &identifier );

    QStringList documentTypes() const;

    bool checkPaths() const;

  signals:
    void searchFinished( SearchHandler *, DocEntry *, const QString & );
    void searchError( SearchHandler *, DocEntry *, const QString & );

  protected:
    bool checkBinary( const QString &cmd ) const;

  protected slots:
    void searchStdout( KProcess *proc, char *buffer, int buflen );
    void searchStderr( KProcess *proc, char *buffer, int buflen );
    void searchExited( KProcess *proc ); 

    void slotJobResult( KIO::Job *job );
    void slotJobData( KIO::Job *, const QByteArray &data );

  private:
    SearchHandler();

    QString mLang;

    QString mSearchCommand;
    QString mSearchUrl;
    QString mIndexCommand;
    QStringList mDocumentTypes;

    QMap<KProcess *,SearchJob *> mProcessJobs;
    QMap<KIO::Job *,SearchJob *> mKioJobs;
};

}

#endif
