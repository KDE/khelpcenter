
/*
    This file is part of KHelpCenter.

    Copyright (c) 2002 Cornelius Schumacher <schumacher@kde.org>

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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef KHC_VIEW_H
#define KHC_VIEW_H

#include <KHTMLPart>

class KActionCollection;

namespace DOM {
  class Node;
  class HTMLLinkElement;
}

namespace KHC {

class GrantleeFormatter;

class View : public KHTMLPart
{
    Q_OBJECT
  public:
    View( QWidget *parentWidget, QObject *parent, KHTMLPart::GUIProfile prof,
          KActionCollection *col );

    ~View();

    bool openUrl( const QUrl &url ) override;

    void saveState( QDataStream &stream ) override;
    void restoreState( QDataStream &stream ) override;

    enum State { Docu, About, Search };

    int state() const { return mState; }
    QString title() const { return mTitle; }

    static QString langLookup( const QString &fname );

    void beginSearchResult();
    void writeSearchResult( const QString & );
    void endSearchResult();

    void beginInternal( const QUrl & );
    QUrl internalUrl() const;

    int fontScaleStepping() const { return m_fontScaleStepping; }

    GrantleeFormatter *grantleeFormatter() const { return mGrantleeFormatter; }

    void copySelectedText();

  public Q_SLOTS:
    void lastSearch();
    void slotIncFontSizes();
    void slotDecFontSizes();
    void slotReload( const QUrl &url = QUrl() );
    void slotCopyLink();
    bool nextPage(bool checkOnly = false);
    bool prevPage(bool checkOnly = false);

  Q_SIGNALS:
    void searchResultCacheAvailable();

  protected:
    bool eventFilter( QObject *o, QEvent *e ) override;

  private Q_SLOTS:
    void setTitle( const QString &title );
    void showMenu( const QString& url, const QPoint& pos);

  private:
    QUrl urlFromLinkNode( const DOM::HTMLLinkElement &link ) const;
 
    int mState;
    QString mTitle;

    QString mSearchResult;
    QUrl mInternalUrl;

    int m_fontScaleStepping;

    GrantleeFormatter *mGrantleeFormatter;
    KActionCollection *mActionCollection;
    QString mCopyURL;
};

}

#endif //KHC_VIEW_H

// vim:ts=2:sw=2:et
