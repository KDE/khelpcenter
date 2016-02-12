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
