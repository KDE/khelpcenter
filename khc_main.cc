/*
 *  khc_main.cpp - part of the KDE Help Center
 *
 *  Copyright (C) 1999 Matthias Elter (me@kde.org)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdlib.h>

#include <kapp.h>
#include <dcopclient.h>
#include <kglobal.h>
#include <klocale.h>
#include <kstddirs.h>
#include <kcmdlineargs.h>

#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>

#include "khc_main.h"
#include "KonquerorIface_stub.h"

#include <kaboutdata.h>

#include "version.h"

static const char *description = 
	I18N_NOOP("KDE Help Center");

void Listener::slotAppRegistered( const QCString &appId )
{
  if ( appId.left( 9 ) == "konqueror" )
  {
    qDebug( "found it!!!! %s", appId.data() );

    QString tempProfile = locateLocal( "appdata", "konqprofile.tmp" );
    QFile f( tempProfile );
    f.open( IO_WriteOnly );

    QTextStream str( &f );

    str << "[Profile]" << endl;
    str << "Container0_Children=View1,View2" << endl;
    str << "Container0_Orientation=Horizontal" << endl;
    str << "Container0_SplitterSizes=30,70" << endl;
    str << "RootItem=Container0" << endl;
    str << "View1_PassiveMode=true" << endl;
    str << "View1_ServiceName=KHelpcenter" << endl;
    str << "View1_ServiceType=Browser/View" << endl;
    str << "View1_URL=file:/home/shaus" << endl;
    str << "View2_PassiveMode=false" << endl;
    str << "View2_ServiceName=KonqHTMLView" << endl;
    str << "View2_ServiceType=text/html" << endl;
    str << "View2_URL=" << QString("file:" + locate("html", "default/khelpcenter/main.html")) << endl;

    f.close();

    KonquerorIface_stub stub( appId, "KonquerorIface" );
    stub.createBrowserWindowFromProfile( tempProfile );

    exit( 0 );
  }
}

int main(int argc, char *argv[])
{
  KAboutData aboutData( "khelpcenter", I18N_NOOP("KDE HelpCenter"), 
    HELPCENTER_VERSION, description, KAboutData::GPL, 
    "(c) 1999-2000, Matthias Elter");
  aboutData.addAuthor("Matthias Elter",0, "me@kde.org");
  KCmdLineArgs::init( argc, argv, &aboutData );
//  KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.
  
  KApplication app( false, false ); // no GUI in this process

  app.dcopClient()->attach();
  // we want to get applicationRegistered
  app.dcopClient()->setNotifications( true );

  Listener listener;

  QObject::connect( app.dcopClient(), SIGNAL( applicationRegistered( const QCString& ) ),
	   &listener, SLOT( slotAppRegistered( const QCString & ) ) );


  system( "konqueror --silent &" );

  app.exec();
}

#include "khc_main.moc"
