#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H

#include <qobject.h>
#include <qlist.h>
#include <qstring.h>

#include <kpixmap.h>
#include <kio/job.h>

#include "docentrytraverser.h"

class QWidget;
class KProcess;
class KConfig;
class KHTMLPart;
class SearchFormatter;
class SearchEngine;

class SearchTraverser : public QObject, public DocEntryTraverser
{
    Q_OBJECT
  public:
    SearchTraverser( SearchEngine *engine, int level );
    ~SearchTraverser();

    void process( DocEntry * );
    
    void startProcess( DocEntry * );

    DocEntryTraverser *createChild();

    void finishTraversal();

  protected slots:
    void slotJobResult( KIO::Job * );
    void slotJobData( KIO::Job *, const QByteArray & );

  private:
    SearchEngine *mEngine;
    int mLevel;

    DocEntry *mEntry;
    QString mJobData;
};


class SearchEngine : public QObject
{
    Q_OBJECT
  public:
    SearchEngine( KHTMLPart * );
    ~SearchEngine();
    
    bool search( QString words, QString method = "and", int matches = 5,
                 QString scope = "" );

    SearchFormatter *formatter(); 
    KHTMLPart *view();

    QString substituteSearchQuery( const QString &query );

    void finishSearch();

  signals:
    void searchFinished();

  protected slots:
    void searchStdout(KProcess *proc, char *buffer, int buflen);
    void searchExited(KProcess *proc);

  protected:
    void processSearchQueue();
    
  private:
    KProcess *mProc;
    bool mSearchRunning;
    QString mSearchResult;

    KHTMLPart *mView;
    SearchFormatter *mFormatter;
    
    QString mWords;
    int mMatches;
    QString mMethod;
    QString mLang;
    QString mScope;
    
    DocEntryTraverser *mRootTraverser;
};

#endif
