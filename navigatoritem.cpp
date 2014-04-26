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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "navigatoritem.h"

#include "toc.h"
#include "docentry.h"
#include "view.h"

#include <KIconLoader>

#include <QPixmap>

using namespace KHC;

namespace {
QLoggingCategory category("org.kde.khelpcenter");
}

NavigatorItem::NavigatorItem( DocEntry *entry, QTreeWidget *parent,
                              QTreeWidgetItem *after )
  : QTreeWidgetItem( parent, after )
{
  init( entry );
}

NavigatorItem::NavigatorItem( DocEntry *entry, QTreeWidgetItem *parent,
                              QTreeWidgetItem *after )
  : QTreeWidgetItem( parent, after )
{
  init( entry );
}

NavigatorItem::NavigatorItem( DocEntry *entry, QTreeWidget *parent )
  : QTreeWidgetItem( parent )
{
    init( entry );
}

NavigatorItem::NavigatorItem( DocEntry *entry, QTreeWidgetItem *parent )
  : QTreeWidgetItem( parent )
{
  init( entry );
}

NavigatorItem::~NavigatorItem()
{
  delete mToc;

  if ( mAutoDeleteDocEntry ) delete mEntry;
}

void NavigatorItem::init( DocEntry *entry )
{
  mEntry = entry;
  mAutoDeleteDocEntry = false;
  mToc = 0;

  updateItem();
}

DocEntry *NavigatorItem::entry() const
{
  return mEntry;
}

void NavigatorItem::setAutoDeleteDocEntry( bool enabled )
{
  mAutoDeleteDocEntry = enabled;
}

void NavigatorItem::updateItem()
{
  setText( 0, entry()->name() );
  setIcon( 0, SmallIcon( entry()->icon() ) );
}

void NavigatorItem::scheduleTOCBuild()
{
  QUrl url ( entry()->url() );
  if ( !mToc && url.scheme() == "help") {
    mToc = new TOC( this );

    qCDebug(category) << "Trying to build TOC for " << entry()->name();
    mToc->setApplication( url.toString(QUrl::RemoveScheme|QUrl::RemoveFilename|QUrl::StripTrailingSlash) );
    QString doc = View::langLookup( url.path() );
    // Enforce the original .docbook version, in case langLookup returns a
    // cached version
    if ( !doc.isNull() ) {
      int pos = doc.indexOf( ".html" );
      if ( pos >= 0 ) {
        doc.replace( pos, 5, ".docbook" );
      }
      qCDebug(category) << "doc = " << doc;

      mToc->build( doc );
    }
  }
}

void NavigatorItem::setExpanded( bool open )
{
  scheduleTOCBuild();
  QTreeWidgetItem::setExpanded( open );
}

// vim:ts=2:sw=2:et
