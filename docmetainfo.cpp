#include <qfile.h>
#include <qregexp.h>
#include <qfileinfo.h>
#include <qdir.h>

#include <kdebug.h>
#include <kdesktopfile.h>
#include <kstandarddirs.h>
#include <kglobal.h>

#include "docentrytraverser.h"

#include "docmetainfo.h"

DocMetaInfo *DocMetaInfo::mSelf = 0;

DocMetaInfo *DocMetaInfo::self()
{
  if ( !mSelf ) mSelf = new DocMetaInfo;
  return mSelf;
}

DocMetaInfo::DocMetaInfo()
{
  kdDebug() << "DocMetaInfo()" << endl;

  mRootEntry.setName( "root entry" );
}

DocMetaInfo::~DocMetaInfo()
{
  kdDebug() << "~DocMetaInfo()" << endl;

  DocEntry::List::ConstIterator it;
  for( it = mDocEntries.begin(); it != mDocEntries.end(); ++it ) {
    delete *it;
  }

  mSelf = 0;
}

DocEntry *DocMetaInfo::addDocEntry( const QString &fileName )
{
  QFileInfo fi( fileName );
  if ( !fi.exists() ) return 0;
  
  DocEntry *entry = new DocEntry();

  QString extension = fi.extension();
  QStringList extensions = QStringList::split( '.', extension );
  QString lang;
  if ( extensions.count() >= 2 ) {
    lang = extensions[ extensions.count() - 2 ];
  }

  if ( !lang.isEmpty() && mLanguages.find( lang ) == mLanguages.end() ) {
    return 0;
  }

  if ( entry->readFromFile( fileName ) ) {
    if ( !lang.isEmpty() ) entry->setLang( lang );
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

void DocMetaInfo::scanMetaInfo( const QStringList &languages )
{
  mLanguages = languages;

  KStandardDirs* kstd = KGlobal::dirs();
  kstd->addResourceType( "data", "share/apps/khelpcenter" );
  QStringList list = kstd->findDirs( "data", "plugins" );
  for( QStringList::Iterator it=list.begin(); it!=list.end(); it++) {
    QDir pluginDir( *it );
    const QFileInfoList *entryList = pluginDir.entryInfoList();
    QFileInfoListIterator it( *entryList );
    QFileInfo *fi;
    for( ; ( fi = it.current() ); ++it ) {
      if ( fi->fileName().left( 1 ) != "." ) {
        scanMetaInfoDir( fi->absFilePath(), &mRootEntry );
      }
    }
  }
}

DocEntry *DocMetaInfo::scanMetaInfoDir( const QString &dirName,
                                        DocEntry *parent )
{
  QDir dir( dirName );

  DocEntry *dirEntry = addDocEntry( dirName + "/.directory" );

  if ( !dirEntry ) {
    dirEntry = new DocEntry;
    dirEntry->setName( dir.dirName() );
    addDocEntry( dirEntry );
  }

  dirEntry->setDirectory( true );

  if ( parent ) parent->addChild( dirEntry );

  const QFileInfoList *entryList = dir.entryInfoList();
  QFileInfoListIterator it( *entryList );
  QFileInfo *fi;
  for( ; ( fi = it.current() ); ++it ) {
    DocEntry *entry = 0;
    if ( fi->isDir() && fi->fileName() != "." && fi->fileName() != ".." ) {
      entry = scanMetaInfoDir( fi->absFilePath(), dirEntry );
    } else if ( fi->extension( false ) == "desktop" ) {
      entry = addDocEntry( fi->absFilePath() );
      if ( parent && entry ) dirEntry->addChild( entry );
    }
  }

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
    traverser->process( *it );
    if ( (*it)->hasChildren() ) {
      DocEntryTraverser *t = traverser->childTraverser( *it );
      traverseEntry( *it, t );
      t->deleteTraverser();
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
//  kdDebug() << "DocMetaInfo::startTraverseEntry()" << endl;

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
//  kdDebug() << "DocMetaInfo::endProcess()" << endl;

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
