
#include "searchengine.h"

#include "docmetainfo.h"
#include "view.h"
#include "searchhandler.h"
#include "khc_debug.h"
#include "grantleeformatter.h"

#include <prefs.h>

namespace KHC
{

SearchTraverser::SearchTraverser( SearchEngine *engine, int level ) :
  mMaxLevel( 999 ), mEngine( engine), mLevel( level ), mResultsPtr( &mResults )
{
#if 0
  khcDebug() << "SearchTraverser():" << mLevel << this;
#endif
}

SearchTraverser::~SearchTraverser()
{
#if 0
    khcDebug() << "~SearchTraverser():" << mLevel << this;
#endif
}

void SearchTraverser::process( DocEntry * )
{
  khcDebug() << "SearchTraverser::process()";
}

void SearchTraverser::startProcess( DocEntry *entry )
{
//  khcDebug() << "SearchTraverser::startProcess():" << entry->name()
//    << "SEARCH:" << entry->search();

  if ( !mEngine->canSearch( entry ) || !entry->searchEnabled() ) {
    mNotifyee->endProcess( entry, this );
    return;
  }

//  khcDebug() << "SearchTraverser::startProcess():" << entry->identifier();

  SearchHandler *handler = mEngine->handler( entry->documentType() );

  if ( !handler ) {
    QString txt;
    if ( entry->documentType().isEmpty() ) {
      txt = i18n("Error: No document type specified.");
    } else {
      txt = i18n("Error: No search handler for document type '%1'.",
          entry->documentType() );
    }
    showSearchError( handler, entry, txt );
    return;
  }

  connectHandler( handler );

  handler->search( entry, mEngine->words(), mEngine->maxResults(),
    mEngine->operation() );

//  khcDebug() << "SearchTraverser::startProcess() done: " << entry->name();
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
    khcWarning() << "SearchTraverser::disconnectHandler() handler not connected.";
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
//  khcDebug() << "SearchTraverser::createChild() level " << mLevel;

  if ( mLevel >= mMaxLevel ) {
    ++mLevel;
    return this;
  } else {
    SearchTraverser *t = new SearchTraverser( mEngine, mLevel + 1 );
    t->mResultsPtr = mResultsPtr;
    t->setParentEntry( parentEntry );
    return t;
  }
}

DocEntryTraverser *SearchTraverser::parentTraverser()
{
//  khcDebug() << "SearchTraverser::parentTraverser(): level: " << mLevel;

  if ( mLevel > mMaxLevel ) {
    return this;
  } else {
    return mParent;
  }
}

void SearchTraverser::deleteTraverser()
{
//  khcDebug() << "SearchTraverser::deleteTraverser()";

  if ( mLevel > mMaxLevel ) {
    --mLevel;
  } else {
    delete this;
  }
}

void SearchTraverser::showSearchError( SearchHandler *handler, DocEntry *entry, const QString &error )
{
//  khcDebug() << "SearchTraverser::showSearchError():" << entry->name();

  mResultsPtr->append( qMakePair( entry, error ) );

  mEngine->logError( entry, error );

  disconnectHandler( handler );

  mNotifyee->endProcess( entry, this );
}

void SearchTraverser::showSearchResult( SearchHandler *handler, DocEntry *entry, const QString &result )
{
//  khcDebug() << "SearchTraverser::showSearchResult():" << entry->name();

  mResultsPtr->append( qMakePair( entry, result ) );

  disconnectHandler( handler );

  mNotifyee->endProcess( entry, this );
}

void SearchTraverser::finishTraversal()
{
//  khcDebug() << "SearchTraverser::finishTraversal()";

  mEngine->view()->beginSearchResult();
  mEngine->view()->writeSearchResult( mEngine->grantleeFormatter()->formatSearchResults( mWords, mResults ) );
  mEngine->view()->endSearchResult();

  mEngine->finishSearch();
}

void SearchTraverser::setWords( const QString &words )
{
  mWords = words;
}


SearchEngine::SearchEngine( View *destination )
  : QObject(),
    mSearchRunning( false ), mView( destination ),
    mRootTraverser( 0 )
{
}

SearchEngine::~SearchEngine()
{
  delete mRootTraverser;
}

bool SearchEngine::initSearchHandlers()
{
  const QStringList resourceDirs = QStandardPaths::locateAll(QStandardPaths::DataLocation, "searchhandlers/", QStandardPaths::LocateDirectory );
  QStringList resources;
  foreach(const QString& dir, resourceDirs) {
      QDir d(dir);
    foreach ( const QString& entry, d.entryList( QStringList( "*.desktop" ), QDir::Files ) ) {
      resources += dir + entry;
    }
  }

  QStringList::ConstIterator it;
  for( it = resources.constBegin(); it != resources.constEnd(); ++it ) {
    QString filename = *it;
    khcDebug() << "SearchEngine::initSearchHandlers(): " << filename;
    QSharedPointer<SearchHandler> handler( SearchHandler::initFromFile( filename ) );
    if ( !handler ) {
      khcWarning() << "Unable to initialize SearchHandler from" << filename;
    } else {
      QStringList documentTypes = handler->documentTypes();
      QStringList::ConstIterator it;
      for( it = documentTypes.constBegin(); it != documentTypes.constEnd(); ++it ) {
        // only register "handler" for this content type if there is no
        // handler set already; since we read more specific directories
        // first, then those will override whatever is in more global
        // ones
        if ( !mHandlers.contains( *it ) ) {
          mHandlers.insert( *it, handler );
        }
      }
    }
  }

  if ( mHandlers.isEmpty() ) {
    QString txt = i18n("No valid search handler found.");
    khcWarning() << txt;
//    KMessageBox::sorry( mView->widget(), txt );
    return false;
  }

  return true;
}

bool SearchEngine::search( const QString & words, const QString & method, int matches,
                           const QStringList & scope )
{
  if ( mSearchRunning ) return false;

  // These should be removed
  mWords = words;
  mMethod = method;
  mMatches = matches;
  mScope = scope;

  // Saner variables to store search parameters:
  mWordList = words.split(' ');
  mMaxResults = matches;
  if ( method == "or" ) mOperation = Or;
  else mOperation = And;

  if ( !mView ) {
    return false;
  }

  QString txt = i18n("Search Results for '%1':", words.toHtmlEscaped() );

  mStderr = "<b>" + txt + "</b>\n";

  if ( mRootTraverser ) {
    khcDebug() << "SearchEngine::search(): mRootTraverser not null.";
    return false;
  }
  mRootTraverser = new SearchTraverser( this, 0 );
  mRootTraverser->setWords( words );
  DocMetaInfo::self()->startTraverseEntries( mRootTraverser );

  return true;
}

QString SearchEngine::substituteSearchQuery( const QString &query,
  const QString &identifier, const QStringList &words, int maxResults,
  Operation operation, const QString &lang, const QString& binary )
{
  QString result = query;
  result.replace( QLatin1String("%i"), identifier );
  result.replace( QLatin1String("%w"), words.join( "+" ) );
  result.replace( QLatin1String("%m"), QString::number( maxResults ) );
  QString o = QLatin1String(operation == Or ? "or" : "and");
  result.replace( QLatin1String("%o"), o );
  result.replace( QLatin1String("%d"), Prefs::indexDirectory() );
  result.replace( QLatin1String("%l"), lang );
  result.replace( QLatin1String("%b"), binary );

  return result;
}

GrantleeFormatter *SearchEngine::grantleeFormatter() const
{
  return mView->grantleeFormatter();
}

View *SearchEngine::view() const
{
  return mView;
}

void SearchEngine::finishSearch()
{
  delete mRootTraverser;
  mRootTraverser = 0;
  mSearchRunning = false;

  emit searchFinished();
}

QString SearchEngine::errorLog() const
{
  return mStderr;
}

void SearchEngine::logError( DocEntry *entry, const QString &msg )
{
  mStderr += entry->identifier() + QLatin1String(": ") + msg;
}

bool SearchEngine::isRunning() const
{
  return mSearchRunning;
}

SearchHandler *SearchEngine::handler( const QString &documentType ) const
{
  QSharedPointer<SearchHandler> ptr = mHandlers.value( documentType );
  return ptr ? ptr.data() : 0;
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
  if ( !h || h->indexCommand( entry->identifier() ).isEmpty() ) return false;

  return true;
}

}


// vim:ts=2:sw=2:et
