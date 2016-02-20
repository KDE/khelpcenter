
/* This file is part of the KDE project
 * Copyright 2002 Cornelius Schumacher <schumacher@kde.org>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License version 2 or at your option version 3 as published
 * by the Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "docmetainfo.h"

#include "khc_debug.h"

#include <QRegExp>

#include <KConfig>
#include <KConfigGroup>

#include "docentrytraverser.h"
#include <KLocalizedString>

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
  khcDebug() << "DocMetaInfo()";

  mRootEntry.setName( i18n("Top-Level Documentation") );
}

DocMetaInfo::~DocMetaInfo()
{
  khcDebug() << "~DocMetaInfo()";

  DocEntry::List::ConstIterator it;
  for( it = mDocEntries.constBegin(); it != mDocEntries.constEnd(); ++it )
  {
    delete *it;
  }

  mLoaded = false;

  mSelf = 0;
}

DocEntry *DocMetaInfo::addDocEntry( const QString &fileName )
{
  QFileInfo fi( fileName );
  if ( !fi.exists() ) return 0;

  QString extension = fi.completeSuffix();
  QStringList extensions = extension.split( '.');
  QString lang;
  if ( extensions.count() >= 2 )
  {
    lang = extensions[ extensions.count() - 2 ];
  }

  if ( !lang.isEmpty() && !mLanguageNames.contains( lang ) )
  {
    return 0;
  }

  DocEntry *entry = new DocEntry();

  if ( entry->readFromFile( fileName ) ) 
  {
    if ( !lang.isEmpty() && lang != mDefaultLanguage )
    {
      entry->setLang( lang );
      entry->setName( i18nc("doctitle (language)","%1 (%2)",
                             entry->name() ,
                             mLanguageNames[ lang ] ) );
    }
    QString indexer = entry->indexer();
    indexer.replace( "%f", fileName );
    entry->setIndexer( indexer );
    addDocEntry( entry );
    return entry;
  } 
  else 
  {
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
  if ( langcode == "en" )
      return i18nc("Describes documentation entries that are in English", "English");

  QString cfgfile = QStandardPaths::locate( QStandardPaths::GenericDataLocation, QStringLiteral( "locale/%1/kf5_entry.desktop" ).arg( langcode ) );

  khcDebug() << "-- langcode: " << langcode << " cfgfile: " << cfgfile;

  KConfig _cfg( cfgfile, KConfig::SimpleConfig );
  KConfigGroup cfg(&_cfg, "KCM Locale" );
  QString name = cfg.readEntry( "Name", langcode );

  return name;
}

void DocMetaInfo::scanMetaInfo( bool force )
{
  if ( mLoaded && !force ) return;

  QStringList langs = QLocale().uiLanguages();
  mDefaultLanguage = langs.first();

  QStringList::ConstIterator it;
  for( it = langs.constBegin(); it != langs.constEnd(); ++it )
  {
    mLanguageNames.insert( *it, languageName( *it ) );
  }

  KConfig config( QLatin1String("khelpcenterrc") );
  KConfigGroup cg(&config, "General");
  QStringList metaInfos = cg.readEntry( "MetaInfoDirs" , QStringList() );

  if ( metaInfos.isEmpty() ) 
  {
    metaInfos = QStandardPaths::locateAll(QStandardPaths::DataLocation, "plugins", QStandardPaths::LocateDirectory);
  }
  for( it = metaInfos.constBegin(); it != metaInfos.constEnd(); ++it)
  {
    khcDebug() << "DocMetaInfo::scanMetaInfo(): scanning " << *it;
    scanMetaInfoDir( *it, &mRootEntry );
  }

  mLoaded = true;
}

DocEntry *DocMetaInfo::scanMetaInfoDir( const QString &dirName,
                                        DocEntry *parent )
{
  QDir dir( dirName );
  if ( !dir.exists() ) return 0;

  foreach( const QFileInfo &fi, dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot) ) 
  {
    DocEntry *entry = 0;
    if ( fi.isDir() ) 
    {
      DocEntry *dirEntry = addDirEntry( QDir( fi.absoluteFilePath() ), parent );
      entry = scanMetaInfoDir( fi.absoluteFilePath(), dirEntry );
    } 
    else if ( fi.suffix() == QLatin1String("desktop") )
    {
      entry = addDocEntry( fi.absoluteFilePath() );
      if ( parent && entry ) parent->addChild( entry );
    }
  }

  return 0;
}

DocEntry *DocMetaInfo::addDirEntry( const QDir &dir, DocEntry *parent )
{
  DocEntry *dirEntry = addDocEntry( dir.absolutePath() + QLatin1String("/.directory") );

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
  khcDebug() << "DocMetaInfo::startTraverseEntries()";
  traverser->setNotifyee( this );
  startTraverseEntry( &mRootEntry, traverser );
}

void DocMetaInfo::startTraverseEntry( DocEntry *entry,
                                      DocEntryTraverser *traverser )
{
  if ( !traverser ) 
  {
    khcDebug() << "DocMetaInfo::startTraverseEntry(): ERROR. No Traverser."
              << endl;
    return;
  }

  if ( !entry ) 
  {
    khcDebug() << "DocMetaInfo::startTraverseEntry(): no entry.";
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
    DocEntryTraverser *parentTraverser = 0;
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
  khcDebug() << "DocMetaInfo::endTraverseEntries()";

  if ( !traverser ) 
  {
    khcDebug() << " no more traversers.";
    return;
  }

  traverser->finishTraversal();
}
// vim:ts=2:sw=2:et
