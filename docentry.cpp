#include <QRegExp>
#include <QFileInfo>

#include <kdebug.h>
#include <kdesktopfile.h>
#include <kurl.h>
#include <kstandarddirs.h>
#include <kapplication.h>
#include <krandom.h>

#include "prefs.h"

#include "docentry.h"

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
  mParent = 0;
  mNextSibling = 0;
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

  if ( !docExists() ) return "unknown";

  if ( isDirectory() ) return "contents2";
  else return "document2";
}

void DocEntry::setUrl( const QString &url )
{
  mUrl = url;
}

QString DocEntry::url() const
{
  if ( !mUrl.isEmpty() ) return mUrl;

  if ( identifier().isEmpty() ) return QString();

  return "khelpcenter:" + identifier();
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

bool DocEntry::readFromFile( const QString &fileName )
{
  KDesktopFile file( fileName );

  mName = file.readName();
  mSearch = file.readEntry( "X-DOC-Search" );
  mIcon = file.readIcon();
  mUrl = file.readPathEntry( "DocPath" );
  mInfo = file.readEntry( "Info" );
  if ( mInfo.isNull() ) mInfo = file.readEntry( "Comment" );
  mLang = file.readEntry( "Lang", "en" );
  mIdentifier = file.readEntry( "X-DOC-Identifier" );
  if ( mIdentifier.isEmpty() ) {
    QFileInfo fi( fileName );
    mIdentifier = fi.completeBaseName();
  }
  mIndexer = file.readEntry( "X-DOC-Indexer" );
  mIndexer.replace( "%f", fileName );
  mIndexTestFile = file.readEntry( "X-DOC-IndexTestFile" );
  mSearchEnabledDefault = file.readEntry( "X-DOC-SearchEnabledDefault",
                                              false );
  mSearchEnabled = mSearchEnabledDefault;
  mWeight = file.readEntry( "X-DOC-Weight", 0 );
  mSearchMethod = file.readEntry( "X-DOC-SearchMethod" );
  mDocumentType = file.readEntry( "X-DOC-DocumentType" );

  mKhelpcenterSpecial = file.readEntry("X-KDE-KHelpcenter-Special");

  return true;
}

bool DocEntry::indexExists( const QString &indexDir )
{
  QString testFile;
  if ( mIndexTestFile.isEmpty() ) {
    testFile = identifier() + ".exists";
  } else {
    testFile = mIndexTestFile;
  }

  if ( !testFile.startsWith( "/" ) ) testFile = indexDir + "/" + testFile;

  return QFile::exists( testFile );
}

bool DocEntry::docExists() const
{
  if ( !mUrl.isEmpty() ) {
    KUrl docUrl( mUrl );
    if ( docUrl.isLocalFile() && !KStandardDirs::exists( docUrl.path() ) ) {
//      kDebug(1400) << "URL not found: " << docUrl.url() << endl;
      return false;
    }
  }

  return true;
}

void DocEntry::addChild( DocEntry *entry )
{
  entry->setParent( this );

  int i;
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
        mChildren.insert( mChildren.indexOf(mChildren.at( i + 1 )), entry );
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

bool DocEntry::isSearchable()
{
  return !search().isEmpty() && docExists() &&
    indexExists( Prefs::indexDirectory() );
}

void DocEntry::dump() const
{
  kDebug() << "  <docentry>" << endl;
  kDebug() << "    <name>" << mName << "</name>" << endl;
  kDebug() << "    <searchmethod>" << mSearchMethod << "</searchmethod>" << endl;
  kDebug() << "    <search>" << mSearch << "</search>" << endl;
  kDebug() << "    <indexer>" << mIndexer << "</indexer>" << endl;
  kDebug() << "    <indextestfile>" << mIndexTestFile << "</indextestfile>" << endl;
  kDebug() << "    <icon>" << mIcon << "</icon>" << endl;
  kDebug() << "    <url>" << mUrl << "</url>" << endl;
  kDebug() << "    <documenttype>" << mDocumentType << "</documenttype>" << endl; 
  kDebug() << "  </docentry>" << endl;
}
// vim:ts=2:sw=2:et
