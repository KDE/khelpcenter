#ifndef KHC_HTMLSEARCH_H
#define KHC_HTMLSEARCH_H

#include <qobject.h>
#include <qstring.h>

class KConfig;

namespace KHC {

class DocEntry;

class HTMLSearch : public QObject
{
    Q_OBJECT
  public:
    HTMLSearch();

    void setupDocEntry( KHC::DocEntry * );

    QString defaultSearch( KHC::DocEntry * );
    QString defaultIndexer( KHC::DocEntry * );
    QString defaultIndexTestFile( KHC::DocEntry * );

  private:
    KConfig *mConfig;
};

}

#endif
