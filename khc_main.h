
#ifndef __khc_main_h__
#define __khc_main_h__

#include <qobject.h>

class Listener : public QObject
{
  Q_OBJECT
public:
  Listener() {}

 private slots:
  void slotAppRegistered( const QCString &appId );

};

#endif
