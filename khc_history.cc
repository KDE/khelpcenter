/*
 *  khc_history.cc - part of the KDE Help Center
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

#include "khc_history.h"

khcHistoryItem::khcHistoryItem()
{
  m_url ="";
  m_xOffset = m_yOffset = 0;
}

khcHistoryItem::khcHistoryItem(const char* url)
{
  m_url = url;
  m_xOffset = m_yOffset = 0;
}

khcHistoryItem::khcHistoryItem(const char* url, long xOffset, long yOffset)
{
  m_url = url;
  m_xOffset = xOffset;
  m_yOffset = yOffset;
}

khcHistoryItem::~khcHistoryItem()
{
}

khcHistory::khcHistory()
{
  m_list.setAutoDelete(true);
  m_current = 0L;
}

khcHistory::~khcHistory()
{
  m_list.clear();
}

void khcHistory::append (khcHistoryItem *item)
{
  if (item)
    {
      m_list.append(item);
      m_current = item;
    }
}

void khcHistory::setCurrent(khcHistoryItem *item)
{
  if (item)
    {
      m_list.find(item);
      m_current = item;
    }
}

khcHistoryItem* khcHistory::next()
{
  khcHistoryItem *next = m_list.next();
  if (!next)
    m_list.find(m_current);
  else
    m_current = next;
  return next;
}

khcHistoryItem* khcHistory::prev()
{
  khcHistoryItem *prev = m_list.prev();
  if (!prev)
    m_list.find(m_current);
  else
    m_current = prev;
  return prev;
}

QList<khcHistoryItem> khcHistory::backList()
{
  QList<khcHistoryItem> list;
  khcHistoryItem *item = m_list.prev();

  while(item)
    {
      list.append(item);
      item = m_list.prev();
    }
  
  m_list.find(m_current);
  return list;
}

QList<khcHistoryItem> khcHistory::forwardList()
{
  QList<khcHistoryItem> list;
  khcHistoryItem *item = m_list.next();

  while(item)
    {
      list.append(item);
      item = m_list.next();
    }
  
  m_list.find(m_current);
  return list;
}
