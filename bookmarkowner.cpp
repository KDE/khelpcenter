/*
    SPDX-FileCopyrightText: 2016 Pino Toscano <pino@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
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

void BookmarkOwner::openBookmark( const KBookmark& bm, Qt::MouseButtons /*mb*/, Qt::KeyboardModifiers /*km*/ )
{
  if ( !bm.url().isValid() ) {
    return;
  }

  Q_EMIT openUrl( bm.url() );
}
