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

class SearchHandler : public QObject
{
    Q_OBJECT
  public:
    static SearchHandler *initFromFile( const QString &filename );

    void search( const QString &identifier, const QStringList &words,
      int maxResults = 10,
      SearchEngine::Operation operation = SearchEngine::And );

    QString indexCommand( const QString &identifier );

    QStringList documentTypes() const;

  signals:
    void searchFinished( const QString & );

  protected slots:
    void searchStdout( KProcess *proc, char *buffer, int buflen );
    void searchStderr( KProcess *proc, char *buffer, int buflen );
    void searchExited( KProcess *proc ); 

    void slotJobResult( KIO::Job *job );
    void slotJobData( KIO::Job *, const QByteArray &data );

  private:
    SearchHandler();

    QString mSearchCommand;
    QString mSearchUrl;
    QString mIndexCommand;
    QStringList mDocumentTypes;

    bool mSearchRunning;
    QString mSearchResult;
    QString mStderr;
};

}

#endif
