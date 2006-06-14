#ifndef __mainwindow_h__
#define __mainwindow_h__

#include <kmainwindow.h>
#include <kio/job.h>
#include <kurl.h>
#include <kparts/browserextension.h>

#include "navigator.h"
#include "glossary.h"

class KHTMLPart;
class QSplitter;

class LogDialog;

namespace KHC {

class View;

class MainWindow : public KMainWindow
{
    Q_OBJECT
	Q_CLASSINFO("D-Bus Interface", "org.kde.khelpcenter.khelpcenter")
  public:
    MainWindow();
    ~MainWindow();

  public Q_SLOTS:
    Q_SCRIPTABLE void openUrl( const QString &url );
    Q_SCRIPTABLE void openUrl( const QString &url, const QByteArray& startup_id );
    Q_SCRIPTABLE void showHome();
    Q_SCRIPTABLE void lastSearch();

  public Q_SLOTS:
    void print();
    void statusBarMessage(const QString &m);
    void slotShowHome();
    void slotLastSearch();
    void showSearchStderr();
    /**
      Show document corresponding to given URL in viewer part.
    */
    void viewUrl( const KUrl &url,
                  const KParts::URLArgs &args = KParts::URLArgs() );
    /**
      Show document corresponding to given URL in viewer part.
    */
    void viewUrl( const QString & );

    /**
      Open document corresponding to given URL, i.e. show it in the viewer part
      and select the corresponding entry in the navigator widget.
    */
    void openUrl( const KUrl &url );

  protected:
    void setupActions();

    virtual void saveProperties( KConfig *config );
    virtual void readProperties( KConfig *config );

    void readConfig();
    void writeConfig();

  protected Q_SLOTS:
    void enableLastSearchAction();
    void enableCopyTextAction();

  private:
    void stop();

  private Q_SLOTS:
    void slotGlossSelected(const GlossaryEntry &entry);
    void slotStarted(KIO::Job *job);
    void slotInfoMessage(KJob *, const QString &);
    void goInternalUrl( const KUrl & );
    /**
      This function is called when the user clicks on a link in the viewer part.
    */
    void slotOpenURLRequest( const KUrl &url,
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
