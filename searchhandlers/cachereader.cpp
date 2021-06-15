/*
    SPDX-FileCopyrightText: 2016 Pino Toscano <pino@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "cachereader.h"

#include <QLoggingCategory>
#include <QStack>
#include <QTextStream>

#include <KCompressionDevice>

#include <algorithm>

namespace {

Q_LOGGING_CATEGORY( LOG, "org.kde.khelpcenter.xapian.cachereader", QtWarningMsg )

}

static bool readAll( const QString& file, QString* data )
{
  KCompressionDevice dev( file, KCompressionDevice::BZip2 );
  if ( !dev.open( QIODevice::ReadOnly ) ) {
    qCWarning(LOG) << "cannot open" << file << ":" << dev.errorString();
    return false;
  }

  *data = QString::fromUtf8( dev.readAll() );
  return true;
}

CacheReader::CacheReader()
{
}

CacheReader::~CacheReader()
{
}

bool CacheReader::parse( const QString& file )
{
  mText.clear();
  mRanges.clear();

  if ( !readAll( file, &mText ) ) {
    return false;
  }
  const int length = mText.length();

  static const QString patternStart = QStringLiteral( "<FILENAME filename=\"" );
  static const QString patternEnd = QStringLiteral( "</FILENAME>" );

  QStack<QString> stack;

  int index = 0;
  while ( index < length ) {
    int start = mText.indexOf( patternStart, index );
    int end = mText.indexOf( patternEnd, index );

    if ( start >= 0 && start < end ) {
      // new document
      const int quote = mText.indexOf( QLatin1Char('"'), start + patternStart.length() );
      const QString name = mText.mid( start + patternStart.length(), quote - ( start + patternStart.length() ) );
      if ( stack.isEmpty() ) {
      } else {
        if ( start > index ) {
          mRanges.insert( stack.top(), qMakePair( index, start - 1 ) );
        }
      }
      index = quote + 2;
      stack.push( name );
    } else if ( end >= 0 ) {
      // end of current document
      Q_ASSERT( !stack.isEmpty() );
      mRanges.insert( stack.top(), qMakePair( index, end - 1 ) );
      index = end + patternEnd.length();
      stack.pop();
    } else {
      break;
    }
  }

  Q_ASSERT( stack.isEmpty() );

  return true;
}

QSet<QString> CacheReader::documents() const
{
  return QSet<QString>( mRanges.keyBegin(), mRanges.keyEnd() );
}

QByteArray CacheReader::document( const QString& id ) const
{
  QList<Range> docRanges = mRanges.values( id );
  if ( docRanges.isEmpty() ) {
    return QByteArray();
  }

  QByteArray doc;

  if ( docRanges.count() == 1 ) {
    const Range range = docRanges.first();
    doc = mText.midRef( range.first, range.second - range.first + 1 ).toUtf8();
  } else {
    std::reverse( docRanges.begin(), docRanges.end() );
    QTextStream stream( &doc );
    stream.setCodec( "UTF-8" );
    for ( const Range &range : qAsConst(docRanges) ) {
      stream << mText.mid( range.first, range.second - range.first + 1 );
    }
  }

  return doc;
}
