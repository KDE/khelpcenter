/*
    SPDX-FileCopyrightText: 2002 Cornelius Schumacher <schumacher@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KHC_VIEW_H
#define KHC_VIEW_H

#include <QWebEngineView>
class QWebEngineDownloadRequest;
class KActionCollection;

namespace KHC
{

class GrantleeFormatter;

class View : public QWebEngineView
{
    Q_OBJECT
public:
    View(QWidget *parentWidget, KActionCollection *col);

    ~View();

    enum State {
        Docu,
        About,
        Search
    };

    int state() const
    {
        return mState;
    }

    static QString langLookup(const QString &fname);

    void beginSearchResult();
    void writeSearchResult(const QString &);
    void endSearchResult();

    void setInternalHtml(const QString &, const QUrl &);
    QUrl internalUrl() const;

    GrantleeFormatter *grantleeFormatter() const
    {
        return mGrantleeFormatter;
    }

    void copySelectedText();

public Q_SLOTS:
    void lastSearch();
    void slotReload(const QUrl &url = QUrl());
    void setCurrentlyHoveredLink(const QString &link);

Q_SIGNALS:
    void searchResultCacheAvailable();

protected:
    void contextMenuEvent(QContextMenuEvent *ev) override;

private:
    void downloadRequested(QWebEngineDownloadRequest *);

    int mState;

    QString mSearchResult;
    QUrl mInternalUrl;

    GrantleeFormatter *mGrantleeFormatter = nullptr;
    KActionCollection *mActionCollection = nullptr;
    QString mCurrentlyHoveredLink;
};

}

#endif // KHC_VIEW_H

// vim:ts=2:sw=2:et
