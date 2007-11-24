/* This file is part of the KDE project
   Copyright 2002 Cornelius Schumacher <schumacher@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License version 2 or at your option version 3 as published
   by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KHC_HTMLSEARCH_H
#define KHC_HTMLSEARCH_H

#include <QObject>


class KConfig;

namespace KHC {

class DocEntry;

class HTMLSearch : public QObject
{
    Q_OBJECT
  public:
    HTMLSearch();
    ~HTMLSearch();

    void setupDocEntry( KHC::DocEntry * );

    QString defaultSearch( KHC::DocEntry * );
    QString defaultIndexer( KHC::DocEntry * );
    QString defaultIndexTestFile( KHC::DocEntry * );

  private:
    KConfig *mConfig;
};

}

#endif
// vim:ts=2:sw=2:et
