#ifndef __view_h__
#define __view_h__

#include <khtml_part.h>

#include "glossary.h"
#include "navigator.h"

namespace DOM {
  class Node;
}

namespace KHC {

class View : public KHTMLPart
{
    Q_OBJECT
  public:
    View( QWidget *parentWidget, const char *widgetName,
             QObject *parent, const char *name, KHTMLPart::GUIProfile prof );

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

    int zoomStepping() const { return m_zoomStepping; }

  public slots:
    void lastSearch();
    void slotIncFontSizes();
    void slotDecFontSizes();

  signals:
    void searchResultCacheAvailable();

  protected:
    bool eventFilter( QObject *o, QEvent *e );

  private slots:
    void setTitle( const QString &title );

  private:
    void showAboutPage();
    KURL urlFromLinkNode( const DOM::Node &n ) const;
 
    int mState;
    QString mTitle;

    QString mSearchResult;
    int m_zoomStepping;
};

}

#endif
/**
 * vim:et
 */
// vim:ts=2:sw=2:et
