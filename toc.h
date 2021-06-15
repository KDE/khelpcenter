/*
    SPDX-FileCopyrightText: 2002 Frerich Raabe <raabe@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KHC_TOC_H
#define KHC_TOC_H

#include "navigatoritem.h"

#include <QDomElement>
#include <QObject>
#include <QProcess>

class QTreeWidgetItem;

namespace KHC {

class TOC : public QObject
{
	Q_OBJECT
	public:
		explicit TOC( NavigatorItem *parentItem );

		QString application() const { return m_application; }
		void setApplication( const QString &application ) { m_application = application; }
	
	public Q_SLOTS:
		void build( const QString &file );
		
	Q_SIGNALS:
		void itemSelected( const QString &url );

	private Q_SLOTS:
		void slotItemSelected( QTreeWidgetItem *item );
		void meinprocExited( int exitCode, QProcess::ExitStatus exitStatus);

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

        NavigatorItem *const m_parentItem;
		static bool m_alreadyWarned;
};

}

#endif // KHC_TOC_H
// vim:ts=2:sw=2:et
