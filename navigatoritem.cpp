/*
 *  This file is part of the KDE Help Center
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

#include <qlistview.h>
#include <qstring.h>
#include <qfile.h>

#include <kapplication.h>
#include <ksimpleconfig.h>
#include <kdebug.h>
#include <kstandarddirs.h>
#include <kglobal.h>
#include <kiconloader.h>

#include "docmetainfo.h"
#include "toc.h"

#include "navigatoritem.h"

using namespace KHC;

NavigatorItem::NavigatorItem(QListView *parent, QListViewItem *after)
  : QListViewItem( parent, after )
{
    init();
}

NavigatorItem::NavigatorItem(QListViewItem *parent, QListViewItem *after)
  : QListViewItem( parent, after )
{
    init();
}

NavigatorItem::NavigatorItem(QListView* parent, const QString& _text,
                                   const QString& _miniicon)
    : QListViewItem(parent)
{
    init(_text, _miniicon);
}

NavigatorItem::NavigatorItem(QListViewItem* parent, const QString& _text,
                                   const QString& _miniicon)
    : QListViewItem(parent)
{
    init(_text, _miniicon);
}

NavigatorItem::NavigatorItem(QListView* parent, QListViewItem* after,
                                   const QString& _text,
                                   const QString& _miniicon)
    : QListViewItem(parent, after)
{
    init(_text, _miniicon);
}

NavigatorItem::NavigatorItem(QListViewItem* parent, QListViewItem* after,
                                   const QString& _text,
                                   const QString& _miniicon)
    : QListViewItem(parent, after)
{
    init(_text, _miniicon);
}

void NavigatorItem::init()
{
    mToc = 0;
}

void NavigatorItem::init(const QString& text, const QString& icon)
{
    init();

    setName( text );
    setIcon( icon );
}

NavigatorItem::~NavigatorItem()
{
    delete mToc;
}

void NavigatorItem::setName( const QString &_name )
{
    mName = _name;
    setText( 0, mName );
}

void NavigatorItem::setUrl( const QString &_url )
{
    mUrl = _url;
}

void NavigatorItem::setInfo( const QString &_info )
{
    mInfo = _info;
}

void NavigatorItem::setIcon( const QString &_icon )
{
    mIcon = _icon;
    setPixmap( 0, SmallIcon( mIcon ) );
}

TOC *NavigatorItem::createTOC()
{
    mToc = new TOC( this );
    return mToc;
}
// vim:ts=2:sw=2:et
