#include <qfile.h>

#include <kdebug.h>
#include <kdesktopfile.h>

#include "docmetainfo.h"

DocEntry::DocEntry()
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

void DocEntry::enableSearch( bool enabled )
{
  mSearchEnabled = enabled;
}

bool DocEntry::searchEnabled() const
{
  return mSearchEnabled;
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
  mSearchEnabled = file.readBoolEntry( "X-DOC-SearchEnabledDefault", false );

  return true;
}

void DocEntry::dump() const
{
  kdDebug() << "  <docentry>" << endl;
  kdDebug() << "    <name>" << mName << "</name>" << endl;
  kdDebug() << "    <search>" << mSearch << "</search>" << endl;
  kdDebug() << "    <icon>" << mIcon << "</icon>" << endl;
  kdDebug() << "    <url>" << mUrl << "</url>" << endl;
  kdDebug() << "  </docentry>" << endl;
}


DocMetaInfo *DocMetaInfo::mSelf = 0;

DocMetaInfo *DocMetaInfo::self()
{
  if ( !mSelf ) mSelf = new DocMetaInfo;
  return mSelf;
}

DocMetaInfo::DocMetaInfo()
{
}

DocMetaInfo::~DocMetaInfo()
{
  DocEntry::List::ConstIterator it;
  for( it = mDocEntries.begin(); it != mDocEntries.end(); ++it ) {
    delete *it;
  }

  mSelf = 0;
}

DocEntry *DocMetaInfo::addDocEntry( const QString &fileName )
{
  if ( !QFile::exists( fileName ) ) {
    return 0;
  }

  DocEntry *entry = new DocEntry();

  if ( entry->readFromFile( fileName ) ) {
    mDocEntries.append( entry );
    if ( !entry->search().isEmpty() ) mSearchEntries.append( entry );
    return entry;
  } else {
    delete entry;
    return 0;
  }
}

DocEntry::List DocMetaInfo::docEntries()
{
  return mDocEntries;
}

DocEntry::List DocMetaInfo::searchEntries()
{
  return mSearchEntries;
}
