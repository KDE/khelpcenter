/*
    SPDX-FileCopyrightText: 2002 Cornelius Schumacher <schumacher@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only
*/

#include "docmetainfo.h"

#include "khc_debug.h"

#include <KConfig>
#include <KConfigGroup>
#include <KLocalizedString>

#include "docentrytraverser.h"

#include <prefs.h>

using namespace KHC;

bool DocMetaInfo::mLoaded = false;

DocMetaInfo *DocMetaInfo::mSelf = nullptr;

DocMetaInfo *DocMetaInfo::self()
{
  if ( !mSelf ) mSelf = new DocMetaInfo;
  return mSelf;
}

DocMetaInfo::DocMetaInfo()
{
  qCDebug(KHC_LOG) << "DocMetaInfo()";

  mRootEntry.setName( i18n("Top-Level Documentation") );
}

DocMetaInfo::~DocMetaInfo()
{
  qCDebug(KHC_LOG) << "~DocMetaInfo()";

  DocEntry::List::ConstIterator it;
  for( it = mDocEntries.constBegin(); it != mDocEntries.constEnd(); ++it )
  {
    delete *it;
  }

  mLoaded = false;

  mSelf = nullptr;
}

DocEntry *DocMetaInfo::addDocEntry( const QFileInfo &fi )
{
  if ( !fi.exists() ) return nullptr;

  QString extension = fi.completeSuffix();
  QStringList extensions = extension.split( QLatin1Char('.'));
  QString lang;
  if ( extensions.count() >= 2 )
  {
    lang = extensions[ extensions.count() - 2 ];
  }

  if ( !lang.isEmpty() && !mLanguages.contains( lang ) )
  {
    return nullptr;
  }

  DocEntry *entry = new DocEntry();

  if ( entry->readFromFile( fi ) )
  {
    if ( !lang.isEmpty() && lang != mLanguages.first() )
    {
      QMap<QString,QString>::Iterator it = mLanguageNames.find( lang );
      if ( it == mLanguageNames.end() )
      {
        it = mLanguageNames.insert( lang, languageName( lang ) );
      }

      entry->setLang( lang );
      entry->setName( i18nc("doctitle (language)","%1 (%2)",
                             entry->name() ,
                             *it ) );
    }
    QString indexer = entry->indexer();
    indexer.replace( QStringLiteral("%f"), fi.absoluteFilePath() );
    entry->setIndexer( indexer );
    addDocEntry( entry );
    return entry;
  } 
  else 
  {
    delete entry;
    return nullptr;
  }
}

void DocMetaInfo::addDocEntry( DocEntry *entry )
{
  mDocEntries.append( entry );
  if ( !entry->search().isEmpty() ) mSearchEntries.append( entry );
}

DocEntry::List DocMetaInfo::docEntries() const
{
  return mDocEntries;
}

DocEntry::List DocMetaInfo::searchEntries() const
{
  return mSearchEntries;
}

QString DocMetaInfo::languageName( const QString &langcode )
{
  if ( langcode == QLatin1String("en") )
      return i18nc("Describes documentation entries that are in English", "English");

  const QString cfgfile = QStandardPaths::locate( QStandardPaths::GenericDataLocation, QStringLiteral( "locale/%1/kf5_entry.desktop" ).arg( langcode ) );

  qCDebug(KHC_LOG) << "-- langcode: " << langcode << " cfgfile: " << cfgfile;

  KConfig _cfg( cfgfile, KConfig::SimpleConfig );
  KConfigGroup cfg(&_cfg, "KCM Locale" );
  const QString name = cfg.readEntry( "Name", langcode );

  return name;
}

void DocMetaInfo::scanMetaInfo( bool force )
{
  if ( mLoaded && !force ) return;

  mLanguages = KLocalizedString::languages();

  QStringList::ConstIterator it;

  QStringList metaInfos = Prefs::metaInfoDirs();

  if ( metaInfos.isEmpty() ) 
  {
    metaInfos = QStandardPaths::locateAll(QStandardPaths::AppDataLocation, QStringLiteral("plugins"), QStandardPaths::LocateDirectory);
  }
  for( it = metaInfos.constBegin(); it != metaInfos.constEnd(); ++it)
  {
    qCDebug(KHC_LOG) << "DocMetaInfo::scanMetaInfo(): scanning " << *it;
    scanMetaInfoDir( *it, &mRootEntry );
  }

  mLoaded = true;
}

DocEntry *DocMetaInfo::scanMetaInfoDir( const QString &dirName,
                                        DocEntry *parent )
{
  QDir dir( dirName );
  if ( !dir.exists() ) return nullptr;

  const auto entryInfoList = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
  for ( const QFileInfo &fi : entryInfoList )
  {
    DocEntry *entry = nullptr;
    if ( fi.isDir() ) 
    {
      DocEntry *dirEntry = addDirEntry( QDir( fi.absoluteFilePath() ), parent );
      entry = scanMetaInfoDir( fi.absoluteFilePath(), dirEntry );
    } 
    else if ( fi.suffix() == QLatin1String("desktop") )
    {
      entry = addDocEntry( fi );
      if ( parent && entry ) parent->addChild( entry );
    }
  }

  return nullptr;
}

DocEntry *DocMetaInfo::addDirEntry( const QDir &dir, DocEntry *parent )
{
  DocEntry *dirEntry = addDocEntry( dir.absoluteFilePath( QStringLiteral(".directory") ) );

  if ( !dirEntry ) 
  {
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
  for( it = children.constBegin(); it != children.constEnd(); ++it )
  {
    if ( (*it)->isDirectory() && !(*it)->hasChildren() &&
         (*it)->khelpcenterSpecial().isEmpty() ) continue;
    traverser->process( *it );
    if ( (*it)->hasChildren() ) 
    {
      DocEntryTraverser *t = traverser->childTraverser( *it );
      if (t)
      {
        traverseEntry( *it, t );
        t->deleteTraverser();
      }
    }
  }
}

void DocMetaInfo::startTraverseEntries( DocEntryTraverser *traverser )
{
  qCDebug(KHC_LOG) << "DocMetaInfo::startTraverseEntries()";
  traverser->setNotifyee( this );
  startTraverseEntry( &mRootEntry, traverser );
}

void DocMetaInfo::startTraverseEntry( DocEntry *entry,
                                      DocEntryTraverser *traverser )
{
  if ( !traverser ) 
  {
    qCWarning(KHC_LOG) << "DocMetaInfo::startTraverseEntry(): ERROR. No Traverser.";
    return;
  }

  if ( !entry ) 
  {
    qCWarning(KHC_LOG) << "DocMetaInfo::startTraverseEntry(): no entry.";
    endTraverseEntries( traverser );
    return;
  }

  traverser->startProcess( entry );
}

void DocMetaInfo::endProcess( DocEntry *entry, DocEntryTraverser *traverser )
{
  if ( !entry ) 
  {
    endTraverseEntries( traverser );
    return;
  }

  if ( entry->hasChildren() ) 
  {
    startTraverseEntry( entry->firstChild(), traverser->childTraverser( entry ) );
  } else if ( entry->nextSibling() ) 
  {
    startTraverseEntry( entry->nextSibling(), traverser );
  } else 
  {
    DocEntry *parent = entry->parent();
    DocEntryTraverser *parentTraverser = nullptr;
    while ( parent ) {
      parentTraverser = traverser->parentTraverser();
      traverser->deleteTraverser();
      if ( parent->nextSibling() ) {
        startTraverseEntry( parent->nextSibling(), parentTraverser );
        break;
      } 
      else 
      {
        parent = parent->parent();
        traverser = parentTraverser;
      }
    }
    if ( !parent )
    {
      endTraverseEntries( traverser );
    }
  }
}

void DocMetaInfo::endTraverseEntries( DocEntryTraverser *traverser )
{
  qCDebug(KHC_LOG) << "DocMetaInfo::endTraverseEntries()";

  if ( !traverser ) 
  {
    qCDebug(KHC_LOG) << " no more traversers.";
    return;
  }

  traverser->finishTraversal();
}
// vim:ts=2:sw=2:et
