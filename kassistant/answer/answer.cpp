/*
    This file is part of the KDE libraries

    Copyright (C) 1998 René Beutler (rbeutler@g26.ethz.ch)

    Very simple documentation search
    taken from kdehelp

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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <ctype.h>
#include <qstring.h>
#include <kapp.h>
#include <iostream.h>
#include "htmlsrch.h"

const char *parseQuery( const char *str, QString &name, QString &value );
QString decodeQuery( QString &str );

int main( int argc, char *argv[] )
{
	KApplication app( argc, argv );

	const char *qs = getenv( "QUERY_STRING" );

	if ( qs == 0L )
		return 1;

	const char *str = qs;
	QString name, value;

	QString search = "";

	do
	{
		str = parseQuery( str, name, value );

		if ( strncasecmp( name, "Search", 7 ) == 0 )
		{
			search = value;
		}
/*
		if ( strncasecmp( name, "Category", 8 ) == 0 )
		{
			if ( strncasecmp( value, "KDE", 3 ) == 0 )
				HTML = true;
			else if ( strncasecmp( value, "man", 3 ) == 0 )
				man = true;
			else if ( strncasecmp( value, "info", 3 ) == 0 )
				info = true;
		}
*/
	}
	while ( str );

	cout << "<HTML><HEAD><TITLE>Q: " << search;
///	cout << "</TITLE></HEAD><BODY BGCOLOR=\"#FFFFFF\">" << endl;
	cout << "</TITLE></HEAD><BODY><BR>" << endl;
	cout << "<FORM METHOD=\"GET\" ACTION=\"/cgi-bin/answer\">" << endl;
	cout << "<INPUT TYPE=TEXT NAME=Search VALUE=\"" << search << "\" SIZE=30><BR><BR>" << endl;
	cout << "<INPUT TYPE=SUBMIT VALUE=\"" << i18n("Answer!") << "\">&nbsp;";
	cout << "<INPUT TYPE=RESET VALUE=\"" << i18n("Reset") << "\">" << endl;
	cout << "<p><HRULE><p><BIG>" << i18n("Search Results:") << "</BIG>" << endl;

	QString dir = KApplication::kde_htmldir() + "/default/khelpcenter";

	searchHTML( dir, search, cout );

	cout << "</BODY></HTML>" << endl;

	return 0;
}

const char *parseQuery( const char *str, QString &name, QString &value )
{
	const char *end = strchr( str, '&' );

	name = value = "";

	QString tmp, field = str;

	if ( end )
	{
		field.truncate( end - str );
		end++;
	}

	int pos;

	if ( ( pos = field.find( '=' ) ) > 0 )
	{
		tmp = field.left( pos );
		name = decodeQuery( tmp );
		tmp = field.right( field.length() - pos - 1 );
		value = decodeQuery( tmp );
	}

	return end;
}

QString decodeQuery( QString &str )
{
	unsigned pos = 0;
	QString decoded = "";

	do
	{
		unsigned char c = str[pos];

		if ( c == '+' )
			decoded += ' ';
		else if ( c == '%' )
		{
			char num[3];
			num[0] = str[++pos];
			num[1] = str[++pos];
			num[2] = '\0';
			int val = strtol( num, NULL, 16 );
			if( !ispunct( (unsigned char) val ) )
				decoded += (unsigned char)val;
		}
		else
			decoded += c;
		pos++;
	}
	while ( pos < str.length() );

	return decoded;
}

