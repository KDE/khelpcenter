/*
    This file is part of KHelpcenter.

    Copyright (c) 2003 Cornelius Schumacher <schumacher@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#ifndef KHC_FORMATTER_H
#define KHC_FORMATTER_H

#include <qstring.h>
#include <qmap.h>

namespace KHC {

class Formatter
{
  public:
    Formatter();
    virtual ~Formatter();

    bool readTemplates();

    virtual QString header( const QString &title );
    virtual QString footer();
    virtual QString separator();
    virtual QString docTitle( const QString & );
    virtual QString sectionHeader( const QString & );
    virtual QString paragraph( const QString & );
    virtual QString title( const QString & );
    
    virtual QString processResult( const QString & );

  private:
    bool mHasTemplate;
    QMap<QString,QString> mSymbols;
};

}

#endif

// vim:ts=2:sw=2:et
