#ifndef SCOPEITEM_H
#define SCOPEITEM_H

#include <Qt3Support/Q3CheckListItem>

#include "docmetainfo.h"

namespace KHC {

class ScopeItem : public Q3CheckListItem
{
  public:
    ScopeItem( Q3ListView *parent, DocEntry *entry )
      : Q3CheckListItem( parent, entry->name(), Q3CheckListItem::CheckBox ),
        mEntry( entry ), mObserver( 0 ) {}

    ScopeItem( Q3ListViewItem *parent, DocEntry *entry )
      : Q3CheckListItem( parent, entry->name(), Q3CheckListItem::CheckBox ),
        mEntry( entry ), mObserver( 0 ) {}

    DocEntry *entry()const { return mEntry; }

    int rtti() const { return rttiId(); }

    static int rttiId() { return 734678; }

    class Observer
    {
      public:
        virtual ~Observer() {}
        virtual void scopeItemChanged( ScopeItem * ) = 0;
    };

    void setObserver( Observer *o ) { mObserver = o; }

  protected:
    void stateChange ( bool )
    {
      if ( mObserver ) mObserver->scopeItemChanged( this );
    }

  private:
    DocEntry *mEntry;

    Observer *mObserver;
};

}

#endif
// vim:ts=2:sw=2:et
