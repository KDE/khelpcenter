/*
 *  htmlsearch.cpp - part of the KDE Help Center
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

#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>
#include <kapp.h>

#include <stdlib.h>

#include "htmlsearch.h"

QString HTMLSearch::readTitle(const char *filename)
{
  QString title;
  QFile file(filename);
  
  if (file.open(IO_ReadOnly))
	{
	  QTextStream stream(&file);
	  QString buffer;
	  int pos;
	  
	  do
		{
		  buffer = stream.readLine();
		  if (stream.eof())
			return filename;
		}
	  while ((pos = buffer.find("<TITLE>", 0, FALSE)) < 0);
	  
	  title = buffer.right(buffer.length() - pos - 7);

	  if ((pos = title.find("</TITLE>", 0, FALSE )) > 0)
		title.truncate(pos);
	  else
		{
		  do
			{
			  buffer = stream.readLine();
			  title += buffer;
			  if (stream.eof())
				return title;
			}
		  while ((pos = buffer.find("</TITLE>", 0, FALSE)) < 0);
		  
		  if ((pos = title.find("</TITLE>", 0, FALSE)) > 0)
			title.truncate(pos);
		}
	}
  return title;
}

int HTMLSearch::countOccurrences(const char *filename, const char *str)
{
  int count = 0;

  QString cmd = "grep -i -c ";	// -i ignore case, -c count occurrences
  cmd += str;
  cmd += ' ';
  cmd += filename;

  FILE *fp = popen(cmd, "r");

  if (fp)
	{
	  char buffer[80];
	  
	  fgets(buffer, 80, fp);
	  count = atoi(buffer);
	  
	  while (!feof(fp)) fgetc(fp);
	  
	  pclose(fp);
	}
  return count;
}

int HTMLSearch::processFiles(const char *dirname, const char *query)
{
  QDir files(dirname, "*.html", 0, QDir::Files | QDir::Readable);

  if (!files.exists())
	return 0;
	
  QStringList fileList = files.entryList();
  QStringList::Iterator itFile;

  for ( itFile = fileList.begin(); !itFile->isNull(); ++itFile)
	{
	  QString filename = dirname;
	  filename += "/";
	  filename += *itFile;
	  
	  int weight = countOccurrences(filename, query);
 
	  if (weight > 1)
		{
		  QString url = "file:";
		  url += filename;

		  QString title = readTitle(filename);

		  Match *match = new Match(title, url);
		  match->setWeight(weight);
		  matchList->inSort(match);
		}
	}
	return 1;
}

int HTMLSearch::processDir(const char *dirname, const char *query)
{
  QDir dir(dirname, "*", 0, QDir::Dirs);

  if (!dir.exists())
	return 0;
	
  QStringList dirList = dir.entryList();
  QStringList::Iterator itDir;

  for ( itDir = dirList.begin(); !itDir->isNull(); ++itDir)
	{
	  if (itDir->at(0) == '.')
		continue;

	  QString filename = dirname;
	  filename += "/";
	  filename += *itDir;

	  processFiles(filename, query);
	  processDir(filename, query);
	}
  return 1;
}

void HTMLSearch::search (const char *query)
{
  QString dir = KApplication::kde_htmldir();
  dir += "/en/"; //change this for i18n!!!
  processDir(dir, query);
}

