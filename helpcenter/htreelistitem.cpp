/*
 *  htreelistitem.cpp - part of the KDE Help Center
 *
 *  Copyright (C) 1999 Matthias Elter (me@kde.org)
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
#include <qfile.h>

#include <kapp.h>
#include <ksimpleconfig.h>

#include "htreelistitem.h"

HTreeListItem::HTreeListItem(const QString& _text, const QString& _miniicon)
    : KTreeListItem()
{
    name = _text;
    miniicon = _miniicon;
    setDrawTree(false);
    setDrawExpandButton(false);
    setIndent(0);
}

bool HTreeListItem::readKDElnk ( const char *filename )
{
    QFile file(filename);
    if (!file.open(IO_ReadOnly))
	return false;

    file.close(); 

    KSimpleConfig config( filename, true );
    config.setDesktopGroup();

    // read document url
    QString path = config.readEntry("DocPath");
    if (path.isNull())
	return false;

    url = kapp->kde_htmldir() + "/default/";
    url += path;

    // read comment text
    info = config.readEntry("Info");
    if (info.isNull())
	info = config.readEntry("Comment");

    // read icon and miniicon
    //icon = config.readEntry("Icon");
    miniicon = "helpdoc.xpm";//config.readEntry("MiniIcon");

    // read name
    name = config.readEntry("Name");
  
    if (name.isNull())
    {
	const char *p = strrchr( filename, '/' );
	if (p)
	    name = p + 1;
	else
	    name = filename;
	int pos;
	if ( ( pos = name.findRev( ".kdelnk" ) ) > 0 )
	{
	    name = name.left( pos );
	}
    }
    return true;
}

void HTreeListItem::insertInTree(KTreeList *tree, KTreeListItem *parent)
{
    QPixmap *item_pm = new QPixmap(kapp->kde_icondir() + "/mini/" + miniicon);

    setText(name);
    setPixmap(item_pm);
  
    if (parent == 0) // insert at toplevel
    {
	setIndent(0);
	tree->insertItem(this, -1, true);
    }
    else             // insert as child of parent
    {
	setIndent(14);
	parent->appendChild(this);
    } 
}

void HTreeListItem::setName(QString _name)
{
    name = _name;
}

void HTreeListItem::setURL(QString _url)
{
    url = _url;
}

void HTreeListItem::setInfo(QString _info)
{
    info = _info;
}

void HTreeListItem::setIcon(QString _icon)
{
    icon = _icon;
}

void HTreeListItem::setMiniIcon(QString _miniicon)
{
    miniicon = _miniicon;
}
