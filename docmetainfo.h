#ifndef __DOCMETAINFO_H__
#define __DOCMETAINFO_H__

#include <qobject.h>
#include <qptrlist.h>
#include <qstring.h>
#include <kpixmap.h>

class DocEntry
{
  public:
    typedef QValueList<DocEntry *> List;

    DocEntry();
    
    void setName( const QString & );
    QString name() const;
    
    void setSearch( const QString & );
    QString search() const;
    
    void setIcon( const QString & );
    QString icon() const;
    
    void setUrl( const QString & );
    QString url() const;

    void setDocPath( const QString & );
    QString docPath() const;

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

    void enableSearch( bool enabled );
    bool searchEnabled() const;

    bool readFromFile( const QString &fileName );

    bool indexExists();
  
    void dump() const;
      
  private:
    QString mName;
    QString mSearch;
    QString mIcon;
    QString mUrl;
    QString mDocPath;
    QString mInfo;
    QString mLang;
    QString mIdentifier;
    QString mIndexer;
    QString mIndexTestFile;
    bool mSearchEnabled;
};


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

    DocEntry *addDocEntry( const QString &fileName );

    DocEntry::List docEntries();
    
    DocEntry::List searchEntries();
    
  private:
    /*!
      DocMetaInfo is a singleton. Private constructor prevents direct
      instantisation.
    */
    DocMetaInfo();
  
    DocEntry::List mDocEntries;
    DocEntry::List mSearchEntries;

    static DocMetaInfo *mSelf;
};

#endif
