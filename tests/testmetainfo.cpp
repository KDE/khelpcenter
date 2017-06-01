#include <QCoreApplication>
#include <QDebug>

#include "docmetainfo.h"
#include "docentrytraverser.h"

using namespace KHC;

class MyTraverser : public DocEntryTraverser
{
  public:
    MyTraverser( const QByteArray &indent = "" ) : mIndent( indent ) {}

    void process( DocEntry *entry ) Q_DECL_OVERRIDE
    {
      qDebug() << mIndent.constData() << entry->name() << " - WEIGHT: " << entry->weight();
#if 0
      if ( entry->parent() ) qDebug() << mIndent << "  PARENT: "
                                       << entry->parent()->name();
      if ( entry->nextSibling() ) qDebug() << mIndent << "  NEXT: "
                                       << entry->nextSibling()->name();
#endif
    }
  
    DocEntryTraverser *createChild( DocEntry * ) Q_DECL_OVERRIDE
    {
      return new MyTraverser( mIndent + "  " );
    }

  private:
    QByteArray mIndent;
};

class LinearTraverser : public DocEntryTraverser
{
  public:
    void process( DocEntry *entry ) Q_DECL_OVERRIDE
    {
      qDebug() << "PROCESS: " << entry->name();
    }
    
    DocEntryTraverser *createChild( DocEntry * ) Q_DECL_OVERRIDE
    {
      return this;
    }
    
    DocEntryTraverser *parentTraverser() Q_DECL_OVERRIDE
    {
      return this;
    }
    
    void deleteTraverser() Q_DECL_OVERRIDE {}
};

class AsyncTraverser : public DocEntryTraverser
{
  public:
    AsyncTraverser( const QByteArray &indent = "" ) : mIndent( indent )
    {
//      qDebug() << "AsyncTraverser()";
    }
    
    ~AsyncTraverser()
    {
//      qDebug() << "~AsyncTraverser()";
    }
    
    void process( DocEntry *entry ) Q_DECL_OVERRIDE
    {
      qDebug() << mIndent.constData() << entry->name();
    }
    
    DocEntryTraverser *createChild( DocEntry * ) Q_DECL_OVERRIDE
    {
//      qDebug() << "AsyncTraverser::childTraverser()";
      return new AsyncTraverser( mIndent + "  " );
    }

  private:
    QByteArray mIndent;
};

int main(int argc,char **argv)
{
  QCoreApplication app(argc,argv);
  QCoreApplication::setApplicationName( "khelpcenter" );

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
