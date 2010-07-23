
/* This file is part of the KDE project
 * Copyright 2002 Cornelius Schumacher <schumacher@kde.org>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License version 2 or at your option version 3 as published
 * by the Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
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
    return 0;
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
// vim:ts=2:sw=2:et
