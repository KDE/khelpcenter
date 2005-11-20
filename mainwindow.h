#ifndef __mainwindow_h__
#define __mainwindow_h__

#include <q3ptrlist.h>

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
    MainWindow();
    ~MainWindow();

  k_dcop:
    void openUrl( const QString &url );
    void openUrl( const QString &url, const QByteArray& startup_id );
    void showHome();
    void lastSearch();

  public slots:
    void print();
    void statusBarMessage(const QString &m);
    void slotShowHome();
    void slotLastSearch();
    void showSearchStderr();
    /**
      Show document corresponding to given URL in viewer part.
    */
    void viewUrl( const KURL &url,
                  const KParts::URLArgs &args = KParts::URLArgs() );
    /**
      Show document corresponding to given URL in viewer part.
    */
    void viewUrl( const QString & );

    /**
      Open document corresponding to given URL, i.e. show it in the viewer part
      and select the corresponding entry in the navigator widget.
    */
    void openUrl( const KURL &url );

  protected:
    void setupActions();

    virtual void saveProperties( KConfig *config );
    virtual void readProperties( KConfig *config );

    void readConfig();
    void writeConfig();

  protected slots:
    void enableLastSearchAction();
    void enableCopyTextAction();

  private:
    void stop();

  private slots:
    void slotGlossSelected(const GlossaryEntry &entry);
    void slotStarted(KIO::Job *job);
    void slotInfoMessage(KIO::Job *, const QString &);
    void goInternalUrl( const KURL & );
    /**
      This function is called when the user clicks on a link in the viewer part.
    */
    void slotOpenURLRequest( const KURL &url,
                             const KParts::URLArgs &args);
    void documentCompleted();
    void slotIncFontSizes();
    void slotDecFontSizes();
    void slotConfigureFonts();
    void slotCopySelectedText();

private:
    void updateZoomActions();

    QSplitter *mSplitter;
    View *mDoc;
    Navigator *mNavigator;

    KAction *mLastSearchAction;
    KAction *mCopyText;
    LogDialog *mLogDialog;
};

}

#endif
// vim:ts=2:sw=2:et
