#ifndef SCOPEITEM_H
#define SCOPEITEM_H

#include <qlistview.h>

#include <docmetainfo.h>

namespace KHC {

class ScopeItem : public QCheckListItem
{
  public:
    ScopeItem( QListView *parent, DocEntry *entry )
      : QCheckListItem( parent, entry->name(), QCheckListItem::CheckBox ),
        mEntry( entry ), mObserver( 0 ) {}

    ScopeItem( QListViewItem *parent, DocEntry *entry )
      : QCheckListItem( parent, entry->name(), QCheckListItem::CheckBox ),
        mEntry( entry ), mObserver( 0 ) {}

    DocEntry *entry() { return mEntry; }
    
    int rtti() const { return rttiId(); }

    static int rttiId() { return 734678; }

    class Observer
    {
      public:
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
