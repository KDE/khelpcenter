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

#include "xapiancommon.h"

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QFile>
#include <QLoggingCategory>
#include <QVector>

#include <iostream>

namespace {

Q_LOGGING_CATEGORY( LOG, "org.kde.khelpcenter.xapian.search", QtWarningMsg )

}

Xapian::Query queryFromWordlist( const QString& words, Xapian::Query::op op )
{
  QVector<std::string> wordlist;
  const QStringList splitlist = words.split( QLatin1Char( '+' ) );
  wordlist.reserve( splitlist.size() );
  Q_FOREACH ( const QString& word, splitlist ) {
    wordlist.append( word.toStdString() );
  }

  return Xapian::Query( op, wordlist.constBegin(), wordlist.constEnd() );
}

int main( int argc, char *argv[] )
{
  QCoreApplication app( argc, argv );

  QCommandLineParser parser;
  const QCommandLineOption indexdirOption( "indexdir", "Index directory", "dir" );
  parser.addOption( indexdirOption );
  const QCommandLineOption identifierOption( "identifier", "Index identifier", "identifier" );
  parser.addOption( identifierOption );
  const QCommandLineOption wordsOption( "words", "Words to search", "words" );
  parser.addOption( wordsOption );
  const QCommandLineOption methodOption( "method", "Method", "and|or" );
  parser.addOption( methodOption );
  const QCommandLineOption maxnumOption( "maxnum", "Maximum number of results", "maxnum" );
  parser.addOption( maxnumOption );
  const QCommandLineOption langOption( "lang", "Language", "lang" );
  parser.addOption( langOption );

  parser.process( app );

  const QString indexdir = parser.value( indexdirOption );
  const QString identifier = parser.value( identifierOption );
  const QString words = parser.value( wordsOption );
  const QString method = parser.value( methodOption );
  const QString maxnumString = parser.value( maxnumOption );
  if ( indexdir.isEmpty() || identifier.isEmpty() ||  words.isEmpty() || method.isEmpty() ) {
    qCCritical(LOG) << "Missing arguments.";
    parser.showHelp( 1 );
  }

  Xapian::Query::op op;
  if ( method == QStringLiteral( "and" ) ) {
    op = Xapian::Query::OP_AND;
  } else if ( method == QStringLiteral( "or" ) ) {
    op = Xapian::Query::OP_OR;
  } else {
    qCCritical(LOG) << "Unrecognized method:" << method;
    parser.showHelp( 1 );
  }

  bool ok;
  const Xapian::doccount maxnum = maxnumString.toUInt( &ok );
  if ( !ok ) {
    qCCritical(LOG) << "--maxnum is not a number";
    parser.showHelp( 1 );
  }

  QString lang = parser.value( langOption );
  if ( lang.isEmpty() || lang == QLatin1String( "C" ) ) {
    lang = "en";
  }

  Xapian::Database db;

  try {
    db = openDb( indexdir + "/" + identifier );
  } catch ( const DatabaseVersionMismatch& e ) {
    qCWarning(LOG) << "Own version mismatch in Xapian DB: found" << e.version << "vs wanted" << e.refVersion;
    return 1;
  } catch ( const Xapian::Error& e ) {
    qCCritical(LOG) << "Xapian error:" << e.get_description();
    return 1;
  }

  try {
    Xapian::Query query( Xapian::Query::OP_AND, Xapian::Query( "L" + lang.toStdString() ), queryFromWordlist( words, op ) );
    qCDebug(LOG) << "query:" << query.get_description();

    Xapian::Enquire enquire( db );
    enquire.set_query( query );

    const Xapian::MSet mset = enquire.get_mset( 0, maxnum );

    std::cout << "<ul>" << std::endl;

    qCDebug(LOG) << "got" << mset.size() << "results";
    for ( Xapian::MSetIterator i = mset.begin(); i != mset.end(); ++i ) {
      const Xapian::Document doc = i.get_document();

      std::string uid;
      std::string html;
      getDocInfo( doc, 0, &uid, &html );
      const std::string title = doc.get_value( VALUE_TITLE );

      const std::string::size_type slash = uid.find_last_of( '/' );
      const std::string html_id = slash != std::string::npos
                                ? uid.substr( 0, slash + 1 ) + html
                                : html;

      std::cout << "<li><a href=\"help:/" << html_id << "\">" << title << "</a></li>" << std::endl;
    }

    std::cout << "</ul>" << std::endl;

  } catch ( const Xapian::Error& e ) {
    qCCritical(LOG) << "Xapian error: " << e.get_description();
    return 1;
  }

  return 0;
}
