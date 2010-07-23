
#ifndef KHC_VIEW_H
#define KHC_VIEW_H

#include <khtml_part.h>

#include "glossary.h"
#include "navigator.h"

#include <QEvent>

class KActionCollection;

namespace DOM {
  class Node;
  class HTMLLinkElement;
}

namespace KHC {

class Formatter;

class View : public KHTMLPart
{
    Q_OBJECT
  public:
    View( QWidget *parentWidget, QObject *parent, KHTMLPart::GUIProfile prof,
          KActionCollection *col );

    ~View();

    virtual bool openUrl( const KUrl &url );

    virtual void saveState( QDataStream &stream );
    virtual void restoreState( QDataStream &stream );

    enum State { Docu, About, Search };

    int state() const { return mState; }
    QString title() const { return mTitle; }

    static QString langLookup( const QString &fname );

    void beginSearchResult();
    void writeSearchResult( const QString & );
    void endSearchResult();

    void beginInternal( const KUrl & );
    KUrl internalUrl() const;

    int zoomStepping() const { return m_zoomStepping; }

    Formatter *formatter() const { return mFormatter; }

    void copySelectedText();

  public Q_SLOTS:
    void lastSearch();
    void slotIncFontSizes();
    void slotDecFontSizes();
    void slotReload( const KUrl &url = KUrl() );
    void slotCopyLink();
    bool nextPage(bool checkOnly = false);
    bool prevPage(bool checkOnly = false);

  Q_SIGNALS:
    void searchResultCacheAvailable();

  protected:
    bool eventFilter( QObject *o, QEvent *e );

  private Q_SLOTS:
    void setTitle( const QString &title );
    void showMenu( const QString& url, const QPoint& pos);

  private:
    KUrl urlFromLinkNode( const DOM::HTMLLinkElement &link ) const;
 
    int mState;
    QString mTitle;

    QString mSearchResult;
    KUrl mInternalUrl;

    int m_zoomStepping;

    Formatter *mFormatter;
    KActionCollection *mActionCollection;
    QString mCopyURL;
};

}

#endif //KHC_VIEW_H

// vim:ts=2:sw=2:et
