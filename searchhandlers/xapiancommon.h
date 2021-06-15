/*
    SPDX-FileCopyrightText: 2016 Pino Toscano <pino@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KHC_XAPIANCOMMON_H
#define KHC_XAPIANCOMMON_H

// must be put before any Qt include
#include <xapian.h>

#include <QDebug>
#include <QString>

enum {
  VALUE_LASTMOD = 0,
  VALUE_TITLE = 1,
};

struct DatabaseVersionMismatch
{
  DatabaseVersionMismatch( int aversion );

  const int version;
  const int refVersion;
};

Xapian::WritableDatabase openWritableDb( const QString& path );
Xapian::Database openDb( const QString& path );
void getDocInfo( const Xapian::Document& doc, std::string* lang, std::string* uid, std::string* xhtml );

QDebug operator<<( QDebug dbg, const std::string& s );

#endif
