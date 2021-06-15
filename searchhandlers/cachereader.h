/*
    SPDX-FileCopyrightText: 2016 Pino Toscano <pino@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
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
