#include <QCoreApplication>
#include <QDebug>

#include <kcmdlineargs.h>
#include <KAboutData>

#include "docmetainfo.h"
#include "docentrytraverser.h"

using namespace KHC;

class MyTraverser : public DocEntryTraverser
{
  public:
    MyTraverser( const QString &indent = "" ) : mIndent( indent ) {}

    void process( DocEntry *entry )
    {
      qDebug() << mIndent << entry->name() << " - WEIGHT: " << entry->weight();
#if 0
      if ( entry->parent() ) qDebug() << mIndent << "  PARENT: "
                                       << entry->parent()->name() << endl;
      if ( entry->nextSibling() ) qDebug() << mIndent << "  NEXT: "
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
      qDebug() << "PROCESS: " << entry->name();
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
//      qDebug() << "AsyncTraverser()";
    }
    
    ~AsyncTraverser()
    {
//      qDebug() << "~AsyncTraverser()";
    }
    
    void process( DocEntry *entry )
    {
      qDebug() << mIndent << entry->name();
    }
    
    DocEntryTraverser *createChild( DocEntry * )
    {
//      qDebug() << "AsyncTraverser::childTraverser()";
      return new AsyncTraverser( mIndent + "  " );
    }

  private:
    QString mIndent;
};

int main(int argc,char **argv)
{
  KAboutData aboutData("testmetainfo", 0, i18n("TestDocMetaInfo"),"0.1");
  // KComponentData componentData(&aboutData); doesn't seem to be necessary
  QCoreApplication app(argc,argv);

  qDebug() << "Scanning Meta Info";

  DocMetaInfo::self()->scanMetaInfo( );

  qDebug() << "My TRAVERSE start";
  MyTraverser t;
  DocMetaInfo::self()->startTraverseEntries( &t );
  qDebug() << "My TRAVERSE end";

  qDebug() << "Linear TRAVERSE start";
  LinearTraverser l;
  DocMetaInfo::self()->startTraverseEntries( &l );
  qDebug() << "Linear TRAVERSE end";

  qDebug() << "Async TRAVERSE start";
  AsyncTraverser a;
  DocMetaInfo::self()->startTraverseEntries( &a );
  qDebug() << "Async TRAVERSE end";

  return 0;
}
// vim:ts=2:sw=2:et
