#include <kdebug.h>
#include <kconfig.h>

#include "docentry.h"

#include "htmlsearch.h"
#include "htmlsearch.moc"

using namespace KHC;

HTMLSearch::HTMLSearch()
{
  mConfig = new KConfig("khelpcenterrc", true);
  mConfig->setGroup( "htdig" );
}

HTMLSearch::~HTMLSearch()
{
    delete mConfig;
}

void HTMLSearch::setupDocEntry( KHC::DocEntry *entry )
{
//  kDebug() << "HTMLSearch::setupDocEntry(): " << entry->name() << endl;

  if ( entry->searchMethod().toLower() != QLatin1String("htdig") ) return;

  if ( entry->search().isEmpty() )
    entry->setSearch( defaultSearch( entry ) );
  if ( entry->indexer().isEmpty() )
    entry->setIndexer( defaultIndexer( entry ) );
  if ( entry->indexTestFile().isEmpty() )
    entry->setIndexTestFile( defaultIndexTestFile( entry ) );

//  entry->dump();
}

QString HTMLSearch::defaultSearch( KHC::DocEntry *entry )
{
  QString htsearch = QLatin1String("cgi:");
  htsearch += mConfig->readPathEntry( "htsearch" );
  htsearch += "?words=%k&method=and&format=-desc&config=";
  htsearch += entry->identifier();

  return htsearch;
}

QString HTMLSearch::defaultIndexer( KHC::DocEntry * )
{
  QString indexer = mConfig->readPathEntry( "indexer" );
  indexer += " --indexdir=%i %f";

  return indexer;
}

QString HTMLSearch::defaultIndexTestFile( KHC::DocEntry *entry )
{
  return entry->identifier() + QLatin1String(".exists");
}

// vim:ts=2:sw=2:et
