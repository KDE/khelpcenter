#ifndef __view_h__
#define __view_h__

#include <khtml_part.h>

#include "glossary.h"
#include "navigator.h"

class KActionCollection;

namespace DOM {
  class Node;
}

namespace KHC {

class Formatter;

class View : public KHTMLPart
{
    Q_OBJECT
  public:
    View( QWidget *parentWidget, const char *widgetName,
          QObject *parent, const char *name, KHTMLPart::GUIProfile prof,
          KActionCollection *col );

    ~View();

    virtual bool openURL( const KURL &url );

    virtual void saveState( QDataStream &stream );
    virtual void restoreState( QDataStream &stream );

    enum State { Docu, About, Search };

    int state() const { return mState; }
    QString title() const { return mTitle; }

    static QString langLookup( const QString &fname );

    void beginSearchResult();
    void writeSearchResult( const QString & );
    void endSearchResult();

    void beginInternal( const KURL & );
    KURL internalUrl() const;

    int zoomStepping() const { return m_zoomStepping; }

    Formatter *formatter() const { return mFormatter; }

    void copySelectedText();

  public slots:
    void lastSearch();
    void slotIncFontSizes();
    void slotDecFontSizes();
    void slotReload( const KURL &url = KURL() );
    void slotCopyLink();
    bool nextPage(bool checkOnly = false);
    bool prevPage(bool checkOnly = false);

  signals:
    void searchResultCacheAvailable();

  protected:
    bool eventFilter( QObject *o, QEvent *e );

  private slots:
    void setTitle( const QString &title );
    void showMenu( const QString& url, const QPoint& pos);

  private:
    void showAboutPage();
    KURL urlFromLinkNode( const DOM::Node &n ) const;
 
    int mState;
    QString mTitle;

    QString mSearchResult;
    KURL mInternalUrl;

    int m_zoomStepping;

    Formatter *mFormatter;
    KActionCollection *mActionCollection;
    QString mCopyURL;
};

}

#endif

// vim:ts=2:sw=2:et
