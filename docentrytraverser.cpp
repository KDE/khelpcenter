#include "docmetainfo.h"

#include "docentrytraverser.h"

void DocEntryTraverser::setNotifyee( DocMetaInfo *n )
{
  mNotifyee = n;
}

void DocEntryTraverser::startProcess( DocEntry *entry )
{
  process( entry );
  mNotifyee->endProcess( entry, this );
}

DocEntryTraverser *DocEntryTraverser::childTraverser()
{
  DocEntryTraverser *child = createChild();
  child->mParent = this;
  return child;
}

DocEntryTraverser *DocEntryTraverser::parentTraverser()
{
  return mParent;
}

void DocEntryTraverser::deleteTraverser()
{
  delete this;
}
