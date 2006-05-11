#ifndef DOCENTRY_H
#define DOCENTRY_H

#include <QString>
#include <QList>

namespace KHC {

class DocEntry
{
  public:
    typedef QList<DocEntry *> List;

    DocEntry();
    
    DocEntry( const QString &name, const QString &url = QString(),
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

    bool readFromFile( const QString &fileName );

    bool indexExists( const QString &indexDir );

    bool docExists() const;

    void addChild( DocEntry * );
    bool hasChildren();
    DocEntry *firstChild();
    List children();
  
    void setParent( DocEntry * );
    DocEntry *parent();
  
    void setNextSibling( DocEntry * );
    DocEntry *nextSibling();

    QString khelpcenterSpecial() const;

    bool isSearchable();
    
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
    int mWeight;
    QString mSearchMethod;
    bool mSearchEnabled;
    bool mSearchEnabledDefault;
    QString mDocumentType;
    bool mDirectory;

    QString mKhelpcenterSpecial;

    List mChildren;
    DocEntry *mParent;
    DocEntry *mNextSibling;
};

}

#endif
// vim:ts=2:sw=2:et
