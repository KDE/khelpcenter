#include "stdlib.h"


#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kstandarddirs.h>
#include <kprocess.h>
#include <klocale.h>

#include "docmetainfo.h"
#include "searchformatter.h"
#include "view.h"

#include "searchengine.h"
#include "searchengine.moc"

namespace KHC
{

SearchTraverser::SearchTraverser( SearchEngine *engine, int level ) :
  mEngine( engine), mLevel( level ), mEntry( 0 )
{
#if 0
  kdDebug() << "SearchTraverser(): " << mLevel << endl;

  kdDebug() << "  0x" << QString::number( int( this ), 16 ) << endl;
#endif
}

SearchTraverser::~SearchTraverser()
{
#if 0
  if ( mEntry ) {
    kdDebug() << "~SearchTraverser(): " << mLevel << " " << mEntry->name() << endl;
  } else {
    kdDebug() << "~SearchTraverser(): null entry" << endl;
  }

  kdDebug() << "  0x" << QString::number( int( this ), 16 ) << endl;
#endif

  QString section;
  if ( parentEntry() ) {
    section = parentEntry()->name();
  } else {
    section = ("Unknown Section");
  }

  if ( !mResult.isEmpty() ) {
    mEngine->view()->writeSearchResult( mEngine->formatter()->sectionHeader( section ) );
    mEngine->view()->writeSearchResult( mResult );
  }
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

DocEntryTraverser *SearchTraverser::createChild( DocEntry *parentEntry )
{
  if ( mLevel >= 3 ) {
    ++mLevel;
    return this;
  } else {
    DocEntryTraverser *t = new SearchTraverser( mEngine, mLevel + 1 );
    t->setParentEntry( parentEntry );
    return t;
  }
}

DocEntryTraverser *SearchTraverser::parentTraverser()
{
//  kdDebug() << "SearchTraverser::parentTraverser(): level: " << mLevel << endl;

  if ( mLevel > 3 ) {
    return this;
  } else {
    return mParent;
  }
}

void SearchTraverser::deleteTraverser()
{
  if ( mLevel > 3 ) {
    --mLevel;
  } else {
    delete this;
  }
}

void SearchTraverser::slotJobResult( KIO::Job *job )
{
  kdDebug() << "SearchTraverser::slotJobResult(): " << mEntry->name() << endl;

  if ( job->error() ) {
    job->showErrorDialog( mEngine->view()->widget() );
  }

  mResult += mEngine->formatter()->docTitle( mEntry->name() );
  mResult += mEngine->formatter()->processResult( mJobData );

  mNotifyee->endProcess( mEntry, this );
}

void SearchTraverser::slotJobData( KIO::Job *, const QByteArray &data )
{
//  kdDebug() << "SearchTraverser::slotJobData()" << endl;

  mJobData.append( data.data() );
}

void SearchTraverser::finishTraversal()
{
  mEngine->view()->writeSearchResult( mEngine->formatter()->footer() );
  mEngine->view()->endSearchResult();

  mEngine->finishSearch();
}


SearchEngine::SearchEngine( View *destination )
  : QObject(),
    mProc( 0 ), mView( destination ), mRootTraverser( 0 )
{
  mFormatter = new SearchFormatter;

  mLang = KGlobal::locale()->language().left( 2 );
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

bool SearchEngine::search( QString _words, QString method, int matches,
                           QString scope )
{
  mWords = _words;
  mMethod = method;
  mMatches = matches;
  mScope = scope;

  KConfig *cfg = KGlobal::config();
  cfg->setGroup( "Search" );
  QString commonSearchProgram = cfg->readEntry( "CommonProgram" );
  bool useCommon = cfg->readBoolEntry( "UseCommonProgram", false );
  
  if ( commonSearchProgram.isEmpty() || !useCommon ) {
    if ( !mView ) {
      return false;
    }

    mView->beginSearchResult();
    mView->writeSearchResult( mFormatter->header() );

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
    if (mWords.find("&") != -1) {
      mWords.replace(QRegExp("&"), " ");
      method = "and";
    }
 
    // replace whitespace with a '+'
    mWords = mWords.stripWhiteSpace();
    mWords = mWords.simplifyWhiteSpace();
    mWords.replace(QRegExp("\\s"), "+");
    
    commonSearchProgram = substituteSearchQuery( commonSearchProgram );

    kdDebug() << "Common Search: " << commonSearchProgram << endl;

    mProc = new KProcess();

    QStringList cmd = QStringList::split( " ", commonSearchProgram );
    QStringList::ConstIterator it;
    for( it = cmd.begin(); it != cmd.end(); ++it ) {
      QString arg = *it;
      if ( arg.left( 1 ) == "\"" && arg.right( 1 ) =="\"" ) {
        arg = arg.mid( 1, arg.length() - 2 );
      }
      *mProc << arg;
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

    mView->beginSearchResult();
    mView->writeSearchResult( mSearchResult );
    mView->endSearchResult();
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

View *SearchEngine::view()
{
  return mView;
}

void SearchEngine::finishSearch()
{
  delete mRootTraverser;
  mRootTraverser = 0;

  emit searchFinished();  
}

}
// vim:ts=2:sw=2:et
