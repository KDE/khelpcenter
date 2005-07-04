/*
 *  This file is part of the KDE Help Center
 *
 *  Copyright (C) 2002 Frerich Raabe <raabe@kde.org>
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
 *  Foundation, Inc., 51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#ifndef TREEBUILDER_H
#define TREEBUILDER_H

#include "qobject.h"

#include <kurl.h>

namespace KHC
{

class NavigatorItem;
class TreeBuilder : public QObject
{
	Q_OBJECT
	public:
		TreeBuilder( QObject *parent, const char *name = 0 );

		virtual void build( NavigatorItem *item ) = 0;

  public slots:
    virtual void selectURL( const QString &url );

  signals:
    void urlSelected( const KURL &url );
};

}

#endif // TREEBUILDER_H
// vim:ts=2:sw=2:et
