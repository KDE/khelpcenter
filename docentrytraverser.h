#ifndef DOCENTRYTRAVERSER_H
#define DOCENTRYTRAVERSER_H

class DocEntry;
class DocMetaInfo;

class DocEntryTraverser
{
  public:
    DocEntryTraverser() : mNotifyee( 0 ), mParent( 0 ) {}
    virtual ~DocEntryTraverser() {}
    
    void setNotifyee( DocMetaInfo * );

    virtual void process( DocEntry * ) = 0;

    virtual void startProcess( DocEntry * );

    virtual DocEntryTraverser *createChild() = 0;

    virtual void deleteTraverser();

    virtual void finishTraversal() {}

    DocEntryTraverser *childTraverser();
    DocEntryTraverser *parentTraverser();

  protected:
    DocMetaInfo *mNotifyee;
    DocEntryTraverser *mParent;
};

#endif
