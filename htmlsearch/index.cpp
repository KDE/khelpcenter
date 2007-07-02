
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <kglobal.h>
#include <kapplication.h>
#include <klocale.h>

#include "htmlsearch.h"


int main(int argc, char *argv[])
{
  KAboutData aboutData( "khtmlindex", "htmlsearch", ki18n("KHtmlIndex"),
	"",
	ki18n("KDE Index generator for help files."));

  KCmdLineArgs::init(argc, argv, &aboutData);

  KCmdLineOptions options;
  options.add("lang <lang>", ki18n("The language to index"), "en");
  KCmdLineArgs::addCmdLineOptions( options );

  KApplication app;
  HTMLSearch search;

  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
  search.generateIndex(args->getOption("lang"));
}
