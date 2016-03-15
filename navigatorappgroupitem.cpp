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

#include "navigatorappgroupitem.h"
#include "navigatorappitem.h"
#include "docentry.h"
#include "khc_debug.h"

#include <KService>
#include <KServiceGroup>

using namespace KHC;

NavigatorAppGroupItem::NavigatorAppGroupItem( DocEntry *entry, QTreeWidget *parent,
                       const QString &relPath )
  : NavigatorItem( entry, parent ),
    mRelpath( relPath ),
    mPopulated( false )
{ 
  populate();
}

NavigatorAppGroupItem::NavigatorAppGroupItem( DocEntry *entry, QTreeWidgetItem *parent,
                       const QString &relPath )
  : NavigatorItem( entry, parent ),
    mRelpath( relPath ),
    mPopulated( false )
{ 
  populate();
}

NavigatorAppGroupItem::NavigatorAppGroupItem( DocEntry *entry, QTreeWidget *parent,
                       QTreeWidgetItem *after )
  : NavigatorItem( entry, parent, after ),
    mPopulated( false )
{ 
  populate();
}

NavigatorAppGroupItem::NavigatorAppGroupItem( DocEntry *entry, QTreeWidgetItem *parent,
                       QTreeWidgetItem *after )
  : NavigatorItem( entry, parent, after ),
    mPopulated( false )
{ 
  populate();
}

void NavigatorAppGroupItem::setRelpath( const QString &relpath )
{
  mRelpath = relpath;
}

void NavigatorAppGroupItem::itemExpanded(bool open)
{
  khcDebug() << "NavigatorAppGroupItem::itemExpanded()";

  if ( open && (childCount() == 0) && !mPopulated )
  {
     khcDebug() << "NavigatorAppGroupItem::itemExpanded(" << this << ", "
               << mRelpath << ")" << endl;
     populate();
  }
  NavigatorItem::itemExpanded(open);
}

void NavigatorAppGroupItem::populate( bool recursive )
{
  if ( mPopulated ) return;

  KServiceGroup::Ptr root = KServiceGroup::group(mRelpath);
  if ( !root ) {
    khcWarning() << "No Service groups\n";
    return;
  }
  KServiceGroup::List list = root->entries();


  for ( KServiceGroup::List::ConstIterator it = list.constBegin();
        it != list.constEnd(); ++it )
  {
    const KSycocaEntry::Ptr e = *it;
    QString url;

    switch ( e->sycocaType() ) {
      case KST_KService:
      {
        const KService::Ptr s(static_cast<KService*>(e.data()));
        url = documentationURL( s.data() );
        if ( !url.isEmpty() ) {
          DocEntry *entry = new DocEntry( s->name(), url, s->icon() );
          NavigatorAppItem *item = new NavigatorAppItem( entry, this );
          item->setAutoDeleteDocEntry( true );
        }
        break;
      }
      case KST_KServiceGroup:
      {
        const KServiceGroup::Ptr g(static_cast<KServiceGroup*>(e.data()));
        if ( ( g->childCount() == 0 ) || g->name().startsWith( '.' ) )
          continue;
        DocEntry *entry = new DocEntry( g->caption(), "", g->icon() );
        NavigatorAppGroupItem *appItem;
        appItem = new NavigatorAppGroupItem( entry, this, g->relPath() );
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

QString NavigatorAppGroupItem::documentationURL( const KService *s )
{
  QString docPath = s->property( QStringLiteral("DocPath") ).toString();
  if ( docPath.isEmpty() ) {
    docPath = s->property( QStringLiteral("X-DocPath") ).toString();
    if ( docPath.isEmpty() ) {
      return QString();
    }
  }

  if ( docPath.startsWith( QStringLiteral("file:")) || docPath.startsWith( QStringLiteral("http:") ) )
    return docPath;

  return QStringLiteral( "help:/" ) + docPath;
}

// vim:ts=2:sw=2:et
