#include <kdebug.h>
#include <kstandarddirs.h>
#include <klocale.h>
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

void HTMLSearch::setupDocEntry( KHC::DocEntry *entry )
{
  kdDebug() << "HTMLSearch::setupDocEntry(): " << entry->name() << endl;

  if ( entry->searchMethod().lower() != "htdig" ) return;

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
  QString htsearch = "cgi:";
  htsearch += mConfig->readEntry( "htsearch" );
  htsearch += "?words=%k&method=and&format=-desc&config=";
  htsearch += entry->identifier();

  return htsearch;
}

QString HTMLSearch::defaultIndexer( KHC::DocEntry * )
{
  QString indexer = mConfig->readEntry( "indexer" );
  indexer += " %f";

  return indexer;
}

QString HTMLSearch::defaultIndexTestFile( KHC::DocEntry *entry )
{
  QString testFile = mConfig->readEntry( "dbdir" );
  testFile += entry->identifier() + ".docs.index";

  return testFile;
}
// vim:ts=2:sw=2:et
