
#ifndef __khc_main_h__
#define __khc_main_h__

#include <kmainwindow.h>
#include <kio/job.h>

class KHTMLPart;
class QSplitter;
class khcNavigator;

class KHMainWindow : public KMainWindow
{
    Q_OBJECT

public:
    KHMainWindow(const KURL &url);
    ~KHMainWindow();

public slots:
    void slotStarted(KIO::Job *job);
    void slotInfoMessage(KIO::Job *, const QString &);

private:
    KHTMLPart *doc;
    QSplitter *splitter;
    khcNavigator *nav;
};

#endif
