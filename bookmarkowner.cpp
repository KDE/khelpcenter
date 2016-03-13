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

#include "bookmarkowner.h"

#include "view.h"

#include <dom/html_document.h>

using namespace KHC;

BookmarkOwner::BookmarkOwner( View *view, QObject *parent )
  : QObject( parent ), KBookmarkOwner()
  , mView( view )
{
}

BookmarkOwner::~BookmarkOwner()
{
}

QString BookmarkOwner::currentTitle() const
{
  return currentUrl().isValid() ? mView->htmlDocument().title().string() : QString();
}

QUrl BookmarkOwner::currentUrl() const
{
  const QUrl url = mView->baseURL();
  // khelpcenter: URLs are internal, hence to not bookmark
  return url.scheme() == QLatin1String( "khelpcenter" ) ? QUrl() : url;
}

bool BookmarkOwner::enableOption( BookmarkOption option ) const
{
  switch ( option ) {
    case ShowAddBookmark:
      return true;
    default:
      return false;
  }
}

void BookmarkOwner::openBookmark( const KBookmark& bm, Qt::MouseButtons /*mb*/, Qt::KeyboardModifiers /*km*/ )
{
  if ( !bm.url().isValid() ) {
    return;
  }

  emit openUrl( bm.url() );
}
