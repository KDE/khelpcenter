/*
 *  manparser.h - part of the KDE Help Center
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

#ifndef __manparser_h__
#define __manparser_h__

#include <stdlib.h>
#include <fstream.h>
#include <qstring.h>

#ifndef MAN_SUCCESS
#define MAN_SUCCESS			1		// the return value from successful man
#endif

#define MAN_SECTIONS		"1:2:3:4:5:6:7:8:9:n"
#define MAN_ENV			    "MANPATH"
#define MAN_MAXSECTIONS		20
#define MAN_MAXPATHS		50

#define MAN_POSSECTION		-1
#define MAN_POSDIR		    -2

#define MAN_DIRECTORY		"index"

class ManPage
{
 public:
    ManPage(const char *theName) { name = theName; _next = NULL; }
    ManPage() {name = ""; _next = NULL; }

    const char *getName() const { return name; }

    ManPage *next() const { return _next; }
    void setNext(ManPage *n) { _next = n; }

 private:
    QString name;
    ManPage *_next;
};

class ManSection
{
 public:
    ManSection(const char *theName);
    ~ManSection();

    int  getNumPages()	{ return numPages; }
    const char *getName()	{ return name; }
    const char *getDesc()	{ return desc; }

    ManPage *moveToHead() {	return curr = head; }
    ManPage *getNext()	{ return curr = curr->next(); }
    ManPage *getCurrent()	{ return curr; }

    void ReadSection();
    void ReadDir(const char *dirName);
    void AddPage(const char *pageName);
    ManPage *FindPage(const char *pageName);

 private:
    ManPage *head, *tail, *curr;
    QString name;
    QString desc;

    int		numPages;
    char	isRead;

    static QString searchPath[MAN_MAXPATHS];
    static int	numPaths;
    static int  sectCount;
};

class ManParser
{
 public:
    ManParser();
    ~ManParser();

    int  readLocation(const char *name);
    const char *getLocation() {	return posString; }
    const char *getPage() { return HTMLPage; }

 public:
    QString HTMLPage;
    QString posString;
    char	staticBuffer[80];
    int		pos;

 private:
    static int instance;
};

#endif

