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
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <kdebug.h>
#include <kservice.h>
#include <kservicegroup.h>

#include "navigatorappitem.h"

using namespace KHC;

NavigatorAppItem::NavigatorAppItem(QListView *parent, QListViewItem *after)
 : NavigatorItem(parent, after)
{
  setExpandable(true);
}

NavigatorAppItem::NavigatorAppItem(QListViewItem *parent, QListViewItem *after)
 : NavigatorItem(parent, after)
{
  setExpandable(true);
}

NavigatorAppItem::NavigatorAppItem (QListView* parent, const QString& text, const QString& miniicon, const QString& _relpath)
 : NavigatorItem(parent, text, miniicon)
 , relpath(_relpath)
{
  setExpandable(true);
}
 
NavigatorAppItem::NavigatorAppItem (QListViewItem* parent, const QString& text, const QString& miniicon, const QString& _relpath)
 : NavigatorItem(parent, text, miniicon)
 , relpath(_relpath)
{
  setExpandable(true);
}

void NavigatorAppItem::setOpen(bool open)
{
  if ( open && (childCount() == 0) )
  {
     kdWarning() << "NavigatorAppItem::setOpen(" << this << ", "
                 << relpath << ")" << endl;
     KServiceGroup::Ptr root = KServiceGroup::group(relpath);
     if (!root) {
        kdWarning() << "No Service groups\n";
        return;
     }
     KServiceGroup::List list = root->entries();


     for (KServiceGroup::List::ConstIterator it = list.begin(); it != list.end(); ++it)
     {
        KSycocaEntry * e = *it;
        KService::Ptr s;
        NavigatorItem *item;
        KServiceGroup::Ptr g;
        QString url;

        switch (e->sycocaType())
	{
	case KST_KService:
              s = static_cast<KService*>(e);
              url = documentationURL(s);
              if (!url.isEmpty())
              {
                 item = new NavigatorItem(this, s->name(), s->icon());
                 item->setUrl(url);
              }
              break;

        case KST_KServiceGroup:
              g = static_cast<KServiceGroup*>(e);
              if ( (g->childCount() == 0 ) || g->name().startsWith("."))
                 continue;
              item = new NavigatorAppItem(this, g->caption(), g->icon(), g->relPath());
              item->setUrl("");
              break;

        default:
              break;
        }
     }
  }
  NavigatorItem::setOpen(open); 
}

// derive a valid URL to the documentation
QString NavigatorAppItem::documentationURL(KService *s)
{
  // if entry contains a DocPath, process it
  QString docPath = s->property("DocPath").toString();
  if (!docPath.isEmpty())
    {
      // see if it is part of our help system, or external
      // note that this test might be a bit too stupid
      if (docPath.left(5) == "file:" || docPath.left(5) == "http:")
          return docPath;

      return QString( "help:/" ) + docPath;
    }
  else
    return QString::null;

}
