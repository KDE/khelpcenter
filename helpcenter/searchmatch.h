/*
 *  searchmatch.h - part of the KDE Help Center
 *
 *  Copyright (c) 1999 Matthias Elter (me@kde.org)
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

#include <qstring.h>
#include <qlist.h>

#ifndef __searchmatch_h__
#define __searchmatch_h__

class Match
{

public:

  Match(const char* _title, const char* _url, int weight = 0)
	{ title = _title; url = _url; }

  void setWeight(int m) { weight = m; }
  void addWeight(int m) { weight += m; }
  void setTitle(const char * _title) { title = _title; }
  void setURL(const char* _url) { url = _url; }

  const char* getURL() { return url; }
  const char* getTitle() { return title; }
  int getWeight() {	return weight; }
  
 protected:

  QString url, title;
  int weight;
};

class MatchList : public QList<Match>
{
protected:
  virtual int compareItems(Item m1, Item m2)
	{
	  if (((Match *)m1)->getWeight() > ((Match *)m2)->getWeight())
		return -1;
	  return 1;
	}
};
#endif
