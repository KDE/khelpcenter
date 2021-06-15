/*
    SPDX-FileCopyrightText: 2002 Cornelius Schumacher <schumacher@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only
*/

#ifndef KHC_DOCMETAINFO_H
#define KHC_DOCMETAINFO_H

#include <QDir>
#include <QMap>

#include "docentry.h"

namespace KHC {

  class DocEntryTraverser;

  /*!
    This class provides some meta information about help documents.
  */
  class DocMetaInfo
  {
    public:
      /*!
	Return instance of DocMetaInfo. There can only be one instance at a time.
      */
      static DocMetaInfo *self();

      ~DocMetaInfo();

      void scanMetaInfo( bool force = false );

      DocEntry *addDocEntry( const QFileInfo &fileInfo );

      void addDocEntry( DocEntry * );

      DocEntry::List docEntries() const;
      
      DocEntry::List searchEntries() const;

      void traverseEntries( DocEntryTraverser * );

      void startTraverseEntries( DocEntryTraverser *traverser );
      void startTraverseEntry( DocEntry *entry, DocEntryTraverser *traverser );
      void endProcess( DocEntry *entry, DocEntryTraverser *traverser );
      void endTraverseEntries( DocEntryTraverser * );

      static QString languageName( const QString &langcode );

    protected:
      DocEntry *scanMetaInfoDir( const QString &filename, DocEntry *parent );
      DocEntry *addDirEntry( const QDir &dir, DocEntry *parent );
      void traverseEntry( DocEntry *, DocEntryTraverser * );

    private:
      /*!
	DocMetaInfo is a singleton. Private constructor prevents direct
	instantisation.
      */
      DocMetaInfo();
    
      DocEntry::List mDocEntries;
      DocEntry::List mSearchEntries;

      DocEntry mRootEntry;

      QStringList mLanguages;

      QMap<QString,QString> mLanguageNames;

      static bool mLoaded;

      static DocMetaInfo *mSelf;
  };

}

#endif //KHC_DOCMETAINFO_H
// vim:ts=2:sw=2:et
