#ifndef DOCENTRYTRAVERSER_H
#define DOCENTRYTRAVERSER_H

class DocEntry;
class DocMetaInfo;

class DocEntryTraverser
{
  public:
    DocEntryTraverser() : mNotifyee( 0 ), mParent( 0 ), mParentEntry( 0 ) {}
    virtual ~DocEntryTraverser() {}
    
    void setNotifyee( DocMetaInfo * );

    virtual void process( DocEntry * ) = 0;

    virtual void startProcess( DocEntry * );

    virtual DocEntryTraverser *createChild( DocEntry *parentEntry ) = 0;

    virtual void deleteTraverser();

    virtual void finishTraversal() {}

    DocEntryTraverser *childTraverser( DocEntry *parentEntry );
    virtual DocEntryTraverser *parentTraverser();

    void setParentEntry( DocEntry * );
    DocEntry *parentEntry();

  protected:
    DocMetaInfo *mNotifyee;
    DocEntryTraverser *mParent;

  private:
    DocEntry *mParentEntry;    
};

#endif
