#include "docmetainfo.h"

#include "docentrytraverser.h"

using namespace KHC;

void DocEntryTraverser::setNotifyee( DocMetaInfo *n )
{
  mNotifyee = n;
}

void DocEntryTraverser::startProcess( DocEntry *entry )
{
  process( entry );
  mNotifyee->endProcess( entry, this );
}

DocEntryTraverser *DocEntryTraverser::childTraverser( DocEntry *parentEntry )
{
  DocEntryTraverser *child = createChild( parentEntry );
  if ( child != this ) {
    child->mParent = this;
    child->mNotifyee = mNotifyee;
  }
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

void DocEntryTraverser::setParentEntry( DocEntry *entry )
{
  mParentEntry = entry;
}

DocEntry *DocEntryTraverser::parentEntry()
{
  return mParentEntry;
}
