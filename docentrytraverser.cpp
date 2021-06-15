/*
    SPDX-FileCopyrightText: 2002 Cornelius Schumacher <schumacher@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only
*/

#include "docentrytraverser.h"
#include "docmetainfo.h"

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
  if (!child)
    return nullptr;
  if ( child != this )
  {
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
// vim:ts=2:sw=2:et
