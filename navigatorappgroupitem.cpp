/*
    SPDX-FileCopyrightText: 2001 Waldo Bastian <bastian@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only
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
    mRelpath( relPath )
{ 
  populate();
}

NavigatorAppGroupItem::NavigatorAppGroupItem( DocEntry *entry, QTreeWidgetItem *parent,
                       const QString &relPath )
  : NavigatorItem( entry, parent ),
    mRelpath( relPath )
{ 
  populate();
}

NavigatorAppGroupItem::NavigatorAppGroupItem( DocEntry *entry, QTreeWidget *parent,
                       QTreeWidgetItem *after )
  : NavigatorItem( entry, parent, after )
{ 
  populate();
}

NavigatorAppGroupItem::NavigatorAppGroupItem( DocEntry *entry, QTreeWidgetItem *parent,
                       QTreeWidgetItem *after )
  : NavigatorItem( entry, parent, after )
{ 
  populate();
}

void NavigatorAppGroupItem::setRelpath( const QString &relpath )
{
  mRelpath = relpath;
}

void NavigatorAppGroupItem::itemExpanded(bool open)
{
  qCDebug(KHC_LOG) << "NavigatorAppGroupItem::itemExpanded()";

  if ( open && (childCount() == 0) && !mPopulated )
  {
     qCDebug(KHC_LOG) << "  -> populate:" << this << "-" << mRelpath;
     populate();
  }
  NavigatorItem::itemExpanded(open);
}

void NavigatorAppGroupItem::populate( bool recursive )
{
  if ( mPopulated ) return;

  KServiceGroup::Ptr root = KServiceGroup::group(mRelpath);
  if ( !root ) {
    qCWarning(KHC_LOG) << "No Service groups for" << mRelpath;
    return;
  }
  KServiceGroup::List list = root->entries();
  bool no_children_present = true;


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
          no_children_present = false;
        }
        break;
      }
      case KST_KServiceGroup:
      {
        const KServiceGroup::Ptr g(static_cast<KServiceGroup*>(e.data()));
        if ( ( g->childCount() == 0 ) || g->name().startsWith( QLatin1Char('.') ) )
          continue;
        DocEntry *entry = new DocEntry( g->caption(), QString(), g->icon() );
        NavigatorAppGroupItem *appItem;
        appItem = new NavigatorAppGroupItem( entry, this, g->relPath() );
        appItem->setAutoDeleteDocEntry( true );
        if ( recursive ) appItem->populate( recursive );
        no_children_present = false;
        break;
      }
      default:
        break;
    }
  }
  sortChildren( 0, Qt::AscendingOrder /* ascending */ );
  mPopulated = true;
  setHidden(no_children_present);
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

  if ( docPath.startsWith(QLatin1String("file:")) || docPath.startsWith(QLatin1String("http") ) )
    return docPath;

  return QStringLiteral( "help:/" ) + docPath;
}

// vim:ts=2:sw=2:et
