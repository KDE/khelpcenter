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
#ifndef KHC_SEARCHHANDLER_H
#define KHC_SEARCHHANDLER_H

#include "searchengine.h"

#include <QObject>
#include <KProcess>

namespace KIO {
class Job;
}

namespace KHC {

class SearchJob : public QObject {
    Q_OBJECT
  public:
    SearchJob(DocEntry *entry);
    ~SearchJob();

    bool startLocal(const QString &cmdString);
    bool startRemote(const QString &url);

  Q_SIGNALS:
    void searchFinished( SearchJob *, DocEntry *, const QString & );
    void searchError( SearchJob *, DocEntry *, const QString & );

  protected Q_SLOTS:
    void searchExited( int exitCode, QProcess::ExitStatus );
    void slotJobResult( KJob *job );
    void slotJobData( KIO::Job *, const QByteArray &data );

  protected:
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

  Q_SIGNALS:
    void searchFinished( SearchHandler *, DocEntry *, const QString & );
    void searchError( SearchHandler *, DocEntry *, const QString & );


  protected:
    bool checkBinary( const QString &cmd ) const;
   
  private slots: 
    void searchFinished( SearchJob *, DocEntry *, const QString & );
    void searchError( SearchJob *, DocEntry *, const QString & );

  private:
    SearchHandler();

    QString mLang;

    QString mSearchCommand;
    QString mSearchUrl;
    QString mIndexCommand;
    QStringList mDocumentTypes;
};

}

#endif
