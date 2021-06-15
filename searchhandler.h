/*
    SPDX-FileCopyrightText: 2005 Cornelius Schumacher <schumacher@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
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
      DocEntry *mEntry = nullptr;
      KProcess *mProcess = nullptr;
      KIO::Job *mKioJob = nullptr;
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
