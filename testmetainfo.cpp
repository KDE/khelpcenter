#include <qstringlist.h>

#include <kaboutdata.h>
#include <kapplication.h>
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
      kdDebug() << mIndent << entry->name() << " - WEIGHT: " << entry->weight()
                << endl;
#if 0
      if ( entry->parent() ) kdDebug() << mIndent << "  PARENT: "
                                       << entry->parent()->name() << endl;
      if ( entry->nextSibling() ) kdDebug() << mIndent << "  NEXT: "
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
      kdDebug() << "PROCESS: " << entry->name() << endl;
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
//      kdDebug() << "AsyncTraverser()" << endl;
    }
    
    ~AsyncTraverser()
    {
//      kdDebug() << "~AsyncTraverser()" << endl;
    }
    
    void process( DocEntry *entry )
    {
      kdDebug() << mIndent << entry->name() << endl;
    }
    
    DocEntryTraverser *createChild( DocEntry * )
    {
//      kdDebug() << "AsyncTraverser::childTraverser()" << endl;
      return new AsyncTraverser( mIndent + "  " );
    }

  private:
    QString mIndent;
};

int main(int argc,char **argv)
{
  KAboutData aboutData("testmetainfo","TestDocMetaInfo","0.1");
  KCmdLineArgs::init(argc,argv,&aboutData);

  KApplication app;

  kdDebug() << "Scanning Meta Info" << endl;

  QStringList langs;
  langs << "en";
//  langs << "de";

  DocMetaInfo::self()->scanMetaInfo( langs );

  kdDebug() << "My TRAVERSE start" << endl;
  MyTraverser t;
  DocMetaInfo::self()->startTraverseEntries( &t );
  kdDebug() << "My TRAVERSE end" << endl;

  kdDebug() << "Linear TRAVERSE start" << endl;
  LinearTraverser l;
  DocMetaInfo::self()->startTraverseEntries( &l );
  kdDebug() << "Linear TRAVERSE end" << endl;

  kdDebug() << "Async TRAVERSE start" << endl;
  AsyncTraverser a;
  DocMetaInfo::self()->startTraverseEntries( &a );
  kdDebug() << "Async TRAVERSE end" << endl;
}
// vim:ts=2:sw=2:et
