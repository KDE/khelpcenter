/*
 *  khc_mansearch.h - part of the KDE Help Center
 *
 *  Copyright (c) 1999 Matthias Elter (me@kde.org)
 *
 *  based on kdehelp code (c) Martin R. Jones
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

#include "khc_searchmatch.h"

#ifndef __khc_mansearch_h__
#define __khc_mansearch_h__

class ManSearch
{
 public:
  ManSearch(MatchList *list) { matchList = list;}
  void search(const char *query);
   
 private:
  const char *FindXRef( const char *theText );

 private:
  MatchList *matchList;
};
#endif


