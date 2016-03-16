/*
 *  This file is part of the KDE Help Center
 *
 *  Copyright (C) 1999 Matthias Elter (me@kde.org)
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
#include "toc.h"
#include "view.h"
#include "khc_debug.h"

#include <QUrl>
#include <QMetaObject>

using namespace KHC;

NavigatorAppItem::NavigatorAppItem( DocEntry *entry, QTreeWidgetItem *parent )
  : NavigatorItem( entry, parent ),
    mToc( 0 )
{
}

NavigatorAppItem::NavigatorAppItem( DocEntry *entry, QTreeWidget *parent,
                  QTreeWidgetItem *after )
  : NavigatorItem( entry, parent, after ),
    mToc( 0 )
{
}

NavigatorAppItem::NavigatorAppItem( DocEntry *entry, QTreeWidgetItem *parent,
                  QTreeWidgetItem *after )
  : NavigatorItem( entry, parent, after ),
    mToc( 0 )
{
}

NavigatorAppItem::~NavigatorAppItem()
{
  delete mToc;
}

void NavigatorAppItem::itemExpanded( bool open )
{
  if ( open ) {
    scheduleTOCBuild();
  }
}

void NavigatorAppItem::scheduleTOCBuild()
{
  if ( mToc ) {
    return;
  }

  const QUrl url = entry()->url();
  if ( url.scheme() == QLatin1String( "help" ) ) {
    mToc = new TOC( this );

    khcDebug() << "Trying to build TOC for" << entry()->name();
    mToc->setApplication( url.toString( QUrl::RemoveScheme | QUrl::RemoveFilename | QUrl::StripTrailingSlash ) );
    QString doc = View::langLookup( url.path() );
    // Enforce the original .docbook version, in case langLookup returns a
    // cached version
    if ( !doc.isNull() ) {
      int pos = doc.indexOf( QLatin1String( ".html" ) );
      if ( pos >= 0 ) {
        doc.replace( pos, 5, QLatin1String( ".docbook" ) );
      }
      khcDebug() << "doc =" << doc;

      mToc->build( doc );
      // ensure the newly populated item is expanded
      QMetaObject::invokeMethod( treeWidget(), "expandItem", Qt::QueuedConnection, Q_ARG( const QTreeWidgetItem*, this ) );
    }
  }
}

// vim:ts=2:sw=2:et
