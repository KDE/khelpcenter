/*
    SPDX-FileCopyrightText: 2016 Pino Toscano <pino@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KHC_BOOKMARKOWNER_H
#define KHC_BOOKMARKOWNER_H

#include <KBookmarkOwner>
#include <QObject>

namespace KHC {

class View;

class BookmarkOwner : public QObject, public KBookmarkOwner
{
  Q_OBJECT
  public:
    explicit BookmarkOwner( View *view, QObject *parent = nullptr );
    ~BookmarkOwner() override;

    QString currentTitle() const override;
    QUrl currentUrl() const override;
    void openBookmark( const KBookmark& bm, Qt::MouseButtons mb, Qt::KeyboardModifiers km ) override;

  Q_SIGNALS:
    void openUrl( const QUrl& url );

  private:
    View *const mView;
};

}

#endif
