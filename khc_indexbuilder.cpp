/*
  This file is part of the KDE Help Center
 
  Copyright (c) 2003 Cornelius Schumacher <schumacher@kde.org>
 
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "khc_indexbuilder.h"

#include "version.h"

#include <kaboutdata.h>
#include <klocale.h>
#include <kcmdlineargs.h>
#include <kuniqueapplication.h>
#include <kdebug.h>
#include <dcopclient.h>
#include <kprocess.h>
#include <kconfig.h>

#include <qfile.h>
#include <qtextstream.h>

#include <unistd.h>
#include <stdlib.h>
#include <iostream>

using namespace KHC;

IndexBuilder::IndexBuilder()
{
  kdDebug(1402) << "IndexBuilder()" << endl;
}

void IndexBuilder::buildIndices( const QString &cmdFile )
{
  QFile f( cmdFile );
  if ( !f.open( IO_ReadOnly ) ) {
    kdError() << "Unable to open file '" << cmdFile << "'" << endl;
    exit( 1 );
  }
  kdDebug(1402) << "Opened file '" << cmdFile << "'" << endl;
  QTextStream ts( &f );
  QString line = ts.readLine();
  while ( !line.isNull() ) {
    kdDebug(1402) << "LINE: " << line << endl;
    mCmdQueue.append( line );
    line = ts.readLine();
  }

  processCmdQueue();
}

void IndexBuilder::processCmdQueue()
{
  kdDebug(1402) << "IndexBuilder::processCmdQueue()" << endl;

  QStringList::Iterator it = mCmdQueue.begin();

  if ( it == mCmdQueue.end() ) {
    quit();
    return;
  }

  QString cmd = *it;

  kdDebug(1402) << "PROCESS: " << cmd << endl;

  KProcess *proc = new KProcess;
  proc->setRunPrivileged( true );

  QStringList args = QStringList::split( " ", cmd );
  *proc << args;

  connect( proc, SIGNAL( processExited( KProcess * ) ),
           SLOT( slotProcessExited( KProcess * ) ) );
  connect( proc, SIGNAL( receivedStdout(KProcess *, char *, int ) ),
           SLOT( slotReceivedStdout(KProcess *, char *, int ) ) );
  connect( proc, SIGNAL( receivedStderr(KProcess *, char *, int ) ),
           SLOT( slotReceivedStderr(KProcess *, char *, int ) ) );

  proc->start( KProcess::NotifyOnExit, KProcess::AllOutput );

  mCmdQueue.remove( it );
}

void IndexBuilder::slotProcessExited( KProcess *proc )
{
  kdDebug(1402) << "IndexBuilder::slotIndexFinished()" << endl;

  if ( !proc->normalExit() ) {
    kdError(1402) << "Process failed" << endl;
  } else {
    int status = proc->exitStatus();
    kdDebug(1402) << "Exit status: " << status << endl;
  }

  delete proc;

  sendProgressSignal();

  processCmdQueue();
}

void IndexBuilder::slotReceivedStdout( KProcess *, char *buffer, int buflen )
{
  QString text = QString::fromLocal8Bit( buffer, buflen );
  std::cout << text.local8Bit().data() << std::flush;
}

void IndexBuilder::slotReceivedStderr( KProcess *, char *buffer, int buflen )
{
  QString text = QString::fromLocal8Bit( buffer, buflen );
  std::cerr << text.local8Bit().data() << std::flush;
}

void IndexBuilder::sendProgressSignal()
{
  kdDebug(1402) << "IndexBuilder::sendProgressSignal()" << endl;
  
  kapp->dcopClient()->emitDCOPSignal("buildIndexProgress()", QByteArray() );  
}

void IndexBuilder::quit()
{
  kdDebug(1402) << "IndexBuilder::quit()" << endl;

  kapp->quit();
}


static KCmdLineOptions options[] =
{
  { "+cmdfile", I18N_NOOP("Document to be indexed."), 0 },
  { "+indexdir", I18N_NOOP("Index directory"), 0 },
  { 0,0,0 }
};

int main( int argc, char **argv )
{
  KAboutData aboutData( "khc_indexbuilder",
                        I18N_NOOP("KHelpcCenter Index Builder"),
                        HELPCENTER_VERSION,
                        I18N_NOOP("The KDE Help Center"),
                        KAboutData::License_GPL,
                        I18N_NOOP("(c) 2003, The KHelpCenter developers") );

  aboutData.addAuthor( "Cornelius Schumacher", 0, "schumacher@kde.org" );

  KCmdLineArgs::init( argc, argv, &aboutData );
  KCmdLineArgs::addCmdLineOptions( options );
  KUniqueApplication::addCmdLineOptions();

  KApplication app;

  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

  if ( args->count() != 2 ) {
    kdDebug(1402) << "Wrong number of arguments." << endl;
    return 1;
  }

  QString cmdFile = args->arg( 0 );
  QString indexDir = args->arg( 1 );

  kdDebug(1402) << "cmdFile: " << cmdFile << endl;
  kdDebug(1402) << "indexDir: " << indexDir << endl;

  QFile file( indexDir + "/testaccess" );
  if ( !file.open( IO_WriteOnly ) || file.putch( ' ' ) < 0 ) {
    kdDebug(1402) << "access denied" << endl;
    return 2;
  } else {
    kdDebug(1402) << "can access" << endl;
    file.remove();
  }
  
  app.dcopClient()->registerAs( "khc_indexbuilder", false );

  IndexBuilder builder;

  builder.buildIndices( cmdFile );

  return app.exec();
}

#include "khc_indexbuilder.moc"

// vim:ts=2:sw=2:et
