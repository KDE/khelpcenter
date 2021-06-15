/*
    SPDX-FileCopyrightText: 2002 Cornelius Schumacher <schumacher@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KHC_SEARCHENGINE_H
#define KHC_SEARCHENGINE_H

#include <QMap>
#include <QObject>
#include <QPair>
#include <QSharedPointer>

#include "docentrytraverser.h"

namespace KHC {

class SearchEngine;
class View;
class SearchHandler;
class GrantleeFormatter;

class SearchTraverser : public QObject, public DocEntryTraverser
{
    Q_OBJECT
  public:
    SearchTraverser( SearchEngine *engine, int level );
    ~SearchTraverser() override;

    void process( DocEntry * ) override;

    void startProcess( DocEntry * ) override;

    DocEntryTraverser *createChild( DocEntry * ) override;

    DocEntryTraverser *parentTraverser() override;

    void deleteTraverser() override;

    void finishTraversal() override;

    void setWords( const QString &words );

  protected:
    void connectHandler( SearchHandler *handler );
    void disconnectHandler( SearchHandler *handler );

  protected Q_SLOTS:
    void showSearchResult( SearchHandler *, DocEntry *, const QString &result );
    void showSearchError( SearchHandler *, DocEntry *, const QString &error );

  private:
    const int mMaxLevel;

    SearchEngine *mEngine = nullptr;
    int mLevel;

    DocEntry *mEntry = nullptr;
    QString mJobData;

    QList<QPair<DocEntry *, QString> > mResults;
    QList<QPair<DocEntry *, QString> > *mResultsPtr;
    QString mWords;

    QMap<SearchHandler *, int> mConnectCount;
};


class SearchEngine : public QObject
{
    Q_OBJECT
  public:
    enum Operation { And, Or };

    explicit SearchEngine( View * );
    ~SearchEngine();

    bool initSearchHandlers();

    bool search( const QString & words, const QString & method = QStringLiteral("and"), int matches = 5,
                 const QStringList & scope = QStringList() );

    GrantleeFormatter *grantleeFormatter() const;
    View *view() const;

    static QString substituteSearchQuery( const QString &query,
      const QString &identifier, const QStringList &words, int maxResults,
      Operation operation, const QString &lang, const QString& binary );

    void finishSearch();

    /**
      Append error message to error log.
    */
    void logError( DocEntry *entry, const QString &msg );

    /**
      Return error log.
    */
    QString errorLog() const;

    bool isRunning() const;

    SearchHandler *handler( const QString &documentType ) const;

    QStringList words() const;
    int maxResults() const;
    Operation operation() const;

    bool canSearch( DocEntry * );
    bool needsIndex( DocEntry * );

  Q_SIGNALS:
    void searchFinished();

  private:
    bool mSearchRunning;
    QString mSearchResult;

    QString mStderr;

    View *mView;

    QString mWords;
    int mMatches;
    QString mMethod;
    QStringList mScope;

    QStringList mWordList;
    int mMaxResults;
    Operation mOperation;

    SearchTraverser *mRootTraverser;

    QMap<QString, QSharedPointer<SearchHandler> > mHandlers;
};

}

#endif //KHC_SEARCHENGINE_H
// vim:ts=2:sw=2:et
