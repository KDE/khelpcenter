#include <qregexp.h>

#include <kdebug.h>
#include <kstandarddirs.h>
#include <klocale.h>
#include <ksimpleconfig.h>

#include "htmlsearch.h"

#include "docentrytraverser.h"

#include "docmetainfo.h"

using namespace KHC;

bool DocMetaInfo::mLoaded = false;

DocMetaInfo *DocMetaInfo::mSelf = 0;

DocMetaInfo *DocMetaInfo::self()
{
  if ( !mSelf ) mSelf = new DocMetaInfo;
  return mSelf;
}

DocMetaInfo::DocMetaInfo()
{
  kdDebug() << "DocMetaInfo()" << endl;

  mHtmlSearch = new HTMLSearch;

  mRootEntry.setName( i18n("Top-Level Documentation") );
}

DocMetaInfo::~DocMetaInfo()
{
  kdDebug() << "~DocMetaInfo()" << endl;

  DocEntry::List::ConstIterator it;
  for( it = mDocEntries.begin(); it != mDocEntries.end(); ++it ) {
    delete *it;
  }

  delete mHtmlSearch;

  mLoaded = false;

  mSelf = 0;
}

DocEntry *DocMetaInfo::addDocEntry( const QString &fileName )
{
  QFileInfo fi( fileName );
  if ( !fi.exists() ) return 0;
  
  QString extension = fi.extension();
  QStringList extensions = QStringList::split( '.', extension );
  QString lang;
  if ( extensions.count() >= 2 ) {
    lang = extensions[ extensions.count() - 2 ];
  }

  if ( !lang.isEmpty() && mLanguages.find( lang ) == mLanguages.end() ) {
    return 0;
  }

  DocEntry *entry = new DocEntry();

  if ( entry->readFromFile( fileName ) ) {
    if ( !lang.isEmpty() && lang != mLanguages.first() ) {
      entry->setLang( lang );
      entry->setName( i18n("doctitle (language)","%1 (%2)")
                           .arg( entry->name() )
                           .arg( mLanguageNames[ lang ] ) );
    }
    if ( entry->searchMethod().lower() == "htdig" ) {
      mHtmlSearch->setupDocEntry( entry );
    }
    QString indexer = entry->indexer();
    indexer.replace( "%f", fileName );
    entry->setIndexer( indexer );
    addDocEntry( entry );
    return entry;
  } else {
    delete entry;
    return 0;
  }
}

void DocMetaInfo::addDocEntry( DocEntry *entry )
{
  mDocEntries.append( entry );
  if ( !entry->search().isEmpty() ) mSearchEntries.append( entry );
}

DocEntry::List DocMetaInfo::docEntries()
{
  return mDocEntries;
}

DocEntry::List DocMetaInfo::searchEntries()
{
  return mSearchEntries;
}

QString DocMetaInfo::languageName( const QString &langcode )
{
  if ( langcode == "en" ) return i18n("English");

  QString cfgfile = locate( "locale",
      QString::fromLatin1( "%1/entry.desktop" ).arg( langcode ) );

  kdDebug() << "-- langcode: " << langcode << " cfgfile: " << cfgfile << endl;
  
  KSimpleConfig cfg( cfgfile );
  cfg.setGroup( "KCM Locale" );
  QString name = cfg.readEntry( "Name", langcode );
  
  return name;
}

void DocMetaInfo::scanMetaInfo( bool force )
{
  if ( mLoaded && !force ) return;

  mLanguages = KGlobal::locale()->languagesTwoAlpha();

  kdDebug( 1400 ) << "LANGS: " << mLanguages.join( " " ) << endl;

  QStringList::ConstIterator it;
  for( it = mLanguages.begin(); it != mLanguages.end(); ++it ) {
    mLanguageNames.insert( *it, languageName( *it ) );
  }

  KConfig config( "khelpcenterrc" );
  config.setGroup( "General" );
  QStringList metaInfos = config.readListEntry( "MetaInfoDirs" );

  if ( metaInfos.isEmpty() ) {
    KStandardDirs* kstd = KGlobal::dirs();
    kstd->addResourceType( "data", "share/apps/khelpcenter" );
    metaInfos = kstd->findDirs( "data", "plugins" );
  }
  for( it = metaInfos.begin(); it != metaInfos.end(); it++) {
    kdDebug() << "DocMetaInfo::scanMetaInfo(): scanning " << *it << endl;
    scanMetaInfoDir( *it, &mRootEntry );
  }

  mLoaded = true;
}

DocEntry *DocMetaInfo::scanMetaInfoDir( const QString &dirName,
                                        DocEntry *parent )
{
  QDir dir( dirName );
  if ( !dir.exists() ) return 0;

  const QFileInfoList *entryList = dir.entryInfoList();
  QFileInfoListIterator it( *entryList );
  QFileInfo *fi;
  for( ; ( fi = it.current() ); ++it ) {
    DocEntry *entry = 0;
    if ( fi->isDir() && fi->fileName() != "." && fi->fileName() != ".." ) {
      DocEntry *dirEntry = addDirEntry( QDir( fi->absFilePath() ), parent );
      entry = scanMetaInfoDir( fi->absFilePath(), dirEntry );
    } else if ( fi->extension( false ) == "desktop" ) {
      entry = addDocEntry( fi->absFilePath() );
      if ( parent && entry ) parent->addChild( entry );
    }
  }

  return 0;
}

DocEntry *DocMetaInfo::addDirEntry( const QDir &dir, DocEntry *parent )
{
  DocEntry *dirEntry = addDocEntry( dir.absPath() + "/.directory" );

  if ( !dirEntry ) {
    dirEntry = new DocEntry;
    dirEntry->setName( dir.dirName() );
    addDocEntry( dirEntry );
  }

  dirEntry->setDirectory( true );

  if ( parent ) parent->addChild( dirEntry );

  return dirEntry;
}


void DocMetaInfo::traverseEntries( DocEntryTraverser *traverser )
{
  traverseEntry( &mRootEntry, traverser );
}

void DocMetaInfo::traverseEntry( DocEntry *entry, DocEntryTraverser *traverser )
{
  DocEntry::List children = entry->children();
  DocEntry::List::ConstIterator it;
  for( it = children.begin(); it != children.end(); ++it ) {
    if ( (*it)->isDirectory() && !(*it)->hasChildren() &&
         (*it)->khelpcenterSpecial().isEmpty() ) continue;
    traverser->process( *it );
    if ( (*it)->hasChildren() ) {
      DocEntryTraverser *t = traverser->childTraverser( *it );
      if (t) {
        traverseEntry( *it, t );
        t->deleteTraverser();
      }
    }
  }
}

void DocMetaInfo::startTraverseEntries( DocEntryTraverser *traverser )
{
  kdDebug() << "DocMetaInfo::startTraverseEntries()" << endl;
  traverser->setNotifyee( this );
  startTraverseEntry( &mRootEntry, traverser );
}

void DocMetaInfo::startTraverseEntry( DocEntry *entry,
                                      DocEntryTraverser *traverser )
{
//  kdDebug() << "DocMetaInfo::startTraverseEntry() " << entry->name() << endl;

  if ( !traverser ) {
    kdDebug() << "DocMetaInfo::startTraverseEntry(): ERROR. No Traverser."
              << endl;
    return;
  }

  if ( !entry ) {
    kdDebug() << "DocMetaInfo::startTraverseEntry(): no entry." << endl;
    endTraverseEntries( traverser );
    return;
  }

  traverser->startProcess( entry );
}

void DocMetaInfo::endProcess( DocEntry *entry, DocEntryTraverser *traverser )
{
//  kdDebug() << "DocMetaInfo::endProcess() " << entry->name() << endl;

  if ( !entry ) {
    endTraverseEntries( traverser );
    return;
  }
  
  if ( entry->hasChildren() ) {
    startTraverseEntry( entry->firstChild(), traverser->childTraverser( entry ) );
  } else if ( entry->nextSibling() ) {
    startTraverseEntry( entry->nextSibling(), traverser );
  } else {
    DocEntry *parent = entry->parent();
    DocEntryTraverser *parentTraverser = 0;
    while ( parent ) {
      parentTraverser = traverser->parentTraverser();
      traverser->deleteTraverser();
      if ( parent->nextSibling() ) {
        startTraverseEntry( parent->nextSibling(), parentTraverser );
        break;
      } else {
        parent = parent->parent();
        traverser = parentTraverser;
      }
    }
    if ( !parent ) {
      endTraverseEntries( traverser );
    }
  }
}

void DocMetaInfo::endTraverseEntries( DocEntryTraverser *traverser )
{
  kdDebug() << "DocMetaInfo::endTraverseEntries()" << endl;

  if ( !traverser ) {
    kdDebug() << " no more traversers." << endl;
    return;
  }

  traverser->finishTraversal();
}
// vim:ts=2:sw=2:et
