#ifndef __mainwindow_h__
#define __mainwindow_h__

#include <qptrlist.h>

#include <kmainwindow.h>
#include <kio/job.h>
#include <kurl.h>
#include <kparts/browserextension.h>

#include "navigator.h"
#include "glossary.h"

class KHTMLPart;
class QSplitter;

namespace KHC {

class View;

class MainWindow : public KMainWindow
{
    Q_OBJECT
  public:
    MainWindow(const KURL &url = KURL() );
    ~MainWindow();

  public slots:
    void slotStarted(KIO::Job *job);
    void slotInfoMessage(KIO::Job *, const QString &);
    void openURL(const QString &url);
    void slotGlossSelected(const GlossaryEntry &entry);                           
    void slotOpenURLRequest( const KURL &url,
                             const KParts::URLArgs &args);
    void documentCompleted();
    void print();
    void statusBarMessage(const QString &m);

  private:
    void openURL( const KURL &url );
    void stop();

    View *doc;
    QSplitter *splitter;
    Navigator *nav;
};

}

#endif
// vim:ts=2:sw=2:et
