/*
 *  This file is part of the KDE Help Center
 *
 *  Copyright (C) 2001 Waldo Bastian <bastian@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as 
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "navigatorappitem.h"

#include "docentry.h"

#include <kdebug.h>
#include <kservicegroup.h>

using namespace KHC;

NavigatorAppItem::NavigatorAppItem( DocEntry *entry, Q3ListView *parent,
                  const QString &relPath )
  : NavigatorItem( entry, parent ),
    mRelpath( relPath ),
    mPopulated( false )
{
  setExpandable( true );
}

NavigatorAppItem::NavigatorAppItem( DocEntry *entry, Q3ListViewItem *parent,
                  const QString &relPath )
  : NavigatorItem( entry, parent ),
    mRelpath( relPath ),
    mPopulated( false )
{
  setExpandable( true );
}

NavigatorAppItem::NavigatorAppItem( DocEntry *entry, Q3ListView *parent,
                  Q3ListViewItem *after )
  : NavigatorItem( entry, parent, after ),
    mPopulated( false )
{
  setExpandable( true );
}

NavigatorAppItem::NavigatorAppItem( DocEntry *entry, Q3ListViewItem *parent,
                  Q3ListViewItem *after )
  : NavigatorItem( entry, parent, after ),
    mPopulated( false )
{
  setExpandable( true );
}

void NavigatorAppItem::setRelpath( const QString &relpath )
{
  mRelpath = relpath;
}

void NavigatorAppItem::setOpen(bool open)
{
  kdDebug() << "NavigatorAppItem::setOpen()" << endl;

  if ( open && (childCount() == 0) && !mPopulated )
  {
     kdDebug() << "NavigatorAppItem::setOpen(" << this << ", "
               << mRelpath << ")" << endl;
     populate();
  }
  Q3ListViewItem::setOpen(open); 
}

void NavigatorAppItem::populate( bool recursive )
{
  if ( mPopulated ) return;

  KServiceGroup::Ptr root = KServiceGroup::group(mRelpath);
  if ( !root ) {
    kdWarning() << "No Service groups\n";
    return;
  }
  KServiceGroup::List list = root->entries();


  for ( KServiceGroup::List::ConstIterator it = list.begin();
        it != list.end(); ++it )
  {
    KSycocaEntry * e = *it;
    KService::Ptr s;
    NavigatorItem *item;
    KServiceGroup::Ptr g;
    QString url;

    switch ( e->sycocaType() ) {
      case KST_KService:
      {
        s = static_cast<KService*>(e);
        url = documentationURL( s );
        if ( !url.isEmpty() ) {
          DocEntry *entry = new DocEntry( s->name(), url, s->icon() );
          item = new NavigatorItem( entry, this );
          item->setAutoDeleteDocEntry( true );
          item->setExpandable( true );
        }
        break;
      }
      case KST_KServiceGroup:
      {
        g = static_cast<KServiceGroup*>(e);
        if ( ( g->childCount() == 0 ) || g->name().startsWith( "." ) )
          continue;
        DocEntry *entry = new DocEntry( g->caption(), "", g->icon() );              
        NavigatorAppItem *appItem;
        appItem = new NavigatorAppItem( entry, this, g->relPath() );
        appItem->setAutoDeleteDocEntry( true );
        if ( recursive ) appItem->populate( recursive );
        break;
      }
      default:
        break;
    }
  }
  sortChildItems( 0, true /* ascending */ );
  mPopulated = true;
}

QString NavigatorAppItem::documentationURL( KService *s )
{
  QString docPath = s->property( "DocPath" ).toString();
  if ( docPath.isEmpty() )
    return QString::null;
  
  if ( docPath.startsWith( "file:") || docPath.startsWith( "http:" ) )
    return docPath;
  
  return QString( "help:/" ) + docPath;
}

// vim:ts=2:sw=2:et
