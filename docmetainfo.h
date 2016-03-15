
/* This file is part of the KDE project
   Copyright 2002 Cornelius Schumacher <schumacher@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License version 2 or at your option version 3 as published
   by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
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

      QString mDefaultLanguage;

      QMap<QString,QString> mLanguageNames;

      static bool mLoaded;

      static DocMetaInfo *mSelf;
  };

}

#endif //KHC_DOCMETAINFO_H
// vim:ts=2:sw=2:et
