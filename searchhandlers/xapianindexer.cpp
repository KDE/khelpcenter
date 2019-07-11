/*
  This file is part of the KDE Help Center

  Copyright (c) 2016 Pino Toscano <pino@kde.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA  02110-1301, USA
*/

#include "cachereader.h"
#include "htmltextdump.h"
#include "xapiancommon.h"

#include <docbookxslt.h>
#include <QDateTime>
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QDirIterator>
#include <QLoggingCategory>
#include <QStandardPaths>

namespace {

Q_LOGGING_CATEGORY( LOG, "org.kde.khelpcenter.xapian.indexer", QtWarningMsg )

}

static Xapian::Document createDocument( Xapian::TermGenerator& xgen, const std::string& uid, const std::string& lang, const std::string& modTime, const QString& html, const QByteArray& data )
{
  Xapian::Document doc;
  xgen.set_document( doc );

  doc.add_boolean_term( uid );
  doc.add_boolean_term( "Ttext/html" );
  doc.add_boolean_term( "L" + lang );
  doc.add_boolean_term( "XHTML" + html.toStdString() );

  doc.add_value( VALUE_LASTMOD, modTime );

  QByteArray title;
  QByteArray text;
  if ( htmlTextDump( data, &title, &text ) ) {
    if ( !title.isEmpty() ) {
      doc.add_value( VALUE_TITLE, title.constData() );
    }

    xgen.index_text( Xapian::Utf8Iterator( text.constData(), text.length() ) );
  }

  return doc;
}

static void analyzeFile( const QFileInfo& cacheFile, const QString& relPath, const std::string& lang, Xapian::WritableDatabase& db, Xapian::TermGenerator& xgen, QSet<Xapian::docid>* handledDocuments )
{
  const std::string uid = "U" + relPath.toStdString();
  const qint64 modTime = cacheFile.lastModified().toMSecsSinceEpoch();
  const std::string std_modTime = QByteArray::number( modTime ).toStdString();

  CacheReader cr;
  if ( !cr.parse( cacheFile.absoluteFilePath() ) ) {
    qCWarning(LOG) << "cannot parse cache file" << cacheFile.absoluteFilePath();
    return;
  }

  const QSet<QString> docs = cr.documents();
  qCDebug(LOG) << cacheFile.absoluteFilePath() << "=>" << docs;

  QSet<QString> docsToAdd = docs;
  QHash<QString, Xapian::docid> docsToUpdate;
  QSet<Xapian::docid> docsToRemove;

  try {
    for ( Xapian::PostingIterator it = db.postlist_begin( uid ); it != db.postlist_end( uid ); ++it ) {
      Xapian::Document doc = db.get_document( *it );
      std::string doc_lang;
      std::string html;
      getDocInfo( doc, &doc_lang, nullptr, &html );
      // skip versions of this cache in different languages
      if ( doc_lang.empty() || doc_lang != lang ) {
        continue;
      }

      const QString id = QString::fromStdString( html );

      if ( docs.constFind( id ) == docs.constEnd() ) {
        // this HTML document does not exist anymore => mark for deletion
        docsToRemove.insert( *it );
        continue;
      }

      // found, no need to add it from scratch
      docsToAdd.remove( id );
      handledDocuments->insert( *it );

      const qint64 old_last_mod = QByteArray::fromStdString( doc.get_value( VALUE_LASTMOD ).c_str() ).toLongLong();
      if ( modTime > old_last_mod ) {
        docsToUpdate.insert( id, *it );
      }
    }

    qCDebug(LOG) << "  docs to remove:" << docsToRemove;
    qCDebug(LOG) << "  docs to add:" << docsToAdd;
    qCDebug(LOG) << "  docs to update:" << docsToUpdate.keys();

    Q_FOREACH ( Xapian::docid id, docsToRemove ) {
      db.delete_document( id );
    }

    const QHash<QString, Xapian::docid>::ConstIterator dtuItEnd = docsToUpdate.constEnd();
    QHash<QString, Xapian::docid>::ConstIterator dtuIt = docsToUpdate.constBegin();
    for ( ; dtuIt != dtuItEnd; ++dtuIt ) {
      const QString name = dtuIt.key();
      const Xapian::Document doc = createDocument( xgen, uid, lang, std_modTime, name, cr.document( name ) );
      db.replace_document( dtuIt.value(), doc );
    }

    Q_FOREACH ( const QString& name, docsToAdd ) {
      const Xapian::Document doc = createDocument( xgen, uid, lang, std_modTime, name, cr.document( name ) );
      const Xapian::docid id = db.add_document( doc );
      handledDocuments->insert( id );
    }

  } catch ( const Xapian::Error& e ) {
    qCCritical(LOG) << "Xapian error (index):" << e.get_description();
  }

}

static QString relativePath( const QString& base, const QString& full )
{
  Q_ASSERT( full.startsWith( base ) );
  int length = base.length();
  if ( full.at( length ) == QLatin1Char( '/' ) ) {
    ++length;
  }
  return full.mid( length );
}

static void walkFiles( const QString& directory, const std::string& lang, Xapian::WritableDatabase& db, Xapian::TermGenerator& xgen, QSet<Xapian::docid>* handledDocuments )
{
  QDirIterator it( directory, QStringList() << QStringLiteral("*.cache.bz2"), QDir::NoDotAndDotDot | QDir::Files, QDirIterator::Subdirectories );
  while ( it.hasNext() ) {
    const QString f = it.next();
    analyzeFile( it.fileInfo(), relativePath( directory, f ), lang, db, xgen, handledDocuments );
  }
}

static Xapian::Stem stemmerForLanguage( const QString& lang )
{
  const QHash<QString, QString> stemmers = QHash<QString, QString>( {
    { QStringLiteral( "da" ), QStringLiteral( "danish" ) },
    { QStringLiteral( "de" ), QStringLiteral( "german" ) },
    { QStringLiteral( "en" ), QStringLiteral( "english" ) },
    { QStringLiteral( "es" ), QStringLiteral( "spanish" ) },
    { QStringLiteral( "fi" ), QStringLiteral( "finnish" ) },
    { QStringLiteral( "fr" ), QStringLiteral( "french" ) },
    { QStringLiteral( "hu" ), QStringLiteral( "hungarian" ) },
    { QStringLiteral( "it" ), QStringLiteral( "italian" ) },
    { QStringLiteral( "nb" ), QStringLiteral( "norwegian" ) },
    { QStringLiteral( "nl" ), QStringLiteral( "dutch" ) },
    { QStringLiteral( "nn" ), QStringLiteral( "norwegian" ) },
    { QStringLiteral( "no" ), QStringLiteral( "norwegian" ) },
    { QStringLiteral( "pt" ), QStringLiteral( "portuguese" ) },
    { QStringLiteral( "ro" ), QStringLiteral( "romanian" ) },
    { QStringLiteral( "ru" ), QStringLiteral( "russian" ) },
    { QStringLiteral( "sv" ), QStringLiteral( "swedish" ) },
    { QStringLiteral( "tr" ), QStringLiteral( "turkish" ) }
  } );
  return Xapian::Stem( stemmers.value( lang, QStringLiteral("none") ).toStdString() );
}

int main( int argc, char *argv[] )
{
  QCoreApplication app( argc, argv );

  QCommandLineParser parser;
  const QCommandLineOption indexdirOption( QStringLiteral("indexdir"), QStringLiteral("Index directory"), QStringLiteral("dir") );
  parser.addOption( indexdirOption );
  const QCommandLineOption identifierOption( QStringLiteral("identifier"), QStringLiteral("Index identifier"), QStringLiteral("identifier") );
  parser.addOption( identifierOption );
  const QCommandLineOption langOption( QStringLiteral("lang"), QStringLiteral("Language"), QStringLiteral("lang") );
  parser.addOption( langOption );

  parser.process( app );

  const QString indexdir = parser.value( indexdirOption );
  const QString identifier = parser.value( identifierOption );
  if ( indexdir.isEmpty() || identifier.isEmpty() ) {
    qCCritical(LOG) << "Missing arguments.";
    parser.showHelp( 1 );
  }

  QString lang = parser.value( langOption );
  if ( lang.isEmpty() || lang == QLatin1String( "C" ) ) {
    lang = QStringLiteral("en");
  }

  Xapian::WritableDatabase db;

  try {
    db = openWritableDb( indexdir + QLatin1Char('/') + identifier );
  } catch ( const Xapian::Error& e ) {
    qCCritical(LOG) << "Xapian error: " << e.get_description();
    return 1;
  }

  Xapian::TermGenerator xgen;
  xgen.set_stemmer( stemmerForLanguage( lang ) );

  QSet<Xapian::docid> handledDocuments;
  const std::string std_lang = lang.toStdString();

  const QStringList docDirs = KDocTools::documentationDirs();

  QStringList localDoc;

  QStringList::ConstIterator it = docDirs.constBegin();
  for ( ; it != docDirs.constEnd(); ++it ) {
    const QString docDirLangName = QString( ( *it ) % QStringLiteral("/") % lang % QStringLiteral("/") );
    QFileInfo docDirLang = QFileInfo( docDirLangName );
    if ( docDirLang.exists() && docDirLang.isDir() ) {
      localDoc << docDirLangName;
    }
  }
  qCDebug(LOG) << "documentation directories:" << localDoc;
  Q_FOREACH ( const QString &path, localDoc ) {
    walkFiles( path, std_lang, db, xgen, &handledDocuments );
  }

  try {
    const std::string lid = "L" + std_lang;

    for ( Xapian::PostingIterator it = db.postlist_begin( lid ); it != db.postlist_end( lid ); ++it ) {
      const Xapian::docid id = *it;
      // handled already, skip
      if ( handledDocuments.contains( id ) ) {
        continue;
      }

      qCDebug(LOG) << "obsolete document:" << id;
#if 0
      if ( LOG.isDebugEnabled() ) {
        const Xapian::Document doc = db.get_document( id );
        for ( Xapian::TermIterator it = doc.termlist_begin(); it != doc.termlist_end(); ++it ) {
          qCDebug(LOG) << "   " << *it;
        }
      }
#endif
      db.delete_document( id );
    }
  } catch ( const Xapian::Error& e ) {
    qCCritical(LOG) << "Xapian error:" << e.get_description();
    return 1;
  }

  db.commit();

  return 0;
}
