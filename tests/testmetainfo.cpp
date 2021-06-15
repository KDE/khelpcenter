/*
    SPDX-FileCopyrightText: 2007 Andreas Pakulat <apaku@gmx.de>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <QCoreApplication>
#include <QDebug>

#include "docmetainfo.h"
#include "docentrytraverser.h"

using namespace KHC;

class MyTraverser : public DocEntryTraverser
{
  public:
    MyTraverser( const QByteArray &indent = "" ) : mIndent( indent ) {}

    void process( DocEntry *entry ) override
    {
      qDebug() << mIndent.constData() << entry->name() << " - WEIGHT: " << entry->weight();
#if 0
      if ( entry->parent() ) qDebug() << mIndent << "  PARENT: "
                                       << entry->parent()->name();
      if ( entry->nextSibling() ) qDebug() << mIndent << "  NEXT: "
                                       << entry->nextSibling()->name();
#endif
    }
  
    DocEntryTraverser *createChild( DocEntry * ) override
    {
      return new MyTraverser( mIndent + "  " );
    }

  private:
    QByteArray mIndent;
};

class LinearTraverser : public DocEntryTraverser
{
  public:
    void process( DocEntry *entry ) override
    {
      qDebug() << "PROCESS: " << entry->name();
    }
    
    DocEntryTraverser *createChild( DocEntry * ) override
    {
      return this;
    }
    
    DocEntryTraverser *parentTraverser() override
    {
      return this;
    }
    
    void deleteTraverser() override {}
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
    
    void process( DocEntry *entry ) override
    {
      qDebug() << mIndent.constData() << entry->name();
    }
    
    DocEntryTraverser *createChild( DocEntry * ) override
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
  QCoreApplication::setApplicationName( QStringLiteral("khelpcenter") );

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
