
#ifndef __khc_main_h__
#define __khc_main_h__

#include <kmainwindow.h>
#include <kio/job.h>
#include <kparts/browserextension.h>
#include "khc_navigator.h"
#include <kurl.h>
#include <qlist.h>

class KHTMLPart;
class QSplitter;
class KToolBarPopupAction;

struct HistoryEntry
{
  KURL url;
  QString title;
  QByteArray buffer;
};

class KHMainWindow : public KMainWindow
{
    Q_OBJECT

public:
    KHMainWindow(const KURL &url);
    ~KHMainWindow();

public slots:
    void slotStarted(KIO::Job *job);
    void slotInfoMessage(KIO::Job *, const QString &);
    void openURL(const QString &url);
    void slotGlossSelected(const khcNavigatorWidget::GlossaryEntry &entry);
    void slotOpenURLRequest( const KURL &url,
                             const KParts::URLArgs &args);
    void slotBack();
    void slotBackActivated( int id );
    void slotForward();
    void slotForwardActivated( int id );
    void slotGoHistoryActivated( int steps );
    void slotGoHistoryDelayed();

private:
    void prepareAbout();
    void createHistoryEntry();
    QString langLookup(const QString &);

    KHTMLPart *doc;
    QSplitter *splitter;
    khcNavigator *nav;
    KToolBarPopupAction *backAction, *forwardAction;
    int m_goBuffer;
    QList<HistoryEntry> m_lstHistory;

};

#endif
