#ifndef SCOPEITEM_H
#define SCIPEITEM_H

#include <qlistview.h>

#include <docmetainfo.h>

class ScopeItem : public QCheckListItem
{
  public:
    ScopeItem( QListView *parent, DocEntry *entry )
      : QCheckListItem( parent, entry->name(), QCheckListItem::CheckBox ),
        mEntry( entry ) {}

    ScopeItem( QListViewItem *parent, DocEntry *entry )
      : QCheckListItem( parent, entry->name(), QCheckListItem::CheckBox ),
        mEntry( entry ) {}

    DocEntry *entry() { return mEntry; }
    
    int rtti() const { return rttiId(); }

    static int rttiId() { return 734678; }

  private:
    DocEntry *mEntry;
};

#endif
