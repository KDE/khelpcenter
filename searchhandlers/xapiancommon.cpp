/*
    SPDX-FileCopyrightText: 2016 Pino Toscano <pino@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "xapiancommon.h"

#include <QByteArray>
#include <QDir>
#include <QLoggingCategory>

namespace {

Q_LOGGING_CATEGORY( LOG, "org.kde.khelpcenter.xapian.common", QtWarningMsg )
const std::string versionKey = "khc-db-version";

}

enum {
  KHC_DB_VERSION = 1
};

DatabaseVersionMismatch::DatabaseVersionMismatch( int aversion )
  : version( aversion )
  , refVersion( KHC_DB_VERSION )
{
}

static int getDatabaseVersion( const Xapian::Database& db )
{
  const std::string value = db.get_metadata( versionKey );
  return QByteArray::fromRawData( value.c_str(), value.size() ).toInt();
}

static Xapian::WritableDatabase openWritableDbHelper( const QString& path, bool checkVersion )
{
  Xapian::WritableDatabase db;
  try {
    Xapian::WritableDatabase newDb = Xapian::WritableDatabase( QFile::encodeName( path ).constData(), Xapian::DB_CREATE_OR_OPEN );
    if ( checkVersion && newDb.get_doccount() > 0 ) {
      const int version = getDatabaseVersion( newDb );
      if ( KHC_DB_VERSION != version ) {
        throw DatabaseVersionMismatch( version );
      }
    }
    newDb.set_metadata( versionKey, QByteArray::number( KHC_DB_VERSION ).constData() );
    db = newDb;
  } catch ( const Xapian::DatabaseCorruptError& e ) {
    qCWarning(LOG) << "Xapian DB corrupted, throwing it away";
    QDir( path ).removeRecursively();
    return openWritableDbHelper( path, false );
  } catch ( const Xapian::DatabaseVersionError& e ) {
    qCWarning(LOG) << "Xapian DB version mismatch, throwing it away";
    QDir( path ).removeRecursively();
    return openWritableDbHelper( path, false );
  } catch ( const DatabaseVersionMismatch& e ) {
    qCWarning(LOG) << "Own version mismatch in Xapian DB: found" << e.version << "vs wanted" << e.refVersion << "- throwing it away";
    QDir( path ).removeRecursively();
    return openWritableDbHelper( path, false );
  }
  return db;
}

Xapian::WritableDatabase openWritableDb( const QString& path )
{
  return openWritableDbHelper( path, true );
}

Xapian::Database openDb( const QString& path )
{
  Xapian::Database db( QFile::encodeName( path ).constData() );
  const int version = getDatabaseVersion( db );
  if ( KHC_DB_VERSION != version ) {
    throw DatabaseVersionMismatch( version );
  }
  return db;
}

void getDocInfo( const Xapian::Document& doc, std::string* lang, std::string* uid, std::string* xhtml )
{
  for ( Xapian::TermIterator it = doc.termlist_begin(); it != doc.termlist_end(); ++it ) {
    const std::string term = *it;
    if ( term.empty() ) {
      continue;
    }
    switch ( term[0] ) {
    case 'L':
      if ( lang ) {
        lang->assign( term.begin() + 1, term.end() );
      }
      break;
    case 'U':
      if ( uid ) {
        uid->assign( term.begin() + 1, term.end() );
      }
      break;
    case 'X':
      if ( xhtml && term.size() > 5 && term.compare( 0, 5, "XHTML") == 0 ) {
        xhtml->assign( term.begin() + 5, term.end() );
      }
      break;
    }
  }
}

QDebug operator<<( QDebug dbg, const std::string& s )
{
  return dbg << s.c_str();
}
