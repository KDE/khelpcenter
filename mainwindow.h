#ifndef __mainwindow_h__
#define __mainwindow_h__

#include <qptrlist.h>

#include <kmainwindow.h>
#include <kio/job.h>
#include <kurl.h>
#include <kparts/browserextension.h>
#include <dcopclient.h>
#include <dcopobject.h>

#include "navigator.h"
#include "glossary.h"

class KHTMLPart;
class QSplitter;

namespace KHC {

class View;

    class MainWindow : public KMainWindow, public DCOPObject
{
    Q_OBJECT
    K_DCOP
  public:
    MainWindow(const KURL &url = KURL() );
    ~MainWindow();
k_dcop:
    void openURL(const QString &url);

public slots:
    void slotOpenURL(const QString &url);
    void print();
    void statusBarMessage(const QString &m);
    void showHome();
    void lastSearch();

  protected:
    void setupActions();

  protected slots:
    void enableLastSearchAction();

  private:
    void openURL( const KURL &url );
    void stop();

  private slots:
    void slotGlossSelected(const GlossaryEntry &entry);
    void slotStarted(KIO::Job *job);
    void slotInfoMessage(KIO::Job *, const QString &);
    void slotOpenURLRequest( const KURL &url,
                             const KParts::URLArgs &args);
    void documentCompleted();
    void openOwnManual();

  private:
    View *mDoc;
    Navigator *mNavigator;

    KAction *mLastSearchAction;
};

}

#endif
// vim:ts=2:sw=2:et
