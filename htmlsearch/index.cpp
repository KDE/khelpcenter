
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <kglobal.h>
#include <kapplication.h>

#include "htmlsearch.h"

static KCmdLineOptions options[] =
{
  { "lang <lang>", I18N_NOOP("The language to index.."), "en" },
  { 0, 0, 0 } // End of options.
};


int main(int argc, char *argv[])
{
  KAboutData aboutData( "khtmlindex", I18N_NOOP("KHtmlIndex"),
	"$Id$",
	I18N_NOOP("KDE Index generator for help files."));

  KCmdLineArgs::init(argc, argv, &aboutData);
  KCmdLineArgs::addCmdLineOptions( options );

  KGlobal::locale()->setMainCatalogue("htmlsearch");
  KApplication app;
  HTMLSearch search;

  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
  search.generateIndex(args->getOption("lang"));
}
