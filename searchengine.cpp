
#include "searchengine.h"

#include "stdlib.h"

#include <QTextDocument>
#include <KApplication>
#include <KConfig>
#include <KDebug>
#include <KStandardDirs>
#include <KProcess>
#include <KShell>
#include <KLocale>
#include <KMessageBox>

#include "docmetainfo.h"
#include "formatter.h"
#include "view.h"
#include "searchhandler.h"
#include "prefs.h"

namespace KHC
{

SearchTraverser::SearchTraverser( SearchEngine *engine, int level ) :
  mMaxLevel( 999 ), mEngine( engine), mLevel( level )
{
#if 0
  kDebug() << "SearchTraverser(): " << mLevel
    << "  0x" << QString::number( int( this ), 16 ) << endl;
#endif
}

SearchTraverser::~SearchTraverser()
{
#if 0
    kDebug() << "~SearchTraverser(): " << mLevel
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
  qDebug() << "SearchTraverser::process()";
}

void SearchTraverser::startProcess( DocEntry *entry )
{
//  kDebug() << "SearchTraverser::startProcess(): " << entry->name() << "  "
//    << "SEARCH: '" << entry->search() << "'" << endl;

  if ( !mEngine->canSearch( entry ) || !entry->searchEnabled() ) {
    mNotifyee->endProcess( entry, this );
    return;
  }

//  kDebug() << "SearchTraverser::startProcess(): " << entry->identifier()
//    << endl;

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

//  kDebug() << "SearchTraverser::startProcess() done: " << entry->name();
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
    qWarning() << "SearchTraverser::disconnectHandler() handler not connected."
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
//  kDebug() << "SearchTraverser::createChild() level " << mLevel;

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
//  kDebug() << "SearchTraverser::parentTraverser(): level: " << mLevel;

  if ( mLevel > mMaxLevel ) {
    return this;
  } else {
    return mParent;
  }
}

void SearchTraverser::deleteTraverser()
{
//  kDebug() << "SearchTraverser::deleteTraverser()";

  if ( mLevel > mMaxLevel ) {
    --mLevel;
  } else {
    delete this;
  }
}

void SearchTraverser::showSearchError( SearchHandler *handler, DocEntry *entry, const QString &error )
{
//  kDebug() << "SearchTraverser::showSearchError(): " << entry->name()
//    << endl;

  mResult += mEngine->formatter()->docTitle( entry->name() );
  mResult += mEngine->formatter()->paragraph( error );

  mEngine->logError( entry, error );

  disconnectHandler( handler );

  mNotifyee->endProcess( entry, this );
}

void SearchTraverser::showSearchResult( SearchHandler *handler, DocEntry *entry, const QString &result )
{
//  kDebug() << "SearchTraverser::showSearchResult(): " << entry->name()
//    << endl;

  mResult += mEngine->formatter()->docTitle( entry->name() );
  mResult += mEngine->formatter()->processResult( result );

  disconnectHandler( handler );

  mNotifyee->endProcess( entry, this );
}

void SearchTraverser::finishTraversal()
{
//  kDebug() << "SearchTraverser::finishTraversal()";

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
  const QStringList resources = KGlobal::dirs()->findAllResources(
    "appdata", "searchhandlers/*.desktop" );
  QStringList::ConstIterator it;
  for( it = resources.constBegin(); it != resources.constEnd(); ++it ) {
    QString filename = *it;
    qDebug() << "SearchEngine::initSearchHandlers(): " << filename;
    SearchHandler *handler = SearchHandler::initFromFile( filename );
    if ( !handler ) {
      QString txt = i18n("Unable to initialize SearchHandler from file '%1'.",
          filename );
//      KMessageBox::sorry( mView->widget(), txt );
    } else {
      QStringList documentTypes = handler->documentTypes();
      QStringList::ConstIterator it;
      for( it = documentTypes.constBegin(); it != documentTypes.constEnd(); ++it ) {
        mHandlers.insert( *it, handler );
      }
    }
  }

  if ( mHandlers.isEmpty() ) {
    QString txt = i18n("No valid search handler found.");
    qWarning() << txt;
//    KMessageBox::sorry( mView->widget(), txt );
    return false;
  }

  return true;
}

void SearchEngine::searchExited(int exitCode, QProcess::ExitStatus exitStatus)
{
  Q_UNUSED(exitCode);
  Q_UNUSED(exitStatus);
  qDebug() << "Search terminated";
  mSearchRunning = false;
}

bool SearchEngine::search( const QString & words, const QString & method, int matches,
                           const QString & scope )
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

  KConfigGroup cfg(KSharedConfig::openConfig(), "Search");
  QString commonSearchProgram = cfg.readPathEntry( "CommonProgram", QString() );
  bool useCommon = cfg.readEntry( "UseCommonProgram", false);

  if ( commonSearchProgram.isEmpty() || !useCommon ) {
    if ( !mView ) {
      return false;
    }

    QString txt = i18n("Search Results for '%1':", Qt::escape(words) );

    mStderr = "<b>" + txt + "</b>\n";

    mView->beginSearchResult();
    mView->writeSearchResult( formatter()->header( i18n("Search Results") ) );
    mView->writeSearchResult( formatter()->title( txt ) );

    if ( mRootTraverser ) {
      qDebug() << "SearchEngine::search(): mRootTraverser not null.";
      return false;
    }
    mRootTraverser = new SearchTraverser( this, 0 );
    DocMetaInfo::self()->startTraverseEntries( mRootTraverser );

    return true;
  } else {
    QString lang = KGlobal::locale()->language().left(2);

    if ( lang.toLower() == "c" || lang.toLower() == "posix" )
	  lang = "en";

    // if the string contains '&' replace with a '+' and set search method to and
    if (mWords.indexOf("&") != -1) {
      mWords.replace('&', ' ');
      mMethod = "and";
    }

    // replace whitespace with a '+'
    mWords = mWords.trimmed();
    mWords = mWords.simplified();
    mWords.replace(QRegExp("\\s"), "+");

    commonSearchProgram = substituteSearchQuery( commonSearchProgram );

    qDebug() << "Common Search: " << commonSearchProgram;

    mProc = new KProcess();
    *mProc << KShell::splitArgs(commonSearchProgram);

    connect( mProc, SIGNAL( finished(int, QProcess::ExitStatus) ),
             this, SLOT( searchExited(int, QProcess::ExitStatus) ) );

    mSearchRunning = true;
    mSearchResult = "";
    mStderr = "<b>" + commonSearchProgram + "</b>\n\n";

    mProc->start();
    if (!mProc->waitForStarted()) {
      qWarning() << "could not start search program '" << commonSearchProgram << "'";
      delete mProc;
      return false;
    }

    while (mSearchRunning && mProc->state() == QProcess::Running)
      kapp->processEvents();

    // no need to use signals/slots
    mStderr += mProc->readAllStandardError();
    mSearchResult += mProc->readAllStandardOutput();

    if ( mProc->exitStatus() == KProcess::CrashExit || mProc->exitCode() != 0 ) {
      qWarning() << "Unable to run search program '" << commonSearchProgram << "'" << endl;
      delete mProc;

      return false;
    }

    delete mProc;

    // modify the search result
    mSearchResult = mSearchResult.replace("http://localhost/", "file:/");
    mSearchResult = mSearchResult.mid( mSearchResult.indexOf( '<' ) );

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
  result.replace( QLatin1String("%k"), mWords );
  result.replace( QLatin1String("%n"), QString::number( mMatches ) );
  result.replace( QLatin1String("%m"), mMethod );
  result.replace( QLatin1String("%l"), mLang );
  result.replace( QLatin1String("%s"), mScope );

  return result;
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
  mStderr += entry->identifier() + QLatin1String(": ") + msg;
}

bool SearchEngine::isRunning() const
{
  return mSearchRunning;
}

SearchHandler *SearchEngine::handler( const QString &documentType ) const
{
  return mHandlers.value( documentType, 0 );
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

#include "searchengine.moc"

// vim:ts=2:sw=2:et
