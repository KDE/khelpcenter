/*
 *  khc_navigatoritem.cc - part of the KDE Help Center
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

#include "khc_factory.h"
#include "docmetainfo.h"

#include "khc_navigatoritem.h"

khcNavigatorItem::khcNavigatorItem(QListView* parent, const QString& _text, const QString& _miniicon)
    : QListViewItem(parent)
{
    init(_text, _miniicon);
}

khcNavigatorItem::khcNavigatorItem(QListViewItem* parent, const QString& _text, const QString& _miniicon)
    : QListViewItem(parent)
{
    init(_text, _miniicon);
}

khcNavigatorItem::khcNavigatorItem(QListView* parent, QListViewItem* after, const QString& _text, const QString& _miniicon)
    : QListViewItem(parent, after)
{
    init(_text, _miniicon);
}

khcNavigatorItem::khcNavigatorItem(QListViewItem* parent, QListViewItem* after, const QString& _text, const QString& _miniicon)
    : QListViewItem(parent, after)
{
    init(_text, _miniicon);
}

void khcNavigatorItem::init(const QString& _text, const QString& _miniicon)
{
    mName = _text;
    mMiniIcon = _miniicon;
    
    setText( 0, mName );
    setPixmap( 0, SmallIcon( mMiniIcon, 0, 0, KHCFactory::instance() ) );
}

bool khcNavigatorItem::readKDElnk ( const QString &filename )
{
    kdDebug() << "khcNavigatorItem::readKDElnk(): " << filename << endl;

    DocEntry *entry = DocMetaInfo::self()->addDocEntry( filename );
    if ( !entry ) return false;

    mUrl = entry->docPath();
    if ( mUrl.isNull() ) return false;

    mInfo = entry->info();
    mIcon = entry->icon();
    
    mMiniIcon = "document2";
    setPixmap( 0, SmallIcon( mMiniIcon, 0, 0, KHCFactory::instance() ) );

    mName = entry->name();
  
    if ( mName.isNull() ) {
        mName = filename.mid( filename.find('/') );
        int pos;
        if ( ( pos = mName.findRev( ".desktop" ) ) > 0 ) {
            mName = mName.left( pos );
        }
    }
    setText( 0, mName );
    return true;
}

void khcNavigatorItem::setName( const QString &_name )
{
    mName = _name;
}

void khcNavigatorItem::setUrl( const QString &_url )
{
    mUrl = _url;
}

void khcNavigatorItem::setInfo( const QString &_info )
{
    mInfo = _info;
}

void khcNavigatorItem::setIcon( const QString &_icon )
{
    mIcon = _icon;
}

void khcNavigatorItem::setMiniIcon( const QString &_miniicon )
{
    mMiniIcon = _miniicon;
}
