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

#ifndef CACHEREADER_H
#define CACHEREADER_H

#include <QByteArray>
#include <QMultiHash>
#include <QPair>
#include <QSet>
#include <QString>

class CacheReader
{
  public:
    CacheReader();
    ~CacheReader();

    bool parse( const QString& file );

    QSet<QString> documents() const;
    QByteArray document( const QString& id ) const;

  private:
    typedef QPair<int, int> Range;

    QString mText;
    QMultiHash<QString, Range> mRanges;
};

#endif
