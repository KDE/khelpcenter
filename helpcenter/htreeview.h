#ifndef HTREEVIEW_H_
#define HTREEVIEW_H_

#include <qlist.h>

#include <ktreelist.h>
#include <ktabctl.h>

#include "htreelistitem.h"

class HTreeView : public KTabCtl
{
  Q_OBJECT
  
public:
  	HTreeView(QWidget *parent=0, const char *name=0);
	virtual ~HTreeView();
signals:
	void itemSelected(QString itemURL);
public slots:
	void slotItemSelected(int index); 
private:
	void addTreeItems();
	KTreeList *tree;
	QWidget *search;
	QList<HTreeListItem> itemList;
};


#endif
