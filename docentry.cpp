#include <qfile.h>
#include <qregexp.h>
#include <qfileinfo.h>

#include <kdebug.h>
#include <kdesktopfile.h>

#include "docentry.h"

using namespace KHC;

DocEntry::DocEntry() :
  mWeight( 0 ), mSearchEnabled( false ), mDirectory( false ), mParent( 0 ),
  mNextSibling( 0 )
{
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
  return mIcon;
}

void DocEntry::setUrl( const QString &url )
{
  mUrl = url;
}

QString DocEntry::url() const
{
  return mUrl;
}

void DocEntry::setDocPath( const QString &docPath )
{
  mDocPath = docPath;
}

QString DocEntry::docPath() const
{
  return mDocPath;
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

void DocEntry::setDirectory( bool dir )
{
  mDirectory = dir;
}

bool DocEntry::isDirectory() const
{
  return mDirectory;
}

bool DocEntry::readFromFile( const QString &fileName )
{
  KDesktopFile file( fileName );

  mName = file.readName();
  mSearch = file.readEntry( "X-DOC-Search" );
  mIcon = file.readIcon();
  mUrl = file.readURL();
  mDocPath = file.readEntry( "DocPath" );
  mInfo = file.readEntry( "Info" );
  if ( mInfo.isNull() ) mInfo = file.readEntry( "Comment" );
  mLang = file.readEntry( "Lang" );
  mIdentifier = file.readEntry( "X-DOC-Identifier" );
  if ( mIdentifier.isEmpty() ) {
    QFileInfo fi( fileName );
    mIdentifier = fi.baseName( true );
  }
  mIndexer = file.readEntry( "X-DOC-Indexer" );
  mIndexer.replace( QRegExp( "%f" ) , fileName );
  mIndexTestFile = file.readEntry( "X-DOC-IndexTestFile" );
  mSearchEnabled = file.readBoolEntry( "X-DOC-SearchEnabledDefault", false );
  mWeight = file.readNumEntry( "X-DOC-Weight", 0 );
  mSearchMethod = file.readEntry( "X-DOC-SearchMethod" );

  mKhelpcenterSpecial = file.readEntry("X-KDE-KHelpcenter-Special");

  return true;
}

bool DocEntry::indexExists()
{
  if ( mIndexTestFile.isEmpty() ) return true;
  
  return QFile::exists( mIndexTestFile );
}

void DocEntry::addChild( DocEntry *entry )
{
  entry->setParent( this );

  uint i;
  for( i = 0; i < mChildren.count(); ++i ) {
    if ( i == 0 ) {
      if ( entry->weight() < mChildren.first()->weight() ) {
        entry->setNextSibling( mChildren.first() );
        mChildren.prepend( entry );        
        break;
      }
    }
    if ( i + 1 < mChildren.count() ) {
      if ( entry->weight() >= mChildren[ i ]->weight() &&
           entry->weight() < mChildren[ i + 1 ]->weight() ) {
        entry->setNextSibling( mChildren[ i + 1 ] );
        mChildren[ i ]->setNextSibling( entry );
        mChildren.insert( mChildren.at( i + 1 ), entry );
        break;
      }
    }
  }
  if ( i == mChildren.count() ) {
    if ( i > 0 ) {
      mChildren.last()->setNextSibling( entry );
    }
    mChildren.append( entry );
  }
}

bool DocEntry::hasChildren()
{
  return mChildren.count();
}

DocEntry *DocEntry::firstChild()
{
  return mChildren.first();
}

DocEntry::List DocEntry::children()
{
  return mChildren;
}

void DocEntry::setParent( DocEntry *parent )
{
  mParent = parent;
}

DocEntry *DocEntry::parent()
{
  return mParent;
}

void DocEntry::setNextSibling( DocEntry *next )
{
  mNextSibling = next;
}

DocEntry *DocEntry::nextSibling()
{
  return mNextSibling;
}

void DocEntry::dump() const
{
  kdDebug() << "  <docentry>" << endl;
  kdDebug() << "    <name>" << mName << "</name>" << endl;
  kdDebug() << "    <searchmethod>" << mSearchMethod << "</searchmethod>" << endl;
  kdDebug() << "    <search>" << mSearch << "</search>" << endl;
  kdDebug() << "    <indexer>" << mIndexer << "</indexer>" << endl;
  kdDebug() << "    <indextestfile>" << mIndexTestFile << "</indextestfile>" << endl;
  kdDebug() << "    <icon>" << mIcon << "</icon>" << endl;
  kdDebug() << "    <url>" << mUrl << "</url>" << endl;
  kdDebug() << "    <docpath>" << mDocPath << "</docpath>" << endl;
  kdDebug() << "  </docentry>" << endl;
}
// vim:ts=2:sw=2:et
