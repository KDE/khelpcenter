
#ifndef KHC_VIEW_H
#define KHC_VIEW_H

#include <khtml_part.h>

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

    bool openUrl( const QUrl &url ) Q_DECL_OVERRIDE;

    void saveState( QDataStream &stream ) Q_DECL_OVERRIDE;
    void restoreState( QDataStream &stream ) Q_DECL_OVERRIDE;

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
    bool eventFilter( QObject *o, QEvent *e ) Q_DECL_OVERRIDE;

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
