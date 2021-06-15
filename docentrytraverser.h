/*
    SPDX-FileCopyrightText: 2002 Cornelius Schumacher <schumacher@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only
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
