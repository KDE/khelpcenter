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
#include "searchformatter.h"

#include "searchengine.h"
#include "searchengine.moc"

SearchTraverser::SearchTraverser( SearchEngine *engine, int level ) :
  mEngine( engine), mLevel( level ), mEntry( 0 )
{
//  kdDebug() << "SearchTraverser(): " << mLevel << endl;
}

SearchTraverser::~SearchTraverser()
{
#if 0
  if ( mEntry ) {
    kdDebug() << "~SearchTraverser(): " << mLevel << " " << mEntry->name() << endl;
  } else {
    kdDebug() << "~SearchTraverser(): null entry" << endl;
  }
#endif
}

void SearchTraverser::process( DocEntry * )
{
  kdDebug() << "SearchTraverser::process()" << endl;
}

void SearchTraverser::startProcess( DocEntry *entry )
{
//  kdDebug() << "SearchTraverser::startProcess(): " << entry->name() << endl;

  mEntry = entry;

  if ( entry->search().isEmpty() || !entry->searchEnabled() ) {
    mNotifyee->endProcess( entry, this );
    return;
  }

  mEngine->view()->write( mEngine->formatter()->docTitle( entry->name() ) );

  QString search = mEngine->substituteSearchQuery( entry->search() );

  kdDebug() << "SearchTraverser::startProcess(): search: " << search << endl;

  mJobData = QString::null;

  KIO::TransferJob *job = KIO::get( search );
  connect( job, SIGNAL( result( KIO::Job * ) ),
           SLOT( slotJobResult( KIO::Job * ) ) );
  connect( job, SIGNAL( data( KIO::Job *, const QByteArray & ) ),
           SLOT( slotJobData( KIO::Job *, const QByteArray & ) ) );

//  kdDebug() << "SearchTraverser::startProcess() done: " << entry->name() << endl;
}

DocEntryTraverser *SearchTraverser::createChild()
{
  return new SearchTraverser( mEngine, mLevel + 1 );
}

void SearchTraverser::slotJobResult( KIO::Job *job )
{
  kdDebug() << "SearchTraverser::slotJobResult(): " << mEntry->name() << endl;

  if ( job->error() ) {
    job->showErrorDialog( mEngine->view()->widget() );
  }

  mEngine->view()->write( mEngine->formatter()->processResult( mJobData ) );

  mNotifyee->endProcess( mEntry, this );
}

void SearchTraverser::slotJobData( KIO::Job *, const QByteArray &data )
{
//  kdDebug() << "SearchTraverser::slotJobData()" << endl;

  mJobData.append( data.data() );
}

void SearchTraverser::finishTraversal()
{
  mEngine->view()->write( mEngine->formatter()->footer() );
  mEngine->view()->end();

  mEngine->finishSearch();
}


SearchEngine::SearchEngine( KHTMLPart *destination )
  : QObject(),
    mProc( 0 ), mView( destination ), mRootTraverser( 0 )
{
  mFormatter = new SearchFormatter;
}

SearchEngine::~SearchEngine()
{
  delete mRootTraverser;
  delete mFormatter;
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

bool SearchEngine::search( QString words, QString method, int matches,
                           QString scope )
{
  mWords = words;
  mMethod = method;
  mMatches = matches;
  mScope = scope;
  mLang = "en";

  KConfig *cfg = KGlobal::config();
  cfg->setGroup( "Search" );
  QString commonSearchProgram = cfg->readEntry( "CommonProgram" );
  bool useCommon = cfg->readBoolEntry( "UseCommonProgram", false );
  
  if ( commonSearchProgram.isEmpty() || !useCommon ) {
    if ( !mView ) {
      return false;
    }

    mView->begin();
    mView->write( mFormatter->header() );

    if ( mRootTraverser ) {
      kdDebug() << "SearchEngine::search(): mRootTraverser not null." << endl;
      return false;
    }
    mRootTraverser = new SearchTraverser( this, 0 );
    DocMetaInfo::self()->startTraverseEntries( mRootTraverser );

    return true;
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

    commonSearchProgram = substituteSearchQuery( commonSearchProgram );

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

QString SearchEngine::substituteSearchQuery( const QString &query )
{
  QString result = query;
  result.replace( QRegExp( "%k" ), mWords );
  result.replace( QRegExp( "%n" ), QString::number( mMatches ) );
  result.replace( QRegExp( "%m" ), mMethod );
  result.replace( QRegExp( "%l" ), mLang );
  result.replace( QRegExp( "%s" ), mScope );

  return result;
}

SearchFormatter *SearchEngine::formatter()
{
  return mFormatter;
}

KHTMLPart *SearchEngine::view()
{
  return mView;
}

void SearchEngine::finishSearch()
{
  delete mRootTraverser;
  mRootTraverser = 0;

  emit searchFinished();  
}
