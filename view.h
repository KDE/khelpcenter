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

    virtual void saveState( QDataStream &stream );
    virtual void restoreState( QDataStream &stream );

    QString title() const { return m_title; }
    static QString langLookup( const QString &fname );

  public slots:
    void showGlossaryEntry( const GlossaryEntry &entry );

  private slots:
    void setTitle( const QString &title );

  private:
    void showAboutPage();
 
    enum State { Docu, About, GlossEntry };

    GlossaryEntry m_glossEntry;
    int m_state;
    QString m_title;
};

}

#endif
/**
 * vim:et
 */
