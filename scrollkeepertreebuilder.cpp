/*
    SPDX-FileCopyrightText: 1999 Matthias Elter <me@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "scrollkeepertreebuilder.h"

#include "navigatoritem.h"
#include "docentry.h"
#include "khc_debug.h"

#include <KProcess>

#include <QDomDocument>
#include <QFile>
#include <QStandardPaths>

#include <prefs.h>

using namespace KHC;

ScrollKeeperTreeBuilder::ScrollKeeperTreeBuilder( QObject *parent )
	: QObject( parent )
{
	loadConfig();
}

void ScrollKeeperTreeBuilder::loadConfig()
{
  mShowEmptyDirs = Prefs::showEmptyDirs();
}

void ScrollKeeperTreeBuilder::buildOrHide( NavigatorItem *parent )
{
  build(parent);

  parent->setHidden(parent->childCount() == 0);
}

void ScrollKeeperTreeBuilder::build( NavigatorItem *parent )
{
  const QString lang = QLocale().bcp47Name();

  qCDebug(KHC_LOG) << "ScrollKeeper language: " << lang;

  const QString exePath = QStandardPaths::findExecutable( QStringLiteral( "scrollkeeper-get-content-list" ) );
  if ( exePath.isEmpty() ) {
    qCDebug(KHC_LOG) << "scrollkeeper-get-content-list is not available, skipping";
    return;
  }

  KProcess proc;
  proc << exePath;
  proc << lang;
  
  proc.setOutputChannelMode(KProcess::OnlyStdoutChannel);
  proc.start();
  if ( !proc.waitForFinished() ) {
    qCWarning(KHC_LOG) << "Could not execute scrollkeeper-get-content-list";
    return;
  }
  mContentsList =QString::fromUtf8( proc.readAllStandardOutput().trimmed());

  if (!QFile::exists(mContentsList)) {
    qCWarning(KHC_LOG) << "Scrollkeeper contents file" << mContentsList
      << "does not exist.";
    return;
  }

  QDomDocument doc(QStringLiteral("ScrollKeeperContentsList"));
  QFile f(mContentsList);
  if ( !f.open( QIODevice::ReadOnly ) )
    return;
  if ( !doc.setContent( &f ) ) {
    f.close();
    return;
  }
  f.close();

  // Create top-level item
  mItems.append(parent);

  QDomElement docElem = doc.documentElement();

  QDomNode n = docElem.firstChild();
  while( !n.isNull() ) {
    QDomElement e = n.toElement();
    if( !e.isNull() ) {
      if (e.tagName() == QLatin1String("sect")) {
        NavigatorItem *createdItem;
        insertSection( parent, e, createdItem );
      }
    }
    n = n.nextSibling();
  }
}

int ScrollKeeperTreeBuilder::insertSection( NavigatorItem *parent,
                                            const QDomNode &sectNode,
                                            NavigatorItem *&sectItem )
{
// TODO: was contents2 -> needs to be changed to help-contents-alternate or similar
  DocEntry *entry = new DocEntry( QString(), QString(), QStringLiteral("help-contents") );
  sectItem = new NavigatorItem( entry, parent );
  sectItem->setAutoDeleteDocEntry( true );
  mItems.append( sectItem );

  int numDocs = 0;  // Number of docs created in this section

  QDomNode n = sectNode.firstChild();
  while( !n.isNull() ) {
    QDomElement e = n.toElement();
    if( !e.isNull() ) {
      if ( e.tagName() == QLatin1String("title") ) {
        entry->setName( e.text() );
        sectItem->updateItem();
      } else if (e.tagName() == QLatin1String("sect")) {
        NavigatorItem *created;
        numDocs += insertSection( sectItem, e, created );
      } else if (e.tagName() == QLatin1String("doc")) {
        insertDoc(sectItem,e);
        ++numDocs;
      }
    }
    n = n.nextSibling();
  }

  // Remove empty sections
  if (!mShowEmptyDirs && numDocs == 0) {
    delete sectItem;
    sectItem = nullptr;
  }

  return numDocs;
}

void ScrollKeeperTreeBuilder::insertDoc( NavigatorItem *parent,
                                         const QDomNode &docNode )
{
  DocEntry *entry = new DocEntry( QString(), QString(), QStringLiteral("text-plain") );
  NavigatorItem *docItem = new NavigatorItem( entry, parent );
  docItem->setAutoDeleteDocEntry( true );
  mItems.append( docItem );

  QString url;

  QDomNode n = docNode.firstChild();
  while( !n.isNull() ) {
    QDomElement e = n.toElement();
    if( !e.isNull() ) {
      if ( e.tagName() == QLatin1String("doctitle") ) {
        entry->setName( e.text() );
        docItem->updateItem();
      } else if ( e.tagName() == QLatin1String("docsource") ) {
        url.append( e.text() );
      } else if ( e.tagName() == QLatin1String("docformat") ) {
        QString mimeType = e.text();
        if ( mimeType == QLatin1String("text/html")) {
          // Let the HTML part figure out how to get the doc
        } else if ( mimeType == QLatin1String("application/xml")
                   || mimeType == QLatin1String("text/xml") /*deprecated name*/ ) {
          if ( url.left( 5 ) == QLatin1String("file:") ) url = url.mid( 5 );
          url.prepend( QLatin1String("ghelp:") );
#if 0
          url.replace( QRegExp( ".xml$" ), ".html" );
#endif
        } else if ( mimeType == QLatin1String("text/sgml") ) {
          // GNOME docs use this type. We don't have a real viewer for this.
          url.prepend( QStringLiteral("file:") );
        } else if ( mimeType.left(5) == QLatin1String("text/") ) {
          url.prepend( QStringLiteral("file:") );
        }
      }
    }
    n = n.nextSibling();
  }

  entry->setUrl( url );
}


// vim:sw=2:ts=2:et
