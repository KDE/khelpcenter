/*
    SPDX-FileCopyrightText: 2016 Pino Toscano <pino@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "xapiancommon.h"

#include <QCoreApplication>
#include <QCommandLineParser>
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
  for ( const QString& word : splitlist ) {
    wordlist.append( word.toStdString() );
  }

  return Xapian::Query( op, wordlist.constBegin(), wordlist.constEnd() );
}

int main( int argc, char *argv[] )
{
  QCoreApplication app( argc, argv );

  QCommandLineParser parser;
  const QCommandLineOption indexdirOption( QStringLiteral("indexdir"), QStringLiteral("Index directory"), QStringLiteral("dir") );
  parser.addOption( indexdirOption );
  const QCommandLineOption identifierOption( QStringLiteral("identifier"), QStringLiteral("Index identifier"), QStringLiteral("identifier") );
  parser.addOption( identifierOption );
  const QCommandLineOption wordsOption( QStringLiteral("words"), QStringLiteral("Words to search"), QStringLiteral("words") );
  parser.addOption( wordsOption );
  const QCommandLineOption methodOption( QStringLiteral("method"), QStringLiteral("Method"), QStringLiteral("and|or") );
  parser.addOption( methodOption );
  const QCommandLineOption maxnumOption( QStringLiteral("maxnum"), QStringLiteral("Maximum number of results"), QStringLiteral("maxnum") );
  parser.addOption( maxnumOption );
  const QCommandLineOption langOption( QStringLiteral("lang"), QStringLiteral("Language"), QStringLiteral("lang") );
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
  if ( method == QLatin1String( "and" ) ) {
    op = Xapian::Query::OP_AND;
  } else if ( method == QLatin1String( "or" ) ) {
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
    lang = QStringLiteral("en");
  }

  Xapian::Database db;

  try {
    db = openDb( indexdir + QLatin1Char('/') + identifier );
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
      getDocInfo( doc, nullptr, &uid, &html );
      const std::string title = doc.get_value( VALUE_TITLE );

      const std::string::size_type slash = uid.find_last_of( '/' );
      const std::string html_id = slash != std::string::npos
                                ? uid.substr( 0, slash + 1 ) + html
                                : html;
      const std::string partial_id = html_id.substr(0, html_id.rfind("/"));

      std::cout << "<li><a href=\"help:/" << html_id << "\">" << partial_id
                << " - " << title << "</a></li>" << std::endl;
    }

    std::cout << "</ul>" << std::endl;

  } catch ( const Xapian::Error& e ) {
    qCCritical(LOG) << "Xapian error: " << e.get_description();
    return 1;
  }

  return 0;
}
