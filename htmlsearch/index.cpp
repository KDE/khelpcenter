#include <qstring.h>

#include <klocale.h>
#include <kglobal.h>
#include <kapp.h>

#include "htmlsearch.h"


int main(int argc, char *argv[])
{
  KGlobal::locale()->setMainCatalogue("htmlsearch");
  KApplication app(argc, argv, "khtmlindex");
  HTMLSearch search;
  
  QString lang="en";

  search.generateIndex(lang);
}




