#ifndef SEARCHFORMATTER_H
#define SEARCHFORMATTER_H

#include <qstring.h>

class SearchFormatter
{
  public:
    SearchFormatter();
    virtual ~SearchFormatter();

    virtual QString header();
    virtual QString footer();  
    virtual QString separator();
    virtual QString docTitle( const QString & );
    virtual QString sectionHeader( const QString & );
    
    virtual QString processResult( const QString & );
};

#endif
