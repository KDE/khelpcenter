/*
    SPDX-FileCopyrightText: 1999 Matthias Elter <me@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only
*/

#include "navigatorappitem.h"
#include "docentry.h"
#include "toc.h"
#include "view.h"
#include "khc_debug.h"

#include <QMetaObject>
#include <QUrl>

using namespace KHC;

QMap< QString,NavigatorAppItem* > NavigatorAppItem::s_menuItemsMap;

NavigatorAppItem::NavigatorAppItem( DocEntry *entry, QTreeWidgetItem *parent )
  : NavigatorItem( entry, parent )
{
  auto iter = s_menuItemsMap.find(entry->url());
  if (iter == s_menuItemsMap.end())
  {
    s_menuItemsMap.insert(entry->url(), this);
  }
}

NavigatorAppItem::NavigatorAppItem( DocEntry *entry, QTreeWidget *parent,
                  QTreeWidgetItem *after )
  : NavigatorItem( entry, parent, after )
{
  auto iter = s_menuItemsMap.find(entry->url());
  if (iter == s_menuItemsMap.end())
  {
    s_menuItemsMap.insert(entry->url(), this);
  }
}

NavigatorAppItem::NavigatorAppItem( DocEntry *entry, QTreeWidgetItem *parent,
                  QTreeWidgetItem *after )
  : NavigatorItem( entry, parent, after )
{
  auto iter = s_menuItemsMap.find(entry->url());
  if (iter == s_menuItemsMap.end())
  {
    s_menuItemsMap.insert(entry->url(), this);
  }
}

NavigatorAppItem::~NavigatorAppItem()
{
  auto iter = s_menuItemsMap.find(entry()->url());
  if ((iter != s_menuItemsMap.end()) && (iter.value() == this))
  {
    s_menuItemsMap.erase(iter);
  }

  delete mToc;
}

void NavigatorAppItem::itemExpanded( bool open )
{
  if ( open ) {
    QUrl expanded_url(entry()->url());
    expanded_url.setFragment(QString());

    auto iter = s_menuItemsMap.find(expanded_url.toString());
    if (iter != s_menuItemsMap.end())
    {
      iter.value()->scheduleTOCBuild();
    }
    else
    {
      scheduleTOCBuild();
    }
  }
}

void NavigatorAppItem::scheduleTOCBuild()
{
  if ( mToc ) {
    return;
  }

  const QUrl url(entry()->url());
  if ( url.scheme() == QLatin1String( "help" ) ) {
    mToc = new TOC( this );

    qCDebug(KHC_LOG) << "Trying to build TOC for" << entry()->name();
    mToc->setApplication( url.toString( QUrl::RemoveScheme | QUrl::RemoveFilename | QUrl::StripTrailingSlash | QUrl::RemoveFragment) );
    QString doc = View::langLookup( url.path() );
    // Enforce the original .docbook version, in case langLookup returns a
    // cached version
    if ( !doc.isNull() ) {
      int pos = doc.indexOf( QLatin1String( ".html" ) );
      if ( pos >= 0 ) {
        doc.replace( pos, 5, QStringLiteral( ".docbook" ) );
      }
      qCDebug(KHC_LOG) << "doc =" << doc;

      mToc->build( doc );
      // ensure the newly populated item is expanded
      QMetaObject::invokeMethod( treeWidget(), "expandItem", Qt::QueuedConnection, Q_ARG( const QTreeWidgetItem*, this ) );
    }
  }
}

// vim:ts=2:sw=2:et
