#ifndef DOCENTRY_H
#define DOCENTRY_H

#include <qstring.h>
#include <qvaluelist.h>

namespace KHC {

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

    void setWeight( int );
    int weight() const;

    void setSearchMethod( const QString & );
    QString searchMethod() const;

    void enableSearch( bool enabled );
    bool searchEnabled() const;

    void setSearchEnabledDefault( bool enabled );
    bool searchEnabledDefault() const;

    void setDirectory( bool );
    bool isDirectory() const;

    bool readFromFile( const QString &fileName );

    bool indexExists();

    void addChild( DocEntry * );
    bool hasChildren();
    DocEntry *firstChild();
    List children();
  
    void setParent( DocEntry * );
    DocEntry *parent();
  
    void setNextSibling( DocEntry * );
    DocEntry *nextSibling();

    QString khelpcenterSpecial() const;
    
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
    int mWeight;
    QString mSearchMethod;
    bool mSearchEnabled;
    bool mSearchEnabledDefault;
    bool mDirectory;

    QString mKhelpcenterSpecial;

    List mChildren;
    DocEntry *mParent;
    DocEntry *mNextSibling;
};

}

#endif
// vim:ts=2:sw=2:et
