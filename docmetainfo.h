#ifndef DOCMETAINFO_H
#define DOCMETAINFO_H

#include <qstring.h>
#include <qstringlist.h>
#include <qvaluelist.h>

#include "docentry.h"

class HTMLSearch;

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

    void scanMetaInfo( const QStringList &languages = QStringList() );

    DocEntry *addDocEntry( const QString &fileName );

    void addDocEntry( DocEntry * );

    DocEntry::List docEntries();
    
    DocEntry::List searchEntries();

    void traverseEntries( DocEntryTraverser * );

    void startTraverseEntries( DocEntryTraverser *traverser );
    void startTraverseEntry( DocEntry *entry, DocEntryTraverser *traverser );
    void endProcess( DocEntry *entry, DocEntryTraverser *traverser );
    void endTraverseEntries( DocEntryTraverser * );

  protected:
    DocEntry *scanMetaInfoDir( const QString &filename, DocEntry *parent );
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

    HTMLSearch *mHtmlSearch;

    static DocMetaInfo *mSelf;
};

}

#endif
