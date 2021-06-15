/*
    SPDX-FileCopyrightText: 2016 Pino Toscano <pino@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
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
      Private *const d;
  };

}

#endif //KHC_FORMATTER_H

// vim:ts=2:sw=2:et
