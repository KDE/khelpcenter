/*
 *  khc_glossary.cc - part of the KDE Help Center
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
#include "khc_glossary.h"

#include <kapplication.h>
#include <kcharsets.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <klistview.h>
#include <klocale.h>
#include <kprocess.h>
#include <kstandarddirs.h>

#include <qdom.h>
#include <qfileinfo.h>
#include <qheader.h>
#include <qregexp.h>

#include <sys/stat.h>

namespace {

QString langLookup(const QString &fname)
{
	QStringList search;

	// assemble the local search paths
	const QStringList localDoc = KGlobal::dirs()->resourceDirs("html");

	// look up the different languages
	for (int id=localDoc.count()-1; id >= 0; --id)
	{
		QStringList langs = KGlobal::locale()->languageList();
		langs.append("default");
		langs.append("en");
		QStringList::ConstIterator lang;
		for (lang = langs.begin(); lang != langs.end(); ++lang)
			search.append(QString("%1%2/%3").arg(localDoc[id]).arg(*lang).arg(fname));
	}

	// try to locate the file
	QStringList::Iterator it;
	for (it = search.begin(); it != search.end(); ++it)
	{
		kdDebug(1400) << "Looking for help in: " << *it << endl;

		QFileInfo info(*it);
		if (info.exists() && info.isFile() && info.isReadable())
			return *it;

		QString file = (*it).left((*it).findRev('/')) + "/index.docbook";
		kdDebug(1400) << "Looking for help in: " << file << endl;
		info.setFile(file);
		if (info.exists() && info.isFile() && info.isReadable())
			return *it;
	}

	return QString::null;
}

}; // anonymous namespace

class SectionItem : public QListViewItem
{
	public:
		SectionItem( QListViewItem *parent, const QString &text )
			: QListViewItem( parent, text )
		{
			setOpen( false );
		}
		
		virtual void setOpen( bool open )
		{
				QListViewItem::setOpen(open);
				
				setPixmap( 0, SmallIcon( QString::fromLatin1( open ? "contents" : "contents2" ) ) );

		}
};

khcGlossary::khcGlossary( QWidget *parent ) : KListView( parent )
{
	connect( this, SIGNAL( executed( QListViewItem * ) ),
	         this, SLOT( treeItemSelected( QListViewItem * ) ) );
	connect( this, SIGNAL( executed( QListViewItem * ) ),
	        this, SLOT( treeItemSelected( QListViewItem * ) ) );
	
	setFrameStyle( QFrame::Panel | QFrame::Sunken );
	addColumn( QString::null );
	header()->hide();
	setAllColumnsShowFocus( true );
	setRootIsDecorated( true );

	m_byTopicItem = new QListViewItem( this, i18n( "By topic" ) );
	m_byTopicItem->setPixmap( 0, SmallIcon( "help" ) );

	m_alphabItem = new QListViewItem( this, i18n( "Alphabetically" ) );
	m_alphabItem->setPixmap( 0, SmallIcon( "charset" ) );

	m_cacheFile = locateLocal( "cache", "help/glossary.xml" );
	kdDebug( 1400 ) << "*** GLOSSARY CACHE: " << m_cacheFile << endl;

	m_sourceFile = langLookup( QString::fromLatin1( "khelpcenter/glossary/index.docbook" ) );

	m_config = kapp->config();
	m_config->setGroup( "Glossary" );

	if ( cacheStatus() == NeedRebuild ) {
		kdDebug( 1400 ) << "Rebuilding glossary cache, cache file = " << m_cacheFile << endl;
		rebuildGlossaryCache();
	} else
		buildGlossaryTree();
}

khcGlossary::~khcGlossary()
{
	m_glossEntries.setAutoDelete( true );
	m_glossEntries.clear();
}

const khcGlossaryEntry &khcGlossary::entry( const QString &term ) const
{
	return *m_glossEntries[ term ];
}

khcGlossary::CacheStatus khcGlossary::cacheStatus() const
{
	if ( !QFile::exists( m_cacheFile ) ||
	     m_config->readEntry( "CachedGlossary" ) != m_sourceFile ||
	     m_config->readNumEntry( "CachedGlossaryTimestamp" ) != glossaryCTime() )
		return NeedRebuild;

	return CacheOk;
}

int khcGlossary::glossaryCTime() const
{
	struct stat stat_buf;
	stat( QFile::encodeName( m_sourceFile ).data(), &stat_buf );

	return stat_buf.st_ctime;
}

void khcGlossary::rebuildGlossaryCache()
{
	KProcess *meinproc = new KProcess;
	connect( meinproc, SIGNAL( processExited( KProcess * ) ),
	         this, SLOT( meinprocExited( KProcess * ) ) );

	*meinproc << locate( "exe", QString::fromLatin1( "meinproc" ) );
	*meinproc << QString::fromLatin1( "--output" ) << m_cacheFile;
	*meinproc << QString::fromLatin1( "--stylesheet" )
	          << locate( "data", QString::fromLatin1( "khelpcenter/glossary.xslt" ) );
	*meinproc << m_sourceFile;

	meinproc->start( KProcess::NotifyOnExit );
}

void khcGlossary::meinprocExited( KProcess *meinproc )
{
	delete meinproc;

	if ( !QFile::exists( m_cacheFile ) )
		return;

	m_config->writeEntry( "CachedGlossary", m_sourceFile );
	m_config->writeEntry( "CachedGlossaryTimestamp", glossaryCTime() );
	m_config->sync();
	
	m_status = CacheOk;

	buildGlossaryTree();
}

void khcGlossary::buildGlossaryTree()
{
	QFile cacheFile(m_cacheFile);
	if ( !cacheFile.open( IO_ReadOnly ) )
		return;

	QDomDocument doc;
	if ( !doc.setContent( &cacheFile ) )
		return;

	QDomNodeList sectionNodes = doc.documentElement().elementsByTagName( QString::fromLatin1( "section" ) );
	for ( unsigned int i = 0; i < sectionNodes.count(); i++ ) {
		QDomElement sectionElement = sectionNodes.item( i ).toElement();
		QString title = sectionElement.attribute( QString::fromLatin1( "title" ) );
		SectionItem *topicSection = new SectionItem( m_byTopicItem, title );

		QDomNodeList entryNodes = sectionElement.elementsByTagName( QString::fromLatin1( "entry" ) );
		for ( unsigned int j = 0; j < entryNodes.count(); j++ ) {
			QDomElement entryElement = entryNodes.item( j ).toElement();
			QDomElement termElement = childElement( entryElement, QString::fromLatin1( "term" ) );
			QString term = termElement.text().simplifyWhiteSpace();

			new QListViewItem(topicSection, term);

			SectionItem *alphabSection = 0L;
			for ( QListViewItemIterator it( m_alphabItem ); it.current(); it++ )
				if ( it.current()->text( 0 ) == term[ 0 ].upper() ) {
					alphabSection = static_cast<SectionItem *>( it.current() );
					break;
				}

			if ( !alphabSection )
				alphabSection = new SectionItem( m_alphabItem, term[ 0 ].upper() );

			new QListViewItem( alphabSection, term );

			QDomElement definitionElement = childElement( entryElement, QString::fromLatin1( "definition" ) );
			QString definition = definitionElement.text().simplifyWhiteSpace();

			QStringList seeAlso;

			QDomElement referencesElement = childElement( entryElement, QString::fromLatin1( "references" ) );
			QDomNodeList referenceNodes = referencesElement.elementsByTagName( QString::fromLatin1( "reference" ) );
			if ( referenceNodes.count() > 0 )
				for ( unsigned int k = 0; k < referenceNodes.count(); k++ ) {
					QDomElement referenceElement = referenceNodes.item( k ).toElement();
					seeAlso += referenceElement.attribute( QString::fromLatin1( "term" ) );
				}
					
			m_glossEntries.insert( term, new khcGlossaryEntry( term, definition, seeAlso ) );
		}
	}
}

void khcGlossary::treeItemSelected( QListViewItem *item )
{
	if ( !item )
		return;

	if ( dynamic_cast<SectionItem *>( item->parent() ) )
		emit entrySelected( entry( item->text( 0 ) ) );

	item->setOpen( !item->isOpen() );
}
	
QDomElement khcGlossary::childElement( const QDomElement &element, const QString &name )
{
	QDomElement e;
	for ( e = element.firstChild().toElement(); !e.isNull(); e = e.nextSibling().toElement() )
		if ( e.tagName() == name )
			break;
	return e;
}
#include "khc_glossary.moc"
// vim:ts=4:sw=4:noet
