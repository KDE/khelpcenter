/*
 *  khc_history.h - part of the KDE Help Center
 *
 *  Copyright (c) 199 Matthias Elter (me@kde.org)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __khc_history_h__
#define __khc_history_h__

#include <qlist.h>
#include <qstring.h>

class khcHistoryItem
{
 public:
  khcHistoryItem();
  khcHistoryItem(const char* url);
  khcHistoryItem(const char* url, long xOffset, long yOffset);
  ~khcHistoryItem();

  QString url() { return m_url; }
  long xOffset() { return m_xOffset; }
  long yOffset() { return m_yOffset; }
    
 protected:
  QString m_url;
  long m_xOffset, m_yOffset;
};

class khcHistory
{
public:
  khcHistory();
  ~khcHistory();

  void append(khcHistoryItem *item);
  void setCurrent(khcHistoryItem *item);

  khcHistoryItem* current() { return m_current; }
  khcHistoryItem* next();
  khcHistoryItem* prev();

  QList<khcHistoryItem> backList();
  QList<khcHistoryItem> forwardList();
      
protected:
  QList<khcHistoryItem> m_list;
  khcHistoryItem *m_current;
};



#endif
