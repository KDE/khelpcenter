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

#include "scrollkeepertreebuilder.h"

#include "navigatoritem.h"
#include "docentry.h"

#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>
#include <k3procio.h>

#include <Qt/qdom.h>
#include <QFile>
#include <QRegExp>
#include <kconfiggroup.h>

using namespace KHC;

ScrollKeeperTreeBuilder::ScrollKeeperTreeBuilder( QObject *parent, const char *name )
	: QObject( parent )
{
  setObjectName( name );

	loadConfig();
}

void ScrollKeeperTreeBuilder::loadConfig()
{
  KConfigGroup configGroup( KGlobal::config(), "ScrollKeeper" );
  mShowEmptyDirs = configGroup.readEntry( "ShowEmptyDirs", false);
}

NavigatorItem *ScrollKeeperTreeBuilder::build( NavigatorItem *parent,
                                               NavigatorItem *after )
{
  QString lang = KGlobal::locale()->language();

  kDebug(1400) << "ScrollKeeper language: " << lang << endl;

  K3ProcIO proc;
  proc << "scrollkeeper-get-content-list";
  proc << lang;
  connect(&proc,SIGNAL(readReady(K3ProcIO *)),SLOT(getContentsList(K3ProcIO *)));
  if (!proc.start(K3Process::Block)) {
    kDebug(1400) << "Could not execute scrollkeeper-get-content-list" << endl;
    return 0;
  }

  if (!QFile::exists(mContentsList)) {
    kDebug(1400) << "Scrollkeeper contents file '" << mContentsList
      << "' does not exist." << endl;
    return 0;
  }

  QDomDocument doc("ScrollKeeperContentsList");
  QFile f(mContentsList);
  if ( !f.open( QIODevice::ReadOnly ) )
    return 0;
  if ( !doc.setContent( &f ) ) {
    f.close();
    return 0;
  }
  f.close();

  // Create top-level item
  mItems.append(parent);

  QDomElement docElem = doc.documentElement();

  NavigatorItem *result = 0;

  QDomNode n = docElem.firstChild();
  while( !n.isNull() ) {
    QDomElement e = n.toElement();
    if( !e.isNull() ) {
      if (e.tagName() == "sect") {
        NavigatorItem *createdItem;
        insertSection( parent, after, e, createdItem );
        if ( createdItem ) result = createdItem;
      }
    }
    n = n.nextSibling();
  }

  return result;
}

void ScrollKeeperTreeBuilder::getContentsList( K3ProcIO *proc )
{
  QString filename;
  proc->readln( filename, true );

  mContentsList = filename;
}

int ScrollKeeperTreeBuilder::insertSection( NavigatorItem *parent,
                                            NavigatorItem *after,
                                            const QDomNode &sectNode,
                                            NavigatorItem *&sectItem )
{
  DocEntry *entry = new DocEntry( "", "", "contents2" );
  sectItem = new NavigatorItem( entry, parent, after );
  sectItem->setAutoDeleteDocEntry( true );
  mItems.append( sectItem );

  int numDocs = 0;  // Number of docs created in this section

  QDomNode n = sectNode.firstChild();
  while( !n.isNull() ) {
    QDomElement e = n.toElement();
    if( !e.isNull() ) {
      if ( e.tagName() == "title" ) {
        entry->setName( e.text() );
        sectItem->updateItem();
      } else if (e.tagName() == "sect") {
        NavigatorItem *created;
        numDocs += insertSection( sectItem, 0, e, created );
      } else if (e.tagName() == "doc") {
        insertDoc(sectItem,e);
        ++numDocs;
      }
    }
    n = n.nextSibling();
  }

  // Remove empty sections
  if (!mShowEmptyDirs && numDocs == 0) {
    delete sectItem;
    sectItem = 0;
  }

  return numDocs;
}

void ScrollKeeperTreeBuilder::insertDoc( NavigatorItem *parent,
                                         const QDomNode &docNode )
{
  DocEntry *entry = new DocEntry( "", "", "document2" );
  NavigatorItem *docItem = new NavigatorItem( entry, parent );
  docItem->setAutoDeleteDocEntry( true );
  mItems.append( docItem );

  QString url;

  QDomNode n = docNode.firstChild();
  while( !n.isNull() ) {
    QDomElement e = n.toElement();
    if( !e.isNull() ) {
      if ( e.tagName() == "doctitle" ) {
        entry->setName( e.text() );
        docItem->updateItem();
      } else if ( e.tagName() == "docsource" ) {
        url.append( e.text() );
      } else if ( e.tagName() == "docformat" ) {
        QString mimeType = e.text();
        if ( mimeType == "text/html") {
          // Let the HTML part figure out how to get the doc
        } else if ( mimeType == "application/xml"
                   || mimeType == "text/xml" /*deprecated name*/ ) {
          if ( url.left( 5 ) == "file:" ) url = url.mid( 5 );
          url.prepend( "ghelp:" );
#if 0
          url.replace( QRegExp( ".xml$" ), ".html" );
#endif
        } else if ( mimeType == "text/sgml" ) {
          // GNOME docs use this type. We don't have a real viewer for this.
          url.prepend( "file:" );
        } else if ( mimeType.left(5) == "text/" ) {
          url.prepend( "file:" );
        }
      }
    }
    n = n.nextSibling();
  }

  entry->setUrl( url );
}

#include "scrollkeepertreebuilder.moc"
// vim:sw=2:ts=2:et
