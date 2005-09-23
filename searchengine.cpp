#include "stdlib.h"

#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kstandarddirs.h>
#include <kprocess.h>
#include <klocale.h>
#include <kmessagebox.h>

#include "docmetainfo.h"
#include "formatter.h"
#include "view.h"
#include "searchhandler.h"
#include "prefs.h"

#include "searchengine.h"

namespace KHC
{

SearchTraverser::SearchTraverser( SearchEngine *engine, int level ) :
  mMaxLevel( 999 ), mEngine( engine), mLevel( level )
{
#if 0
  kdDebug() << "SearchTraverser(): " << mLevel
    << "  0x" << QString::number( int( this ), 16 ) << endl;
#endif
}

SearchTraverser::~SearchTraverser()
{
#if 0
    kdDebug() << "~SearchTraverser(): " << mLevel
      << "  0x" << QString::number( int( this ), 16 ) << endl;
#endif

  QString section;
  if ( parentEntry() ) {
    section = parentEntry()->name();
  } else {
    section = ("Unknown Section");
  }

  if ( !mResult.isEmpty() ) {
    mEngine->view()->writeSearchResult(
      mEngine->formatter()->sectionHeader( section ) );
    mEngine->view()->writeSearchResult( mResult );
  }
}

void SearchTraverser::process( DocEntry * )
{
  kdDebug() << "SearchTraverser::process()" << endl;
}

void SearchTraverser::startProcess( DocEntry *entry )
{
//  kdDebug() << "SearchTraverser::startProcess(): " << entry->name() << "  "
//    << "SEARCH: '" << entry->search() << "'" << endl;

  if ( !mEngine->canSearch( entry ) || !entry->searchEnabled() ) {
    mNotifyee->endProcess( entry, this );
    return;
  }

//  kdDebug() << "SearchTraverser::startProcess(): " << entry->identifier()
//    << endl;

  SearchHandler *handler = mEngine->handler( entry->documentType() );

  if ( !handler ) {
    QString txt;
    if ( entry->documentType().isEmpty() ) {
      txt = i18n("Error: No document type specified.");
    } else { 
      txt = i18n("Error: No search handler for document type '%1'.")
        .arg( entry->documentType() );
    }
    showSearchError( handler, entry, txt );
    return;
  }
  
  connectHandler( handler );

  handler->search( entry, mEngine->words(), mEngine->maxResults(),
    mEngine->operation() );

//  kdDebug() << "SearchTraverser::startProcess() done: " << entry->name() << endl;
}

void SearchTraverser::connectHandler( SearchHandler *handler )
{
  QMap<SearchHandler *,int>::Iterator it;
  it = mConnectCount.find( handler );
  int count = 0;
  if ( it != mConnectCount.end() ) count = *it;
  if ( count == 0 ) {
    connect( handler, SIGNAL( searchError( SearchHandler *, DocEntry *, const QString & ) ),
      SLOT( showSearchError( SearchHandler *, DocEntry *, const QString & ) ) );
    connect( handler, SIGNAL( searchFinished( SearchHandler *, DocEntry *, const QString & ) ),
      SLOT( showSearchResult( SearchHandler *, DocEntry *, const QString & ) ) );
  }
  mConnectCount[ handler ] = ++count;
}

void SearchTraverser::disconnectHandler( SearchHandler *handler )
{
  QMap<SearchHandler *,int>::Iterator it;
  it = mConnectCount.find( handler );
  if ( it == mConnectCount.end() ) {
    kdError() << "SearchTraverser::disconnectHandler() handler not connected."
      << endl;
  } else {
    int count = *it;
    --count;
    if ( count == 0 ) {
      disconnect( handler, SIGNAL( searchError( SearchHandler *, DocEntry *, const QString & ) ),
        this, SLOT( showSearchError( SearchHandler *, DocEntry *, const QString & ) ) );
      disconnect( handler, SIGNAL( searchFinished( SearchHandler *, DocEntry *, const QString & ) ),
        this, SLOT( showSearchResult( SearchHandler *, DocEntry *, const QString & ) ) );
    }
    mConnectCount[ handler ] = count;
  }
}

DocEntryTraverser *SearchTraverser::createChild( DocEntry *parentEntry )
{
//  kdDebug() << "SearchTraverser::createChild() level " << mLevel << endl;

  if ( mLevel >= mMaxLevel ) {
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

  if ( mLevel > mMaxLevel ) {
    return this;
  } else {
    return mParent;
  }
}

void SearchTraverser::deleteTraverser()
{
//  kdDebug() << "SearchTraverser::deleteTraverser()" << endl;

  if ( mLevel > mMaxLevel ) {
    --mLevel;
  } else {
    delete this;
  }
}

void SearchTraverser::showSearchError( SearchHandler *handler, DocEntry *entry, const QString &error )
{
//  kdDebug() << "SearchTraverser::showSearchError(): " << entry->name()
//    << endl;

  mResult += mEngine->formatter()->docTitle( entry->name() );
  mResult += mEngine->formatter()->paragraph( error );

  mEngine->logError( entry, error );

  disconnectHandler( handler );

  mNotifyee->endProcess( entry, this );
}

void SearchTraverser::showSearchResult( SearchHandler *handler, DocEntry *entry, const QString &result )
{
//  kdDebug() << "SearchTraverser::showSearchResult(): " << entry->name()
//    << endl;

  mResult += mEngine->formatter()->docTitle( entry->name() );
  mResult += mEngine->formatter()->processResult( result );

  disconnectHandler( handler );

  mNotifyee->endProcess( entry, this );
}

void SearchTraverser::finishTraversal()
{
//  kdDebug() << "SearchTraverser::finishTraversal()" << endl;

  mEngine->view()->writeSearchResult( mEngine->formatter()->footer() );
  mEngine->view()->endSearchResult();

  mEngine->finishSearch();
}


SearchEngine::SearchEngine( View *destination )
  : QObject(),
    mProc( 0 ), mSearchRunning( false ), mView( destination ),
    mRootTraverser( 0 )
{
  mLang = KGlobal::locale()->language().left( 2 );
}

SearchEngine::~SearchEngine()
{
  delete mRootTraverser;
}

bool SearchEngine::initSearchHandlers()
{
  QStringList resources = KGlobal::dirs()->findAllResources(
    "appdata", "searchhandlers/*.desktop" );
  QStringList::ConstIterator it;
  for( it = resources.begin(); it != resources.end(); ++it ) {
    QString filename = *it;
    kdDebug() << "SearchEngine::initSearchHandlers(): " << filename << endl;
    SearchHandler *handler = SearchHandler::initFromFile( filename );
    if ( !handler || !handler->checkPaths() ) {
      QString txt = i18n("Unable to initialize SearchHandler from file '%1'.")
        .arg( filename );
      kdWarning() << txt << endl;
//      KMessageBox::sorry( mView->widget(), txt );        
    } else {
      QStringList documentTypes = handler->documentTypes();
      QStringList::ConstIterator it;
      for( it = documentTypes.begin(); it != documentTypes.end(); ++it ) {
        mHandlers.insert( *it, handler );
      }
    }
  }
  
  if ( mHandlers.isEmpty() ) {
    QString txt = i18n("No valid search handler found.");
    kdWarning() << txt << endl;
//    KMessageBox::sorry( mView->widget(), txt );
    return false;
  }

  return true;
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

void SearchEngine::searchStderr(KProcess *, char *buffer, int len)
{
  if ( !buffer || len == 0 )
    return;

  mStderr.append( QString::fromUtf8( buffer, len ) );
}

void SearchEngine::searchExited(KProcess *)
{
  kdDebug() << "Search terminated" << endl;
  mSearchRunning = false;
}

bool SearchEngine::search( QString words, QString method, int matches,
                           QString scope )
{
  if ( mSearchRunning ) return false;

  // These should be removed
  mWords = words;
  mMethod = method;
  mMatches = matches;
  mScope = scope;

  // Saner variables to store search parameters:
  mWordList = words.split( " ");
  mMaxResults = matches;
  if ( method == "or" ) mOperation = Or;
  else mOperation = And;

  KConfig *cfg = KGlobal::config();
  cfg->setGroup( "Search" );
  QString commonSearchProgram = cfg->readPathEntry( "CommonProgram" );
  bool useCommon = cfg->readBoolEntry( "UseCommonProgram", false );
  
  if ( commonSearchProgram.isEmpty() || !useCommon ) {
    if ( !mView ) {
      return false;
    }

    QString txt = i18n("Search Results for '%1':").arg( words );

    mStderr = "<b>" + txt + "</b>\n";

    mView->beginSearchResult();
    mView->writeSearchResult( formatter()->header( i18n("Search Results") ) );
    mView->writeSearchResult( formatter()->title( txt ) );

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
      mWords.replace("&", " ");
      method = "and";
    }
 
    // replace whitespace with a '+'
    mWords = mWords.stripWhiteSpace();
    mWords = mWords.simplifyWhiteSpace();
    mWords.replace(QRegExp("\\s"), "+");
    
    commonSearchProgram = substituteSearchQuery( commonSearchProgram );

    kdDebug() << "Common Search: " << commonSearchProgram << endl;

    mProc = new KProcess();

    QStringList cmd = commonSearchProgram.split( " ");
    QStringList::ConstIterator it;
    for( it = cmd.begin(); it != cmd.end(); ++it ) {
      QString arg = *it;
      if ( arg.left( 1 ) == "\"" && arg.right( 1 ) =="\"" ) {
        arg = arg.mid( 1, arg.length() - 2 );
      }
      *mProc << arg.utf8();
    }

    connect( mProc, SIGNAL( receivedStdout( KProcess *, char *, int ) ),
             SLOT( searchStdout( KProcess *, char *, int ) ) );
    connect( mProc, SIGNAL( receivedStderr( KProcess *, char *, int ) ),
             SLOT( searchStderr( KProcess *, char *, int ) ) );
    connect( mProc, SIGNAL( processExited( KProcess * ) ),
             SLOT( searchExited( KProcess * ) ) );

    mSearchRunning = true;
    mSearchResult = "";
    mStderr = "<b>" + commonSearchProgram + "</b>\n\n";

    mProc->start(KProcess::NotifyOnExit, KProcess::All);

    while (mSearchRunning && mProc->isRunning())
      kapp->processEvents();

    if ( !mProc->normalExit() || mProc->exitStatus() != 0 ) {
      kdError() << "Unable to run search program '" << commonSearchProgram
                << "'" << endl;
      delete mProc;
      
      return false;
    }

    delete mProc;

    // modify the search result
    mSearchResult = mSearchResult.replace("http://localhost/", "file:/");
    mSearchResult = mSearchResult.mid( mSearchResult.find( '<' ) );

    mView->beginSearchResult();
    mView->writeSearchResult( mSearchResult );
    mView->endSearchResult();

    emit searchFinished();
  }

  return true;
}

QString SearchEngine::substituteSearchQuery( const QString &query )
{
  QString result = query;
  result.replace( "%k", mWords );
  result.replace( "%n", QString::number( mMatches ) );
  result.replace( "%m", mMethod );
  result.replace( "%l", mLang );
  result.replace( "%s", mScope );

  return result;
}

QString SearchEngine::substituteSearchQuery( const QString &query,
  const QString &identifier, const QStringList &words, int maxResults,
  Operation operation, const QString &lang )
{
  QString result = query;
  result.replace( "%i", identifier );
  result.replace( "%w", words.join( "+" ) );
  result.replace( "%m", QString::number( maxResults ) );
  QString o;
  if ( operation == Or ) o = "or";
  else o = "and";
  result.replace( "%o", o );
  result.replace( "%d", Prefs::indexDirectory() );
  result.replace( "%l", lang );

  return result;
}

Formatter *SearchEngine::formatter() const
{
  return mView->formatter();
}

View *SearchEngine::view() const
{
  return mView;
}

void SearchEngine::finishSearch()
{
  delete mRootTraverser;
  mRootTraverser = 0;

  emit searchFinished();  
}

QString SearchEngine::errorLog() const
{
  return mStderr;
}

void SearchEngine::logError( DocEntry *entry, const QString &msg )
{
  mStderr += entry->identifier() + ": " + msg;
}

bool SearchEngine::isRunning() const
{
  return mSearchRunning;
}

SearchHandler *SearchEngine::handler( const QString &documentType ) const
{
  QMap<QString,SearchHandler *>::ConstIterator it;
  it = mHandlers.find( documentType );

  if ( it == mHandlers.end() ) return 0;
  else return *it;
}

QStringList SearchEngine::words() const
{
  return mWordList;
}

int SearchEngine::maxResults() const
{
  return mMaxResults;
}

SearchEngine::Operation SearchEngine::operation() const
{
  return mOperation;
}

bool SearchEngine::canSearch( DocEntry *entry )
{
  return entry->docExists() && !entry->documentType().isEmpty() &&
    handler( entry->documentType() );
}

bool SearchEngine::needsIndex( DocEntry *entry )
{
  if ( !canSearch( entry ) ) return false;

  SearchHandler *h = handler( entry->documentType() );
  if ( h->indexCommand( entry->identifier() ).isEmpty() ) return false;
  
  return true;
}

}

#include "searchengine.moc"

// vim:ts=2:sw=2:et
