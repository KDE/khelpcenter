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
class KToolBarPopupAction;

namespace KHC {

class View;

struct HistoryEntry
{
  KURL url;
  QString title;
  QByteArray buffer;
};

class MainWindow : public KMainWindow
{
    Q_OBJECT
  public:
    MainWindow(const KURL &url);
    ~MainWindow();

  public slots:
    void slotStarted(KIO::Job *job);
    void slotInfoMessage(KIO::Job *, const QString &);
    void openURL(const QString &url);
    void slotGlossSelected(const GlossaryEntry &entry);                           
    void slotOpenURLRequest( const KURL &url,
                             const KParts::URLArgs &args);
    void slotBack();
    void slotBackActivated( int id );
    void slotForward();
    void slotForwardActivated( int id );
    void slotGoHistoryActivated( int steps );
    void slotGoHistoryDelayed();
    void documentCompleted();
    void fillBackMenu();
    void fillForwardMenu();
    void fillGoMenu();
    void goMenuActivated( int id );
    void print();
    void statusBarMessage(const QString &m);

  private:
    void createHistoryEntry();
    void updateHistoryEntry();
    void goHistory( int steps );
    void openURL( const KURL &url );
    void updateHistoryActions();
    void stop();
    void fillHistoryPopup( QPopupMenu *popup, bool onlyBack, bool onlyForward,
                           bool checkCurrentItem, uint startPos = 0 );

    inline bool canGoBack() const { return m_lstHistory.at() > 0; }
    inline bool canGoForward() const { return m_lstHistory.at() !=
                                       (int)m_lstHistory.count() - 1; }

    View *doc;
    QSplitter *splitter;
    Navigator *nav;
    KToolBarPopupAction *back, *forward;
    int m_goBuffer;
    QPtrList<HistoryEntry> m_lstHistory;
    int m_goMenuIndex;
    int m_goMenuHistoryStartPos;
    int m_goMenuHistoryCurrentPos;
};

}

#endif
