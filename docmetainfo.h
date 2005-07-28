#ifndef DOCMETAINFO_H
#define DOCMETAINFO_H

#include <qstring.h>
#include <qstringlist.h>
#include <q3valuelist.h>
#include <qdir.h>
#include <qmap.h>

#include "docentry.h"

namespace KHC {

class HTMLSearch;
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

    DocEntry *addDocEntry( const QString &fileName );

    void addDocEntry( DocEntry * );

    DocEntry::List docEntries();
    
    DocEntry::List searchEntries();

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

    HTMLSearch *mHtmlSearch;

    static bool mLoaded;

    static DocMetaInfo *mSelf;
};

}

#endif
// vim:ts=2:sw=2:et
