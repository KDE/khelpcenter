#include <qstring.h>


#include <kapp.h>


#include "htmlsearch.h"


int main(int argc, char *argv[])
{
  KApplication app(argc, argv, "khtmlindex");
  HTMLSearch search;
  
  QString lang="en";

  search.generateIndex(lang);
}




