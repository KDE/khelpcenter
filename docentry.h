/*
    SPDX-FileCopyrightText: 2002 Cornelius Schumacher <schumacher@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KHC_DOCENTRY_H
#define KHC_DOCENTRY_H

#include <QFileInfo>
#include <QList>
#include <QString>

namespace KHC {

  class DocEntry
  {
    public:
      typedef QList<DocEntry *> List;

      DocEntry();

      explicit DocEntry( const QString &name, const QString &url = QString(),
		const QString &icon = QString() );

      void setName( const QString & );
      QString name() const;

      void setSearch( const QString & );
      QString search() const;

      void setIcon( const QString & );
      QString icon() const;

      void setUrl( const QString & );
      QString url() const;

      void setInfo( const QString & );
      QString info() const;

      void setLang( const QString & );
      QString lang() const;

      void setIdentifier( const QString & );
      QString identifier() const;

      void setIndexer( const QString & );
      QString indexer() const;

      void setIndexTestFile( const QString & );
      QString indexTestFile() const;

      void setWeight( int );
      int weight() const;

      void setSearchMethod( const QString & );
      QString searchMethod() const;

      void enableSearch( bool enabled );
      bool searchEnabled() const;

      void setSearchEnabledDefault( bool enabled );
      bool searchEnabledDefault() const;

      void setDocumentType( const QString & );
      QString documentType() const;

      void setDirectory( bool );
      bool isDirectory() const;

      bool readFromFile( const QFileInfo &fileInfo );

      bool docExists() const;

      void addChild( DocEntry * );
      bool hasChildren() const;
      DocEntry *firstChild() const;
      List children() const;

      void setParent( DocEntry * );
      DocEntry *parent() const;

      void setNextSibling( DocEntry * );
      DocEntry *nextSibling() const;

      QString khelpcenterSpecial() const;

      bool isSearchable() const;

      void dump() const;

    protected:
      void init();

    private:
      QString mName;
      QString mSearch;
      QString mIcon;
      QString mUrl;
      QString mInfo;
      QString mLang;
      mutable QString mIdentifier;
      QString mIndexer;
      QString mIndexTestFile;
      QString mSearchMethod;
      QString mDocumentType;

      QString mKhelpcenterSpecial;

      List mChildren;
      DocEntry *mParent = nullptr;
      DocEntry *mNextSibling = nullptr;

      int mWeight;
      bool mSearchEnabled : 1;
      bool mSearchEnabledDefault : 1;
      bool mDirectory : 1;
  };

}

#endif //KHC_DOCENTRY_H
// vim:ts=2:sw=2:et
