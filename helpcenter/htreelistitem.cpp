#include "htreelistitem.h"

#include <qstring.h>
#include <kapp.h>

HTreeListItem::HTreeListItem(const char *theText = 0, const QPixmap *thePixmap = 0)
  : KTreeListItem(theText, thePixmap)
{
  url = "";
}

void HTreeListItem::setURL(QString _url)
{
  url = _url;
}
