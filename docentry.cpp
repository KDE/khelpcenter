/*
    SPDX-FileCopyrightText: 2002 Cornelius Schumacher <schumacher@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "docentry.h"

#include "khc_debug.h"

#include <QUrl>

#include <KConfigGroup>
#include <KDesktopFile>
#include <KRandom>

using namespace KHC;

DocEntry::DocEntry()
{
  init();
}

DocEntry::DocEntry( const QString &name, const QString &url,
                    const QString &icon )
{
  init();

  mName = name;
  mUrl = url;
  mIcon = icon;
}

void DocEntry::init()
{
  mWeight = 0;
  mSearchEnabled = false;
  mDirectory = false;
  mParent = nullptr;
  mNextSibling = nullptr;
}

void DocEntry::setName( const QString &name )
{
  mName = name;
}

QString DocEntry::name() const
{
  return mName;
}

void DocEntry::setSearch( const QString &search )
{
  mSearch = search;
}

QString DocEntry::search() const
{
  return mSearch;
}

void DocEntry::setIcon( const QString &icon )
{
  mIcon = icon;
}

QString DocEntry::icon() const
{
  if ( !mIcon.isEmpty() ) return mIcon;
  if ( !docExists() ) return QStringLiteral("unknown");
  if ( isDirectory() ) return QStringLiteral("help-contents");
  
  else return QStringLiteral("text-plain");
}

void DocEntry::setUrl( const QString &url )
{
  mUrl = url;
}

QString DocEntry::url() const
{
  if ( !mUrl.isEmpty() ) return mUrl;
  if ( identifier().isEmpty() ) return QString();

  return QStringLiteral("khelpcenter:") + identifier();
}

void DocEntry::setInfo( const QString &info )
{
  mInfo = info;
}

QString DocEntry::info() const
{
  return mInfo;
}

void DocEntry::setLang( const QString &lang )
{
  mLang = lang;
}

QString DocEntry::lang() const
{
  return mLang;
}

void DocEntry::setIdentifier( const QString &identifier )
{
  mIdentifier = identifier;
}

QString DocEntry::identifier() const
{
  if ( mIdentifier.isEmpty() ) mIdentifier = KRandom::randomString( 15 );
  return mIdentifier;
}

void DocEntry::setIndexer( const QString &indexer )
{
  mIndexer = indexer;
}

QString DocEntry::indexer() const
{
  return mIndexer;
}

void DocEntry::setIndexTestFile( const QString &indexTestFile )
{
  mIndexTestFile = indexTestFile;
}

QString DocEntry::indexTestFile() const
{
  return mIndexTestFile;
}

void DocEntry::setWeight( int weight )
{
  mWeight = weight;
}

int DocEntry::weight() const
{
  return mWeight;
}

void DocEntry::setSearchMethod( const QString &method )
{
  mSearchMethod = method;
}

QString DocEntry::searchMethod() const
{
  return mSearchMethod;
}

void DocEntry::setDocumentType( const QString &str )
{
  mDocumentType = str;
}

QString DocEntry::documentType() const
{
  return mDocumentType;
}

QString DocEntry::khelpcenterSpecial() const
{
  return mKhelpcenterSpecial;
}

void DocEntry::enableSearch( bool enabled )
{
  mSearchEnabled = enabled;
}

bool DocEntry::searchEnabled() const
{
  return mSearchEnabled;
}

void DocEntry::setSearchEnabledDefault( bool enabled )
{
  mSearchEnabledDefault = enabled;
}

bool DocEntry::searchEnabledDefault() const
{
  return mSearchEnabledDefault;
}

void DocEntry::setDirectory( bool dir )
{
  mDirectory = dir;
}

bool DocEntry::isDirectory() const
{
  return mDirectory;
}

bool DocEntry::readFromFile( const QFileInfo &fileInfo )
{
  KDesktopFile file( fileInfo.absoluteFilePath() );
  KConfigGroup desktopGroup = file.desktopGroup();

  mName = file.readName();
  mSearch = desktopGroup.readEntry( "X-DOC-Search" );
  mIcon = file.readIcon();
  mUrl = file.readDocPath();
  mInfo = desktopGroup.readEntry( "Info" );
  if ( mInfo.isNull() ) 
  {
    mInfo = file.readComment();
  }
  mLang = desktopGroup.readEntry( "Lang", "en" );
  mIdentifier = desktopGroup.readEntry( "X-DOC-Identifier" );
  if ( mIdentifier.isEmpty() ) 
  {
    mIdentifier = fileInfo.completeBaseName();
  }
  mIndexer = desktopGroup.readEntry( "X-DOC-Indexer" );
  mIndexer.replace( QStringLiteral("%f"), fileInfo.absoluteFilePath() );
  mIndexTestFile = desktopGroup.readEntry( "X-DOC-IndexTestFile" );
  mSearchEnabledDefault = desktopGroup.readEntry( "X-DOC-SearchEnabledDefault",
                                              false );
  mSearchEnabled = mSearchEnabledDefault;
  mWeight = desktopGroup.readEntry( "X-DOC-Weight", 0 );
  mSearchMethod = desktopGroup.readEntry( "X-DOC-SearchMethod" );
  mDocumentType = desktopGroup.readEntry( "X-DOC-DocumentType" );

  mKhelpcenterSpecial = desktopGroup.readEntry("X-KDE-KHelpcenter-Special");

  return true;
}

bool DocEntry::docExists() const
{
  if ( !mUrl.isEmpty() ) 
  {
    QUrl docUrl( mUrl );
    if ( docUrl.isLocalFile() && !QFile::exists( docUrl.toLocalFile() ) )
    {
      return false;
    }
  }

  return true;
}

void DocEntry::addChild( DocEntry *entry )
{
  entry->setParent( this );

  int i;
  for( i = 0; i < mChildren.count(); ++i ) 
  {
    if ( i == 0 ) {
      if ( entry->weight() < mChildren.first()->weight() ) 
      {
        entry->setNextSibling( mChildren.first() );
        mChildren.prepend( entry );
        break;
      }
    }
    if ( i + 1 < mChildren.count() ) 
    {
      if ( entry->weight() >= mChildren[ i ]->weight() &&
           entry->weight() < mChildren[ i + 1 ]->weight() ) 
      {
        entry->setNextSibling( mChildren[ i + 1 ] );
        mChildren[ i ]->setNextSibling( entry );
        mChildren.insert( mChildren.indexOf(mChildren.at( i + 1 )), entry );
        break;
      }
    }
  }
  if ( i == mChildren.count() )
  {
    if ( i > 0 ) 
    {
      mChildren.last()->setNextSibling( entry );
    }
    mChildren.append( entry );
  }
}

bool DocEntry::hasChildren() const
{
  return mChildren.count();
}

DocEntry *DocEntry::firstChild() const
{
  return mChildren.first();
}

DocEntry::List DocEntry::children() const
{
  return mChildren;
}

void DocEntry::setParent( DocEntry *parent )
{
  mParent = parent;
}

DocEntry *DocEntry::parent() const
{
  return mParent;
}

void DocEntry::setNextSibling( DocEntry *next )
{
  mNextSibling = next;
}

DocEntry *DocEntry::nextSibling() const
{
  return mNextSibling;
}

bool DocEntry::isSearchable() const
{
  return !search().isEmpty() && docExists();
}

void DocEntry::dump() const
{
  qCDebug(KHC_LOG) << "  <docentry>";
  qCDebug(KHC_LOG) << "    <name>" << mName << "</name>";
  qCDebug(KHC_LOG) << "    <searchmethod>" << mSearchMethod << "</searchmethod>";
  qCDebug(KHC_LOG) << "    <search>" << mSearch << "</search>";
  qCDebug(KHC_LOG) << "    <indexer>" << mIndexer << "</indexer>";
  qCDebug(KHC_LOG) << "    <indextestfile>" << mIndexTestFile << "</indextestfile>";
  qCDebug(KHC_LOG) << "    <icon>" << mIcon << "</icon>";
  qCDebug(KHC_LOG) << "    <url>" << mUrl << "</url>";
  qCDebug(KHC_LOG) << "    <documenttype>" << mDocumentType << "</documenttype>";
  qCDebug(KHC_LOG) << "  </docentry>";
}
// vim:ts=2:sw=2:et
