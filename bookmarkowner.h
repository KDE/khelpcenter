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

#ifndef KHC_BOOKMARKOWNER_H
#define KHC_BOOKMARKOWNER_H

#include <kbookmarkowner.h>
#include <QObject>

namespace KHC {

class View;

class BookmarkOwner : public QObject, public KBookmarkOwner
{
  Q_OBJECT
  public:
    BookmarkOwner( View *view, QObject *parent = 0 );
    virtual ~BookmarkOwner();

    QString currentTitle() const Q_DECL_OVERRIDE;
    QUrl currentUrl() const Q_DECL_OVERRIDE;
    void openBookmark( const KBookmark& bm, Qt::MouseButtons mb, Qt::KeyboardModifiers km ) Q_DECL_OVERRIDE;

  Q_SIGNALS:
    void openUrl( const QUrl& url );

  private:
    View *mView;
};

}

#endif
