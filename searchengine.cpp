#include "stdlib.h"
#include "glob.h"

#include <qstring.h>
#include <qstringlist.h>
#include <qfile.h>
#include <qwidget.h>
#include <qregexp.h>
#include <qdir.h>
#include <qpixmapcache.h>
#include <qtextstream.h>

#include <kapp.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kstddirs.h>
#include <kprocess.h>
#include <klocale.h>
#include <khtml_part.h>
#include <kio/job.h>

#include "docmetainfo.h"

#include "searchengine.h"
#include "searchengine.moc"

SearchEngine::SearchEngine( KHTMLPart *destination )
  : QObject(),
    mProc( 0 ), mView( destination )
{
}

void SearchEngine::searchStdout(KProcess *, char *buffer, int len)
{
  if ( !buffer || len == 0 )
    return;

  QString bufferStr;
  char *p;
  p = (char*) malloc( sizeof(char) * (len+1) );
  p = strncpy( p, buffer, len );
  p[len] = '\0';

  mSearchResult += bufferStr.fromUtf8(p);

  free(p);
}


void SearchEngine::searchExited(KProcess *)
{
  kdDebug() << "Search terminated" << endl;
  mSearchRunning = false;
}

bool SearchEngine::search(QString words, QString method, int matches, QString scope)
{
  KConfig *cfg = KGlobal::config();
  cfg->setGroup( "Search" );
  QString commonSearchProgram = cfg->readEntry( "CommonProgram" );
  bool useCommon = cfg->readBoolEntry( "UseCommonProgram", false );
  
  if ( commonSearchProgram.isEmpty() || !useCommon ) {
    if ( !mView ) {
      return false;
    }
    
    mSearchQueue.clear();
    
    DocEntry::List entries = DocMetaInfo::self()->searchEntries();
    DocEntry::List::ConstIterator it;
    for( it = entries.begin(); it != entries.end(); ++it ) {
      if ( (*it)->searchEnabled() ) {
        QString search = substituteSearchQuery( (*it)->search(), words,
                                                matches );
      
        kdDebug() << "SEARCH: " << search << endl;

        mSearchQueue.append( search );
      }

    }

    if ( mSearchQueue.count() ) {
      mView->begin();
      mView->write( "<html><head><title>" + i18n("Search Results") +
                    "</title></head><body>" );
      processSearchQueue();
    }

    return true;

#if 0  
    QString text = "<html><head><title>Titel</title></head><body>";
    text += "Hallihallo";
    text += "</body></html>";

    mView->begin();
    mView->write( text );
    mView->end();
#endif
  } else {
    QString lang = KGlobal::locale()->language().left(2);

    if ( lang.lower() == "c" || lang.lower() == "posix" )
	  lang = "en";

    // if the string contains '&' replace with a '+' and set search method to and
    if (words.find("&") != -1) {
      words.replace(QRegExp("&"), " ");
      method = "and";
    }
 
    // replace whitespace with a '+'
    words = words.stripWhiteSpace();
    words = words.simplifyWhiteSpace();
    words.replace(QRegExp("\\s"), "+");

    commonSearchProgram = substituteSearchQuery( commonSearchProgram, words,
                                                 matches, method, lang, scope );

    mProc = new KProcess();

    QStringList cmd = QStringList::split( " ", commonSearchProgram );
    QStringList::ConstIterator it;
    for( it = cmd.begin(); it != cmd.end(); ++it ) {
      *mProc << (*it);
    }

    connect( mProc, SIGNAL( receivedStdout( KProcess *, char *, int ) ),
             SLOT( searchStdout( KProcess *, char *, int ) ) );
    connect( mProc, SIGNAL( processExited( KProcess * ) ),
             SLOT( searchExited( KProcess * ) ) );

    mSearchRunning = true;
    mSearchResult = "";

    mProc->start(KProcess::NotifyOnExit, KProcess::All);

    while (mSearchRunning && mProc->isRunning())
      kapp->processEvents();

    if ( !mProc->normalExit() || mProc->exitStatus() != 0 ) {
      kdDebug() << "Error running search script... returning now" << endl;
      delete mProc;
      
      return false;
    }

    delete mProc;

    // modify the search result
    mSearchResult = mSearchResult.replace(QRegExp("http://localhost/"), "file:/");
    mSearchResult = mSearchResult.mid( mSearchResult.find( '<' ) );

    mView->begin();
    mView->write( mSearchResult );
    mView->end();
  }

  emit searchFinished();
  return true;
}

void SearchEngine::processSearchQueue()
{
  QStringList::Iterator it = mSearchQueue.begin();

  if ( it == mSearchQueue.end() ) {
    mView->write("</body></html>");
    mView->end();
    emit searchFinished();
    return;
  }

  kdDebug() << "SearchEngine::processSearchQueue(): " << (*it) << endl;

  mJobData = QString::null;

  KIO::TransferJob *job = KIO::get( *it );
  connect( job, SIGNAL( result( KIO::Job * ) ),
           SLOT( slotJobResult( KIO::Job * ) ) );
  connect( job, SIGNAL( data( KIO::Job *, const QByteArray & ) ),
           SLOT( slotJobData( KIO::Job *, const QByteArray & ) ) );

  mSearchQueue.remove( it );
}

void SearchEngine::slotJobResult( KIO::Job *job )
{
  if ( job->error() ) {
    job->showErrorDialog( mView->widget() );
  }

  mView->write( processResult( mJobData ) );

  if ( mSearchQueue.count() ) {
    mView->write("<table width=100%><tr><td bgcolor=\"#7B8962\">&nbsp;"
                 "</td></tr></table>");
  }
  
  processSearchQueue();
}

void SearchEngine::slotJobData( KIO::Job *, const QByteArray &data )
{
  kdDebug() << "SearchEngine::slotJobData()" << endl;

  mJobData.append( data.data() );
}

QString SearchEngine::processResult( const QString &data )
{
  QString result;

  enum { Header, BodyTag, Body, Footer };

  int state = Header;

  for( uint i = 0; i < data.length(); ++i ) {
    QChar c = data[i];
    switch ( state ) {
      case Header:
        if ( c == '<' && data.mid( i, 5 ).lower() == "<body" ) {
          state = BodyTag;
          i += 4;
        }
        break;
      case BodyTag:
        if ( c == '>' ) state = Body;
        break;
      case Body:
        if ( c == '<' && data.mid( i, 7 ).lower() == "</body>" ) {
          state = Footer;
        } else {
          result.append( c );
        }
        break;
      case Footer:
        break;
      default:
        result.append( c );
        break;
    }
  }

  kdDebug() << "Result:" << endl << result << endl;

  return result;
}

QString SearchEngine::substituteSearchQuery( const QString &query,
                                             const QString &words,
                                             int matches,
                                             const QString &method,
                                             const QString &lang,
                                             const QString &scope )
{
  QString result = query;
  result.replace( QRegExp( "%k" ), words );
  result.replace( QRegExp( "%n" ), QString::number( matches ) );
  result.replace( QRegExp( "%m" ), method );
  result.replace( QRegExp( "%l" ), lang );
  result.replace( QRegExp( "%s" ), scope );

  return result;
}
