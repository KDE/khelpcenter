#include <qstring.h>

#include <klocale.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <kglobal.h>
#include <kapp.h>

#include "htmlsearch.h"


int main(int argc, char *argv[])
{
  KAboutData aboutData( "khtmlindex", I18N_NOOP("KHtmlIndex"),
	"$Id:  $",
	I18N_NOOP("KDE Index generator for help files."));

  KCmdLineArgs::init(argc, argv, &aboutData);

  KGlobal::locale()->setMainCatalogue("htmlsearch");
  KApplication app;
  HTMLSearch search;
  
  QString lang="en";

  search.generateIndex(lang);
}




