#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H

#include <qobject.h>
#include <qptrlist.h>
#include <qstring.h>

#include <kpixmap.h>
#include <kio/job.h>

#include "docentrytraverser.h"

class QWidget;
class KProcess;
class KConfig;
class KHTMLPart;

namespace KHC {

class SearchFormatter;
class SearchEngine;
class View;

class SearchTraverser : public QObject, public DocEntryTraverser
{
    Q_OBJECT
  public:
    SearchTraverser( SearchEngine *engine, int level );
    ~SearchTraverser();

    void process( DocEntry * );
    
    void startProcess( DocEntry * );

    DocEntryTraverser *createChild( DocEntry * );

    DocEntryTraverser *parentTraverser();

    void deleteTraverser();

    void finishTraversal();

  protected slots:
    void slotJobResult( KIO::Job * );
    void slotJobData( KIO::Job *, const QByteArray & );

  private:
    SearchEngine *mEngine;
    int mLevel;

    DocEntry *mEntry;
    QString mJobData;
    
    QString mResult;
};


class SearchEngine : public QObject
{
    Q_OBJECT
  public:
    SearchEngine( View * );
    ~SearchEngine();
    
    bool search( QString words, QString method = "and", int matches = 5,
                 QString scope = "" );

    SearchFormatter *formatter(); 
    View *view();

    QString substituteSearchQuery( const QString &query );

    void finishSearch();

    QString errorLog();

  signals:
    void searchFinished();

  protected slots:
    void searchStdout(KProcess *proc, char *buffer, int buflen);
    void searchStderr(KProcess *proc, char *buffer, int buflen);
    void searchExited(KProcess *proc);

  protected:
    void processSearchQueue();
    
  private:
    KProcess *mProc;
    bool mSearchRunning;
    QString mSearchResult;

    QString mStderr;

    View *mView;
    SearchFormatter *mFormatter;
    
    QString mWords;
    int mMatches;
    QString mMethod;
    QString mLang;
    QString mScope;
    
    DocEntryTraverser *mRootTraverser;
};

}

#endif
// vim:ts=2:sw=2:et
