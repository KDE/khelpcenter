/*
    SPDX-FileCopyrightText: 2002 Frerich Raabe <raabe@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KHC_GLOSSARY_H
#define KHC_GLOSSARY_H

#include <QDomElement>
#include <QHash>
#include <QList>
#include <QProcess>
#include <QTreeWidget>

class EntryItem;

namespace KHC {

  class GlossaryEntryXRef
  {
      public:
	  typedef QList<GlossaryEntryXRef> List;

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

  class GlossaryEntry
  {
    public:
      GlossaryEntry() {}
      GlossaryEntry( const QString &id, const QString &term,
	      const QString &definition,
	      const GlossaryEntryXRef::List &seeAlso ) :
	  m_id( id ),
	  m_term( term ),
	  m_definition( definition ),
	  m_seeAlso( seeAlso )
	  { }

	QString id() const { return m_id; }
	QString term() const { return m_term; }
	QString definition() const { return m_definition; }
	GlossaryEntryXRef::List seeAlso() const { return m_seeAlso; }
    
    private:
	QString m_id;
	QString m_term;
	QString m_definition;
	GlossaryEntryXRef::List m_seeAlso;
  };

  class Glossary : public QTreeWidget
  {
    Q_OBJECT
    public:
      explicit Glossary( QWidget *parent );
      virtual ~Glossary();

      const GlossaryEntry &entry( const QString &id ) const;

    public Q_SLOTS:
      void slotSelectGlossEntry( const QString &id );

    Q_SIGNALS:
      void entrySelected( const GlossaryEntry &entry );
	
    private Q_SLOTS:
      void meinprocFinished(int exitCode, QProcess::ExitStatus exitStatus);
      void treeItemSelected( QTreeWidgetItem *item );

    protected:
      void showEvent(QShowEvent *event) override;
		
    private:
      enum CacheStatus { NeedRebuild, CacheOk };

      CacheStatus cacheStatus() const;
      int glossaryCTime() const;
      void rebuildGlossaryCache();
      void buildGlossaryTree();
      static QDomElement childElement( const QDomElement &e, const QString &name );

      QTreeWidgetItem *m_byTopicItem;
      QTreeWidgetItem *m_alphabItem;
      QString m_sourceFile;
      QString m_cacheFile;
      CacheStatus m_status;
      QHash<QString, GlossaryEntry*> m_glossEntries;
      QHash<QString, EntryItem*> m_idDict;
      bool m_initialized;
      static bool m_alreadyWarned;
  };

}

#endif // KHC_GLOSSARY_H
// vim:ts=2:sw=2:et
