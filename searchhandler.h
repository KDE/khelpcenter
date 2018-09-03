
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

#include <QProcess>

class KConfigGroup;
class KJob;
class KProcess;
namespace KIO {
  class Job;
}

namespace KHC {

  class SearchJob : public QObject {
      Q_OBJECT
    public:
      explicit SearchJob(DocEntry *entry);
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

      virtual ~SearchHandler();

      virtual void search( DocEntry *, const QStringList &words,
	int maxResults = 10,
	SearchEngine::Operation operation = SearchEngine::And ) = 0;

      virtual QString indexCommand( const QString &identifier ) const = 0;

      QStringList documentTypes() const;

      virtual bool checkPaths(QString* error) const = 0;

    Q_SIGNALS:
      void searchFinished( SearchHandler *, DocEntry *, const QString & );
      void searchError( SearchHandler *, DocEntry *, const QString & );

    protected:
      SearchHandler( const KConfigGroup &cg );

      QString mLang;
      QStringList mDocumentTypes;
  };

  class ExternalProcessSearchHandler : public SearchHandler
  {
      Q_OBJECT
    public:
      explicit ExternalProcessSearchHandler( const KConfigGroup &cg );

      void search( DocEntry *, const QStringList &words,
	int maxResults = 10,
	SearchEngine::Operation operation = SearchEngine::And ) override;

      QString indexCommand( const QString &identifier ) const override;

      bool checkPaths(QString* error) const override;

    private:
      bool checkBinary( const QString &cmd ) const;

    private Q_SLOTS:
      void slotSearchFinished( SearchJob *, DocEntry *, const QString & );
      void slotSearchError( SearchJob *, DocEntry *, const QString & );

    private:
      QString mSearchCommand;
      QString mSearchUrl;
      QString mSearchBinary;
      QString mIndexCommand;
      QString mTryExec;
  };

}

#endif //KHC_SEARCHHANDLER_H
