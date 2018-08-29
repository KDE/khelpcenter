
/* This file is part of the KDE project
   Copyright 2002 Cornelius Schumacher <schumacher@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License version 2 or at your option version 3 as published
   by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KHC_DOCENTRYTRAVERSER_H
#define KHC_DOCENTRYTRAVERSER_H

namespace KHC {

  class DocEntry;
  class DocMetaInfo;

  class DocEntryTraverser
  {
    public:
      DocEntryTraverser(){}
      virtual ~DocEntryTraverser() {}
      
      void setNotifyee( DocMetaInfo * );

      virtual void process( DocEntry * ) = 0;

      virtual void startProcess( DocEntry * );

      virtual DocEntryTraverser *createChild( DocEntry *parentEntry ) = 0;

      virtual void deleteTraverser();

      virtual void finishTraversal() {}

      DocEntryTraverser *childTraverser( DocEntry *parentEntry );
      virtual DocEntryTraverser *parentTraverser();

      void setParentEntry( DocEntry * );
      DocEntry *parentEntry();

    protected:
      DocMetaInfo *mNotifyee = nullptr;
      DocEntryTraverser *mParent = nullptr;

    private:
      DocEntry *mParentEntry = nullptr;
  };

}

#endif //KHC_DOCENTRYTRAVERSER_H
// vim:ts=2:sw=2:et
