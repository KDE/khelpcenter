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

class LogDialog;

namespace KHC {

class View;

class MainWindow : public KMainWindow, public DCOPObject
{
    Q_OBJECT
    K_DCOP
  public:
    MainWindow( const KURL &url = KURL() );
    ~MainWindow();

  k_dcop:
    void openURL(const QString &url);
    void showHome();
    void lastSearch();

  public slots:
    void slotOpenURL(const QString &url);
    void print();
    void statusBarMessage(const QString &m);
    void slotShowHome();
    void slotLastSearch();
    void showSearchStderr();

  protected:
    void setupActions();

    virtual void saveProperties( KConfig *config );
    virtual void readProperties( KConfig *config );

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
    void slotIncFontSizes();
    void slotDecFontSizes();
    void slotConfigureFonts();
    void slotConfigureKeys();

private:
    void updateZoomActions();

    View *mDoc;
    Navigator *mNavigator;

    KAction *mLastSearchAction;

    LogDialog *mLogDialog;
};

}

#endif
// vim:ts=2:sw=2:et
