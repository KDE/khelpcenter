
#ifndef __khc_main_h__
#define __khc_main_h__

#include <kmainwindow.h>

class KHTMLPart;

class KHMainWindow : public KMainWindow
{
    Q_OBJECT

public:
    KHMainWindow(const KURL &url);
    ~KHMainWindow();

private:
    KHTMLPart *doc;
};

#endif
