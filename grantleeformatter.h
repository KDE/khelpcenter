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

#ifndef KHC_GRANTLEEFORMATTER_H
#define KHC_GRANTLEEFORMATTER_H

#include <QList>
#include <QPair>
#include <QString>

namespace KHC {

class DocEntry;
class GlossaryEntry;

  class GrantleeFormatter
  {
    public:
      GrantleeFormatter();
      ~GrantleeFormatter();

      QString formatOverview( const QString& title, const QString& name, const QString& content );
      QString formatGlossaryEntry( const GlossaryEntry& entry );
      QString formatSearchResults( const QString& words, const QList<QPair<DocEntry *, QString> >& results );

    private:
      struct Private;
      Private *d;
  };

}

#endif //KHC_FORMATTER_H

// vim:ts=2:sw=2:et
