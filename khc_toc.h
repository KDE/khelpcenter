/*
 *  khc_toc.h - part of the KDE Help Center
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

#include <klistview.h>

#include <qbuffer.h>
#include <qdom.h>

class KProcess;

class khcTOC : public KListView
{
	Q_OBJECT
	public:
		khcTOC( QWidget *parent );

		QString application() const { return m_application; }
		void setApplication( const QString &application ) { m_application = application; }
	
	public slots:
		void reset();
		void build( const QString &file );
		
	signals:
		void itemSelected( const QString &url );

	private slots:
		void slotItemSelected( QListViewItem *item );
		void gotMeinprocOutput( KProcess *meinproc, char *data, int len );
		void meinprocExited( KProcess *meinproc );

	private:
		void fill( const QDomDocument &doc );

		QString m_application;
		QBuffer m_meinprocBuffer;
};

class khcTOCItem : public KListViewItem
{
	public:
		khcTOCItem( khcTOCItem *parent, const QString &text );
		khcTOCItem( khcTOCItem *parent, QListViewItem *after, const QString &text );
		khcTOCItem( khcTOC *parent, const QString &text );
		khcTOCItem( khcTOC *parent, QListViewItem *after, const QString &text );

		virtual QString link() const = 0;

		khcTOC *toc() const;
};

class khcTOCChapterItem : public khcTOCItem
{
	public:
		khcTOCChapterItem( khcTOC *parent, const QString &title, const QString &name );
		khcTOCChapterItem( khcTOC *parent, QListViewItem *after, const QString &title, const QString &name );

		virtual QString link() const;
		
		virtual void setOpen( bool open );
	
	private:
		QString m_name;
};

class khcTOCSectionItem : public khcTOCItem
{
	public:
		khcTOCSectionItem( khcTOCChapterItem *parent, const QString &title, const QString &name );
		khcTOCSectionItem( khcTOCChapterItem *parent, QListViewItem *after, const QString &title, const QString &name );

		virtual QString link() const;
	
	private:
		QString m_name;
};

#endif // KHC_TOC_H
// vim:ts=4:sw=4:noet
