/*
 *  mansearch.cpp - part of the KDE Help Center
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

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>

#include "mansearch.h"

#define MAXSECTIONLEN  4
#define NUMSECTIONS    10


void ManSearch::search(const char *search)
{
  char syscmd[256];
  char buffer[1024];
  char tmp[256];
  const char *pos, *ptr;

  FILE *fp;

  sprintf(syscmd, "apropos %s", search);
  fp = popen(syscmd, "r");

  if (!fp)
	return;

  while (!feof(fp))
	{
	  if (fgets(buffer, 1024, fp) == NULL)
		break;

	  if (strchr(buffer, 27))	// escape sequence
		continue;

	  pos = buffer;
	  ptr = FindXRef(pos);	// check for cross reference in this line

	  if (ptr)
		{
		  pos = strchr(ptr, ')') + 1;
		  strncpy(tmp, ptr, pos-ptr);
		  tmp[pos-ptr] = '\0';

		  QString url = "man:/"; 
		  url += tmp;

		  QString text = tmp;
		  QString title = pos;
		  title = title.left(title.length() - 1); // truncate trailing '\n'
		  text += ' ';
		  text += title.stripWhiteSpace();
		 
		  Match *match = new Match(text, url);
		  match->setWeight(2);
		  matchList->inSort(match);
		}
	}
  pclose(fp);
}

const char * ManSearch::FindXRef(const char *theText)
{
  static char *sections[] = { "1", "2", "3", "4", "5", "6",
							  "7", "8", "9", "n" };
  const char *ptr, *ptr1, *xrefPtr;
  int i;
  
  ptr = strchr(theText, '(');

  if (ptr)
	{
	  ptr1 = strchr(ptr, ')');
	  if (ptr1)
		{
		  if ((ptr1-ptr-1 > MAXSECTIONLEN) || (ptr1-ptr <= 1))
			return NULL;

		  for (i = 0; i < NUMSECTIONS; i++)
			{
			  if (!strncmp(ptr+1, sections[i], strlen(sections[i])))
				{
				  xrefPtr = ptr-1;

				  // this allows 1 space between name and '('
				  if (*xrefPtr == ' ') xrefPtr--;
				  if (*xrefPtr == ' ') return NULL;

				  while ((xrefPtr > theText) && (*(xrefPtr-1) != ' '))
					xrefPtr--;

				  return xrefPtr;
				}
			}
		}
	  else
		return NULL;
	}
  return NULL;
}
