#include <kaboutdata.h>
#include <kapp.h>
#include <kdebug.h>
#include <klocale.h>
#include <kcmdlineargs.h>

#include "kcmhelpcenter.h"

int main(int argc,char **argv)
{
  KAboutData aboutData("testmetainfo","TestDocMetaInfo","0.1");
  KCmdLineArgs::init(argc,argv,&aboutData);

//  KApplication app( false, false );
  KApplication app;

  KCMHelpCenter m;
}
