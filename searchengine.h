#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H

#include <qobject.h>
#include <qlist.h>
#include <qstring.h>

#include <kpixmap.h>
#include <kio/job.h>

class QWidget;
class KProcess;
class KConfig;
class KHTMLPart;

class SearchEngine : public QObject
{
    Q_OBJECT
  public:
    SearchEngine( KHTMLPart * );

    bool search(QString words, QString method, int matches, QString scope);

  signals:
    void searchFinished();

  protected slots:
    void searchStdout(KProcess *proc, char *buffer, int buflen);
    void searchExited(KProcess *proc);

    void slotJobResult( KIO::Job * );
    void slotJobData( KIO::Job *, const QByteArray & );

  protected:
    void processSearchQueue();

    QString substituteSearchQuery( const QString &query, const QString &words,
                                   int matches = 5,
                                   const QString &method = "and",
                                   const QString &lanf = "en",
                                   const QString &scope = "" );

    QString processResult( const QString & );
    
  private:
    KProcess *mProc;
    bool mSearchRunning;
    QString mSearchResult;

    QStringList mSearchQueue;
    
    QString mJobData;

    KHTMLPart *mView;
};

#endif
