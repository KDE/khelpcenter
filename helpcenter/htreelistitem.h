#ifndef HTREELISTITEM_H_
#define HTREELISTITEM_H_

#include <ktreelist.h>

class HTreeListItem : public KTreeListItem
{
public:
  HTreeListItem(const char *theText = 0, const QPixmap *thePixmap = 0);
  void setURL(QString _url);
  QString getURL() { return url; }
private:
  QString url;
};

#endif
