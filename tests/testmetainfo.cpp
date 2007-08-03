#include <QCoreApplication>

#include <kaboutdata.h>
#include <kdebug.h>
#include <klocale.h>
#include <kcmdlineargs.h>

#include "docmetainfo.h"
#include "docentrytraverser.h"

using namespace KHC;

class MyTraverser : public DocEntryTraverser
{
  public:
    MyTraverser( const QString &indent = "" ) : mIndent( indent ) {}

    void process( DocEntry *entry )
    {
      kDebug() << mIndent << entry->name() << " - WEIGHT: " << entry->weight()
                << endl;
#if 0
      if ( entry->parent() ) kDebug() << mIndent << "  PARENT: "
                                       << entry->parent()->name() << endl;
      if ( entry->nextSibling() ) kDebug() << mIndent << "  NEXT: "
                                       << entry->nextSibling()->name() << endl;
#endif
    }
  
    DocEntryTraverser *createChild( DocEntry * )
    {
      return new MyTraverser( mIndent + "  " );
    }

  private:
    QString mIndent;
};

class LinearTraverser : public DocEntryTraverser
{
  public:
    void process( DocEntry *entry )
    {
      kDebug() << "PROCESS: " << entry->name();
    }
    
    DocEntryTraverser *createChild( DocEntry * )
    {
      return this;
    }
    
    DocEntryTraverser *parentTraverser()
    {
      return this;
    }
    
    void deleteTraverser() {}
};

class AsyncTraverser : public DocEntryTraverser
{
  public:
    AsyncTraverser( const QString &indent = "" ) : mIndent( indent )
    {
//      kDebug() << "AsyncTraverser()";
    }
    
    ~AsyncTraverser()
    {
//      kDebug() << "~AsyncTraverser()";
    }
    
    void process( DocEntry *entry )
    {
      kDebug() << mIndent << entry->name();
    }
    
    DocEntryTraverser *createChild( DocEntry * )
    {
//      kDebug() << "AsyncTraverser::childTraverser()";
      return new AsyncTraverser( mIndent + "  " );
    }

  private:
    QString mIndent;
};

int main(int argc,char **argv)
{
  KAboutData aboutData("testmetainfo", 0,ki18n("TestDocMetaInfo"),"0.1");
  // KComponentData componentData(&aboutData); doesn't seem to be necessary
  QCoreApplication app(argc,argv);

  kDebug() << "Scanning Meta Info";

  DocMetaInfo::self()->scanMetaInfo( );

  kDebug() << "My TRAVERSE start";
  MyTraverser t;
  DocMetaInfo::self()->startTraverseEntries( &t );
  kDebug() << "My TRAVERSE end";

  kDebug() << "Linear TRAVERSE start";
  LinearTraverser l;
  DocMetaInfo::self()->startTraverseEntries( &l );
  kDebug() << "Linear TRAVERSE end";

  kDebug() << "Async TRAVERSE start";
  AsyncTraverser a;
  DocMetaInfo::self()->startTraverseEntries( &a );
  kDebug() << "Async TRAVERSE end";

  return 0;
}
// vim:ts=2:sw=2:et
