/*
 *  manparser.cpp - part of the KDE Help Center
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
#include <dirent.h>

#ifdef HAVE_PATHS_H
#include <paths.h>
#endif

#ifndef _PATH_TMP
#define _PATH_TMP "/tmp"
#endif

#include "manparser.h"
#include "error.h"

#include <kapp.h>

#define MAXSECTIONLEN	4

// Is this a safe command to issue via system, i.e. has a naughty user inserted
// special shell characters in a URL?
bool safeCommand( const char *cmd )
{
  if (strpbrk(cmd, "&;`'\\\"|*?~<>^()[]{}$\n\r"))
	return false;
  return true;
}

// this is an array of sections available
// the first instance of ManParser creates these sections
// as a section is entered the contents are read (once only).
static ManSection	*sections[MAN_MAXSECTIONS];
static int numSections = 0;

// creates a list of available pages in a section
int Read(ManSection &sect, QString &page )
{
  QString buffer;
  sect.moveToHead();

  while (sect.getCurrent())
	{
	  buffer = sect.getCurrent()->getName();
	  buffer += "(";
	  buffer += sect.getName();
	  buffer += ")";
	  page += "<cell width=200>&nbsp;";
	  page += "<A HREF=man:/" + buffer + ">";
	  page += sect.getCurrent()->getName();
	  page += "</A>";
	  page += "</cell>";
	  sect.getNext();
	} 
  return 0;
}

// maintain a list of man pages available in a section
QString ManSection::searchPath[MAN_MAXPATHS];
int ManSection::numPaths = 0;
int ManSection::sectCount = 0;

ManSection::ManSection(const char *theName)
{
	char *envPath;
	name = theName;
	numPages = 0;
	isRead = 0;
	head = tail = NULL;

    sectCount++;

	if (strncmp(theName, "1", 1) == 0)
	  desc = i18n("User Commands");
	else if (strncmp(theName, "2", 1) == 0)
	  desc = i18n("System Calls");
	else if (strncmp(theName, "3", 1) == 0)
	  desc = i18n("Subroutines");
	else if (strncmp(theName, "4", 1) == 0)
	  desc = i18n("Devices");
	else if (strncmp(theName, "5", 1) == 0)
	  desc = i18n("File Formats");
	else if (strncmp(theName, "6", 1) == 0)
	  desc = i18n("Games");
	else if (strncmp(theName, "7", 1) == 0)
	  desc = i18n("Miscellaneous");
	else if (strncmp(theName, "8", 1) == 0)
	  desc = i18n("System Administration");
	else if (strncmp(theName, "9", 1) == 0)
	  desc = i18n("Kernel");
	else if (strncmp(theName, "n", 1) == 0)
	  desc = i18n("New");
	else
	  desc = "";

    if (numPaths == 0)
    {
	  if ((envPath = getenv(MAN_ENV)))
        {
		  QString paths = envPath;
		  QString p = strtok(paths, ":");

		  while (p)
            {
			  searchPath[numPaths++] = p;
			  p = strtok(NULL, ":");
            }
        }
	  else
        {
#ifndef __FreeBSD__
		  searchPath[numPaths++] = "/usr/man";
#else
		  searchPath[numPaths++] = _PATH_MAN;
		  searchPath[numPaths++] = "/usr/X11R6/man";
#endif
		  searchPath[numPaths++] = "/usr/local/man";
        }
    }
}

ManSection::~ManSection()
{
  ManPage *tmp;
  int i;
  
  delete [] name;
  sectCount--;

  if (sectCount == 0)
    {
	  for (i = 0; i < numPaths; i++)
		delete [] (searchPath[i]);
	  numPaths = 0;
    }
  
  curr = head;
  while (curr)
	{
	  tmp = curr;
	  curr = curr->next();
	  delete tmp;
	}
}

// read a list of pages in the section
void ManSection::ReadSection()
{
  //char buffer[256];
  QString buffer;
  int i;
  DIR *dir;
  struct dirent *dirEntry;
  
  if (!isRead)		// only read once
	isRead = 1;
  else
	return;
  
  for (i = 0; i < numPaths; i++)
	{
	  dir = opendir(searchPath[i]);
	  
	  if (dir == NULL)
		return;

	  while ((dirEntry = readdir(dir))) // check for matching cat and man dir
		{
		  if (!strncmp(dirEntry->d_name, "man", 3))
			{
			  if (!strcmp(strtok(dirEntry->d_name+3, "."), name))
				{
				  buffer = searchPath[i];
				  buffer += "/";
				  buffer += dirEntry->d_name;
				  ReadDir(buffer);
				}
			}
		  else if (!strncmp(dirEntry->d_name, "cat", 3))
			{
			  if (!strcmp(strtok(dirEntry->d_name+3, "."), name))
				{
				  buffer = searchPath[i];
				  buffer += "/";
				  buffer += dirEntry->d_name;
				  ReadDir(buffer);
				}
			}
		}
	  closedir(dir);
	}
}

// read the contents of a directory and add to the list of pages
void ManSection::ReadDir(const char *dirName )
{
  DIR *dir;
  struct dirent *dirEntry;
  char *ptr;
  char buffer[256];
  
  cout << i18n("Reading Dir: ") << dirName << endl;
  
  dir = opendir(dirName);
  
  if ( dir )
    {
	  while ( (dirEntry = readdir(dir)) )
        {
		  if ( dirEntry->d_name[0] == '.' )
			continue;
		  
		  strcpy( buffer, dirEntry->d_name );
		  
		  ptr = strrchr( buffer, '.' );
		  if ( !strcmp(ptr, ".gz") || !strcmp(ptr, ".Z") ) // skip compress extn
            {
			  *ptr = '\0';
			  ptr = strrchr( buffer, '.' );
            }
		  if (ptr)
            {
			  *ptr = '\0';
			  AddPage( buffer );
            }
        }
	  closedir(dir);
    }
}

// add a page to the list
void ManSection::AddPage(const char *pageName)
{
  ManPage *page, *tmp;
  curr = head;
  int result;
  
  // insert page in alphabetical order
  if ( head )
	{
	  result = strcmp(head->getName(), pageName);
	  if ( result > 0 )
		{
		  page = new ManPage(pageName);
		  page->setNext( head );
		  head = page;
		  return;
		}
	  else if ( result == 0 )
		return;
	  
	  while ( curr->next() )
		{
		  int result = strcmp( curr->next()->getName(), pageName );
		  if ( result < 0 )
			curr = curr->next();
		  else if ( result == 0)
			return;
		  else
			{
			  page = new ManPage( pageName );
			  tmp = curr->next();
			  curr->setNext( page );
			  page->setNext( tmp );
			  return;
			}
		}
	  page = new ManPage( pageName );
	  curr->setNext( page );
	}
  else
	{
	  page = new ManPage( pageName );
	  head = tail = page;
	}
}

int ManParser::instance = 0;

ManParser::ManParser()
{
  char sectList[80];
  char *sects = getenv("MANSECT");
  
  if (sects)
	strcpy(sectList, sects);
  else
	strcpy(sectList, MAN_SECTIONS);
  
  sectList[ strlen( sectList ) + 1 ] = '\0';
  
  
  // create the sections (this does not read the contents of a section.
  // This is done only when it is requested).
  // we do this only for the first instance
  if (instance == 0)
	{
	  numSections = 0;
	  
	  // get the manual sections
	  // I'd like to use strtok here, but its used in ManSection
	  // constructor (strtok annoys me).
	  char *s = sectList;
	  char *e = strchr(sectList, ':');
	  while (*s != '\0')
		{
		  if (e) *e = '\0';
		  sections[numSections++] = new ManSection(s);
		  s += strlen(s)+1;
		  e = strchr(s, ':');
		}
	}
  instance++;
  pos = MAN_POSSECTION;
}

ManParser::~ManParser()
{
  instance--;
  if (instance == 0)
	{
	  for (int i = 0; i < numSections; i++)
		delete sections[i];
	}
}

// ----------------------------------------------------------------------------
// read the specified page
// formats allowed
// MAN_DIRECTORY	reads a list of sections
// (sec)			reads a list of pages in 'sections'
// page(sec)		reads the specified page from 'sections'
// page				reads the specified page
//
int ManParser::readLocation(const char *name)
{
  char tmpName[80];
  int i = 0;
  
  strcpy(tmpName, name);
  
  if (*tmpName == '(')				// read a list of pages in this section
	{
	  strtok(tmpName+1, ")");
	  for (i = 0; i < numSections; i++)
		{
		  if (!strcmp(tmpName+1, sections[i]->getName()))
			{
			  sections[i]->ReadSection();
			  Read(*(sections[i]), HTMLPage);
			  posString = i18n("Unix man pages - Section");
			  posString += sections[i]->getName();
			  pos = MAN_POSSECTION;
			  return 0;
			}
		}
	  return 1;
	}
  else									// read the specified page
	{
	  char stdFile[256];
	  char errFile[256];
	  char sysCmd[256];
	  char rmanCmd[256];
	  char *ptr;
	  
	  sysCmd[0] = '\0';
	  
	  sprintf(stdFile, "%s/khelpcenterXXXXXX", _PATH_TMP);	// temp file
	  mktemp(stdFile);
	  
	  sprintf(errFile, "%s/khelpcenterXXXXXX", _PATH_TMP);	// temp file
	  mktemp(errFile);
	  
	  sprintf(rmanCmd, "%s/rman -f HTML",
			  (const char *)KApplication::kde_bindir());
	  
	  // create the system cmd to read the man page
	  if ( (ptr = strchr(tmpName, '(')) )
		{
		  if (!strchr(tmpName, ')')) return 1;	// check for closing )
		  *ptr = '\0';
		  ptr++;
		  for (i = 0; i < numSections; i++)	// read which section?
			{
			  if (!strncmp(ptr, sections[i]->getName(),
						   strlen(sections[i]->getName())))
				{
				  pos = i;
				  break;
				}
			}
		  if ( safeCommand( sections[pos]->getName() ) &&
			   safeCommand( tmpName ) )
            {
			  sprintf(sysCmd, "man %s %s < /dev/null 2> %s | %s > %s",
					  sections[pos]->getName(),
					  tmpName, errFile, rmanCmd, stdFile );
            }
		}
	  else if ( safeCommand( tmpName ) )
		{
		  sprintf(sysCmd, "man %s < /dev/null 2> %s | %s > %s",
				  tmpName, errFile, rmanCmd, stdFile);
		}
	  
	  if ( sysCmd[0] == '\0' )
        {
		  Error.Set(ERR_WARNING, i18n("\"man\" system call failed"));
		  return -1;
        }
	  
	  // call 'man' to read man page
	  int status = system(sysCmd);
	  
	  if (status < 0)			// system returns -ve on failure
		{
		  Error.Set(ERR_WARNING, i18n("\"man\" system call failed"));
		  return 1;
		}
	  
	  // open the man page and parse it
	  ifstream stream(stdFile);
	  
	  if (stream.fail())
		{
		  Error.Set(ERR_FATAL, i18n("Opening temporary file failed"));
		  return 1;
		}
	  
	  // if this file is very short assume the man call failed
	  stream.seekg( 0, ios::end );
	  if ( stream.tellg() < 5 )
		{
		  stream.close();
		  stream.open( errFile );
		}
	  stream.seekg( 0, ios::beg );
	  
	  char buffer[256];
	  HTMLPage = "";
	  
	  while ( !stream.eof() )
		{
		  stream.getline( buffer, 256 );
		  HTMLPage += buffer;
		  if ( HTMLPage.right(1) == "-" )
			HTMLPage.truncate( HTMLPage.length() - 1 );
		  else
			HTMLPage.append(" ");
		}
	  
	  stream.close();
	  posString = name;
	  
	  remove(stdFile);	// don't need tmp file anymore
	  remove(errFile);	// don't need tmp file anymore
	}
  return 0;
}
