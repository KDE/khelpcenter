
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <kglobal.h>
#include <kapplication.h>
#include <klocale.h>

#include "htmlsearch.h"

static KCmdLineOptions options[] =
{
  { "lang <lang>", I18N_NOOP("The language to index"), "en" },
   KCmdLineLastOption // End of options.
};


int main(int argc, char *argv[])
{
  KAboutData aboutData( "khtmlindex", I18N_NOOP("KHtmlIndex"),
	"",
	I18N_NOOP("KDE Index generator for help files."));

  KCmdLineArgs::init(argc, argv, &aboutData);
  KCmdLineArgs::addCmdLineOptions( options );

  KGlobal::locale()->setMainCatalog("htmlsearch");
  KApplication app;
  HTMLSearch search;

  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
  search.generateIndex(args->getOption("lang"));
}
