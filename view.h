#ifndef __view_h__
#define __view_h__

#include <khtml_part.h>

#include "glossary.h"
#include "navigator.h"

namespace KHC {

class View : public KHTMLPart
{
    Q_OBJECT
  public:
    View( QWidget *parentWidget, const char *widgetName,
             QObject *parent, const char *name, KHTMLPart::GUIProfile prof );

    virtual bool openURL( const KURL &url );

    QString title() const { return mTitle; }

    static QString langLookup( const QString &fname );

    void beginSearchResult();
    void writeSearchResult( const QString & );
    void endSearchResult();

  public slots:
    void lastSearch();

  signals:
    void searchResultCacheAvailable();

  private slots:
    void setTitle( const QString &title );

  private:
    void showAboutPage();
 
    QString mTitle;

    QString mSearchResult;
};

}

#endif
/**
 * vim:et
 */
// vim:ts=2:sw=2:et
