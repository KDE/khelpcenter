/*
 *  This file is part of the KDE Help Center
 *
 *  Copyright (C) 2001 Waldo Bastian <bastian@kde.org>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License version 2 or at your option version 3 as published
 *  by the Free Software Foundation.
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

#include <KDebug>
#include <KService>
#include <KServiceGroup>

using namespace KHC;

NavigatorAppItem::NavigatorAppItem( DocEntry *entry, QTreeWidget *parent,
                  const QString &relPath )
  : NavigatorItem( entry, parent ),
    mRelpath( relPath ),
    mPopulated( false )
{ 
  populate();
}

NavigatorAppItem::NavigatorAppItem( DocEntry *entry, QTreeWidgetItem *parent,
                  const QString &relPath )
  : NavigatorItem( entry, parent ),
    mRelpath( relPath ),
    mPopulated( false )
{ 
  populate();
}

NavigatorAppItem::NavigatorAppItem( DocEntry *entry, QTreeWidget *parent,
                  QTreeWidgetItem *after )
  : NavigatorItem( entry, parent, after ),
    mPopulated( false )
{ 
  populate();
}

NavigatorAppItem::NavigatorAppItem( DocEntry *entry, QTreeWidgetItem *parent,
		    QTreeWidgetItem *after )
  : NavigatorItem( entry, parent, after ),
    mPopulated( false )
{ 
  populate();
}

void NavigatorAppItem::setRelpath( const QString &relpath )
{
  mRelpath = relpath;
}

void NavigatorAppItem::setExpanded(bool open)
{
  kDebug() << "NavigatorAppItem::setOpen()";

  if ( open && (childCount() == 0) && !mPopulated )
  {
     kDebug() << "NavigatorAppItem::setOpen(" << this << ", "
               << mRelpath << ")" << endl;
     populate();
  }
  QTreeWidgetItem::setExpanded(open);
}

void NavigatorAppItem::populate( bool recursive )
{
  if ( mPopulated ) return;

  KServiceGroup::Ptr root = KServiceGroup::group(mRelpath);
  if ( !root ) {
    kWarning() << "No Service groups\n";
    return;
  }
  KServiceGroup::List list = root->entries();


  for ( KServiceGroup::List::ConstIterator it = list.constBegin();
        it != list.constEnd(); ++it )
  {
    const KSycocaEntry::Ptr e = *it;
    NavigatorItem *item;
    QString url;

    switch ( e->sycocaType() ) {
      case KST_KService:
      {
        const KService::Ptr s = KService::Ptr::staticCast(e);
        url = documentationURL( s.data() );
        if ( !url.isEmpty() ) {
          DocEntry *entry = new DocEntry( s->name(), url, s->icon() );
          item = new NavigatorItem( entry, this );
          item->setAutoDeleteDocEntry( true );
        }
        break;
      }
      case KST_KServiceGroup:
      {
        KServiceGroup::Ptr g = KServiceGroup::Ptr::staticCast(e);
        if ( ( g->childCount() == 0 ) || g->name().startsWith( '.' ) )
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
  sortChildren( 0, Qt::AscendingOrder /* ascending */ );
  mPopulated = true;
}

QString NavigatorAppItem::documentationURL( const KService *s )
{
  QString docPath = s->property( QLatin1String("DocPath") ).toString();
  if ( docPath.isEmpty() ) {
    docPath = s->property( QLatin1String("X-DocPath") ).toString();
    if ( docPath.isEmpty() ) {
      return QString();
    }
  }

  if ( docPath.startsWith( QLatin1String("file:")) || docPath.startsWith( QLatin1String("http:") ) )
    return docPath;

  return QLatin1String( "help:/" ) + docPath;
}

// vim:ts=2:sw=2:et
