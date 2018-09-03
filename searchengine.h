
/*
    This file is part of KHelpCenter.

    Copyright (c) 2002 Cornelius Schumacher <schumacher@kde.org>

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

#ifndef KHC_SEARCHENGINE_H
#define KHC_SEARCHENGINE_H

#include <QObject>
#include <QMap>
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

    SearchEngine *mEngine;
    int mLevel;

    DocEntry *mEntry;
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
