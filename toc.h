/*
 *  toc.h - part of the KDE Help Center
 *
 *  Copyright (C) 2002 Frerich Raabe (raabe@kde.org)
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
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#ifndef KHC_TOC_H
#define KHC_TOC_H

#include "navigatoritem.h"

#include <qdom.h>
#include <qlistview.h>
#include <qobject.h>

class KProcess;

namespace KHC {

class TOC : public QObject
{
	Q_OBJECT
	public:
		TOC( NavigatorItem *parentItem );

		QString application() const { return m_application; }
		void setApplication( const QString &application ) { m_application = application; }
	
	public slots:
		void build( const QString &file );
		
	signals:
		void itemSelected( const QString &url );

	private slots:
		void slotItemSelected( QListViewItem *item );
		void meinprocExited( KProcess *meinproc );

	private:
		enum CacheStatus { NeedRebuild, CacheOk };

		CacheStatus cacheStatus() const;
		int sourceFileCTime() const;
		int cachedCTime() const;
		QDomElement childElement( const QDomElement &e, const QString &name );
		void buildCache();
		void fillTree();

		QString m_application;
		QString m_cacheFile;
		QString m_sourceFile;

		NavigatorItem *m_parentItem;
};

}

#endif // KHC_TOC_H
// vim:ts=2:sw=2:et
