/*
    This file is part of the KDE libraries

    Copyright (C) 1998 René Beutler (rbeutler@g26.ethz.ch)
    this file is taken from kdehelp
    modified by René Beutler

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#include <stdlib.h>
#include <ctype.h>
#include <qdir.h>
#include <qfile.h>
#include <qtstream.h>
#include <qstring.h>
#include <qlist.h>
#include <kapp.h>
#include "htmlsrch.h"

void readTags( const char *filename, QString &title, QString &keywords );

class Match
{
public:
	Match( const char *f )
		{	
			QString keyws;
			occurences = 0; filename = f;
			readTags( f, title, keyws );

			// parse QString(keyws) to a list of keywords
			int pos = 0, lastPos = 0;
			do {
				pos = keyws.find( ',', pos );
				if( pos != -1 )
				{
					keywords.append( keyws.mid( lastPos, pos-lastPos ).stripWhiteSpace() );
					pos++;	// skip on char
					lastPos = pos;
				} else
					// don't forget the last one
					keywords.append( keyws.mid( lastPos, keyws.length()-lastPos ).stripWhiteSpace() );			
			} while( pos != -1 );
		}

	bool occurs( QString s )
		{
			return ( keywords.contains( s ) > 0 ) ? TRUE : FALSE;
		}
	
	void inc()
		{	occurences++; }

	const char *getFilename()
		{	return filename; }

	const char *getTitle()
		{	return title; }

	int getOccurences()
		{	return occurences; }

protected:
	QString filename;
	QString title;
	QStrList keywords;
	int occurences;
};

class MatchList : public QList<Match>
{
protected:
	virtual int compareItems( Item m1, Item m2 )
	{
		if ( ((Match *)m1)->getOccurences() > ((Match *)m2)->getOccurences() )
			return -1;
		return 1;
	}
};

void readTags( const char *filename, QString &title, QString &keywords )
{
	// ----> ATTENTION: this routine assumes, that <TITLE> and </TITLE> resp.
	//                  <META NAME="Keywords" CONTENT=".."> are on on a single line

	QFile file( filename );
	long tStart, tEnd, kStart, kEnd;
	bool titleDone = false, keysDone = false;

	if ( file.open( IO_ReadOnly ) )
	{
		QTextStream stream( &file );
		QString buffer, buf;

		do
		{
			buffer = stream.readLine();
			if ( stream.eof() )
			{
				title = filename;
				keywords = "";
				return;
			}

			buf = buffer.simplifyWhiteSpace();

			tStart = buf.simplifyWhiteSpace().find( "<TITLE>", 0, FALSE );
			if( tStart >= 0 )
			{
				tEnd   = buf.find( "</TITLE>", 0, FALSE );
				title = buf.mid( tStart+7, tEnd-tStart-7);
				titleDone = true;
			}
			
			kStart = buf.find( "META NAME=\"KEYWORDS\" CONTENT=\"", 0, FALSE );
			if( kStart >= 0 )
			{
				kEnd   = buf.find( "\">", 0, FALSE );
				keywords = buf.mid( kStart+30, kEnd-kStart-30);
				keysDone = true;
			}

		}
		while ( (titleDone == false) || (keysDone == false) );
	}

//printf("%s : %s\n", (const char *)title, (const char *)keywords); fflush(stdout);
}


int processFiles( MatchList &list, const char *dirname, const char **query )
{
	QDir files( dirname, "*.html", 0, QDir::Files | QDir::Readable );

	if ( !files.exists() )
		return 0;
	
	const QStrList *fileList = files.entryList();

	QStrListIterator itFile( *fileList );

	for ( ; itFile.current(); ++itFile )
	{
		QString filename = dirname;
		filename += "/";
		filename += itFile.current();
		Match *match = new Match( filename );

		for ( int i = 0; query[i]; i++ )
		{
			if( match->occurs( query[i] ) == true )
				match->inc();
		}

		if ( match->getOccurences() > 0 )
			list.inSort( match );
		else
			delete match;
	}

	return 1;
}

int processDir( MatchList &list, const char *dirname, const char **query )
{
	processFiles( list,  dirname, query );

	QDir dir( dirname, "*", 0, QDir::Dirs );
	if ( !dir.exists() )
		return 0;

	const QStrList *dirList = dir.entryList();

	QStrListIterator itDir( *dirList );

	for ( ; itDir.current(); ++itDir )
	{
		if ( itDir.current()[0] == '.' )
			continue;

		QString filename = dirname;
		filename += "/";
		filename += itDir.current();

		processDir( list, filename, query );
	}

	return 1;
}

bool searchHTML( QString dir, const char *search, ostream &stream )
{
	int k = 0;
	const char *query[20];
	char *buffer = new char [ strlen( search ) + 1 ];
	QString s;

	s = QString( search ).simplifyWhiteSpace();
	if( s.isEmpty() )
	{
		stream << "<p>" << i18n("<B>You did not enter a question!</B><BR><BR>");
		stream << i18n("Please enter a question before you press the 'Answer' button.");
		return false;
	}
	
	strcpy( buffer, search );
	query[k] = strtok( buffer, " " );

	while ( query[k] && k < 19 )
	{
		k++;
		query[k] = strtok( NULL, " " );
	}

	if ( k )
	{
		MatchList list;
		list.setAutoDelete( TRUE );

		processDir( list, dir, query );

		if ( list.count() > 0 )
		{
			Match *match;
			stream << "<BIG>&nbsp;";
			stream << list.count() << " ";
			stream << ( (list.count() == 1) ? i18n("match") : i18n("matches") );
			stream << "</BIG><p>";
			stream << "<BLOCKQUOTE>";
			for ( match = list.first(); match; match = list.next() )
			{
                  		s.sprintf("<A HREF=\"file:%s\">%s</A><BR>",
				          match->getFilename(), match->getTitle() );
				stream << s;
			}
			stream << "</BLOCKQUOTE>";
		}
		else
		{
			stream << "<BIG>&nbsp;" << i18n("No Matches") << "</BIG>";
		}
	
	}

	delete [] buffer;

	return 0;
}

