/*
 *  glossary.h - part of the KDE Help Center
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
#ifndef KHC_GLOSSARY_H
#define KHC_GLOSSARY_H

#include <klistview.h>

#include <qdict.h>
#include <qdom.h>
#include <qstringlist.h>

class KConfig;
class KProcess;

namespace KHC {

class GlossaryEntryXRef
{
	friend QDataStream &operator>>( QDataStream &, GlossaryEntryXRef & );
	public:
		typedef QValueList<GlossaryEntryXRef> List;

		GlossaryEntryXRef() {}
		GlossaryEntryXRef( const QString &term, const QString &id ) :
			m_term( term ),
			m_id( id )
		{
		}

		QString term() const { return m_term; }
		QString id() const { return m_id; }
	
	private:
		QString m_term;
		QString m_id;
};

inline QDataStream &operator<<( QDataStream &stream, const GlossaryEntryXRef &e )
{
	return stream << e.term() << e.id();
}

inline QDataStream &operator>>( QDataStream &stream, GlossaryEntryXRef &e )
{
	return stream >> e.m_term >> e.m_id;
}

class GlossaryEntry
{
	friend QDataStream &operator>>( QDataStream &, GlossaryEntry & );
	public:
		GlossaryEntry() {}
		GlossaryEntry( const QString &term, const QString &definition,
				const GlossaryEntryXRef::List &seeAlso ) :
			m_term( term ),
			m_definition( definition ),
			m_seeAlso( seeAlso )
			{
			}

		QString term() const { return m_term; }
		QString definition() const { return m_definition; }
		GlossaryEntryXRef::List seeAlso() const { return m_seeAlso; }
	
	private:
		QString m_term;
		QString m_definition;
		GlossaryEntryXRef::List m_seeAlso;
};

inline QDataStream &operator<<( QDataStream &stream, const GlossaryEntry &e )
{
	return stream << e.term() << e.definition() << e.seeAlso();
}

inline QDataStream &operator>>( QDataStream &stream, GlossaryEntry &e )
{
	return stream >> e.m_term >> e.m_definition >> e.m_seeAlso;
}

class Glossary : public KListView
{
	Q_OBJECT
	public:
		Glossary( QWidget *parent );
		virtual ~Glossary();

		const GlossaryEntry &entry( const QString &id ) const;
 
    static QString entryToHtml( const GlossaryEntry &entry );

	signals:
		void entrySelected( const GlossaryEntry &entry );
		
	private slots:
		void meinprocExited( KProcess *meinproc );
		void treeItemSelected( QListViewItem *item );

	private:
		enum CacheStatus { NeedRebuild, CacheOk };

		CacheStatus cacheStatus() const;
		int glossaryCTime() const;
		void rebuildGlossaryCache();
		void buildGlossaryTree();
		QDomElement childElement( const QDomElement &e, const QString &name );

		KConfig *m_config;
		QListViewItem *m_byTopicItem;
		QListViewItem *m_alphabItem;
		QString m_sourceFile;
		QString m_cacheFile;
		CacheStatus m_status;
		QDict<GlossaryEntry> m_glossEntries;
};

}

#endif // KHC_GLOSSARY_H
// vim:ts=2:sw=2:et
