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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA  02110-1301, USA
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
  kDebug(1402) << "IndexBuilder()" << endl;
}

void IndexBuilder::buildIndices( const QString &cmdFile )
{
  QFile f( cmdFile );
  if ( !f.open( QIODevice::ReadOnly ) ) {
    kError() << "Unable to open file '" << cmdFile << "'" << endl;
    exit( 1 );
  }
  kDebug(1402) << "Opened file '" << cmdFile << "'" << endl;
  QTextStream ts( &f );
  QString line = ts.readLine();
  while ( !line.isNull() ) {
    kDebug(1402) << "LINE: " << line << endl;
    mCmdQueue.append( line );
    line = ts.readLine();
  }

  processCmdQueue();
}

void IndexBuilder::processCmdQueue()
{
  kDebug(1402) << "IndexBuilder::processCmdQueue()" << endl;

  QStringList::Iterator it = mCmdQueue.begin();

  if ( it == mCmdQueue.end() ) {
    quit();
    return;
  }

  QString cmd = *it;

  kDebug(1402) << "PROCESS: " << cmd << endl;

  KProcess *proc = new KProcess;
  proc->setRunPrivileged( true );

  QStringList args = cmd.split( " ");
  *proc << args;

  connect( proc, SIGNAL( processExited( KProcess * ) ),
           SLOT( slotProcessExited( KProcess * ) ) );
  connect( proc, SIGNAL( receivedStdout(KProcess *, char *, int ) ),
           SLOT( slotReceivedStdout(KProcess *, char *, int ) ) );
  connect( proc, SIGNAL( receivedStderr(KProcess *, char *, int ) ),
           SLOT( slotReceivedStderr(KProcess *, char *, int ) ) );

  mCmdQueue.remove( it );

  if ( !proc->start( KProcess::NotifyOnExit, KProcess::AllOutput ) ) {
    sendErrorSignal( i18n("Unable to start command '%1'.", cmd ) );
    processCmdQueue();
  }
}

void IndexBuilder::slotProcessExited( KProcess *proc )
{
  kDebug(1402) << "IndexBuilder::slotIndexFinished()" << endl;

  if ( !proc->normalExit() ) {
    kError(1402) << "Process failed" << endl;
  } else {
    int status = proc->exitStatus();
    kDebug(1402) << "Exit status: " << status << endl;
  }

  delete proc;

  sendProgressSignal();

  processCmdQueue();
}

void IndexBuilder::slotReceivedStdout( KProcess *, char *buffer, int buflen )
{
  QString text = QString::fromLocal8Bit( buffer, buflen );
  std::cout << text.toLocal8Bit().data() << std::flush;
}

void IndexBuilder::slotReceivedStderr( KProcess *, char *buffer, int buflen )
{
  QString text = QString::fromLocal8Bit( buffer, buflen );
  std::cerr << text.toLocal8Bit().data() << std::flush;
}

void IndexBuilder::sendErrorSignal( const QString &error )
{
  kDebug(1402) << "IndexBuilder::sendErrorSignal()" << endl;
  
  QByteArray params;
  QDataStream stream( &params, QIODevice::WriteOnly );

  stream.setVersion(QDataStream::Qt_3_1);
  stream << error;
  kapp->dcopClient()->emitDCOPSignal("buildIndexError(QString)", params );  
}

void IndexBuilder::sendProgressSignal()
{
  kDebug(1402) << "IndexBuilder::sendProgressSignal()" << endl;
 
  kapp->dcopClient()->emitDCOPSignal("buildIndexProgress()", QByteArray() );  
}

void IndexBuilder::quit()
{
  kDebug(1402) << "IndexBuilder::quit()" << endl;

  kapp->quit();
}


static KCmdLineOptions options[] =
{
  { "+cmdfile", I18N_NOOP("Document to be indexed"), 0 },
  { "+indexdir", I18N_NOOP("Index directory"), 0 },
  KCmdLineLastOption
};

int main( int argc, char **argv )
{
  KAboutData aboutData( "khc_indexbuilder",
                        I18N_NOOP("KHelpCenter Index Builder"),
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
    kDebug(1402) << "Wrong number of arguments." << endl;
    return 1;
  }

  QString cmdFile = args->arg( 0 );
  QString indexDir = args->arg( 1 );

  kDebug(1402) << "cmdFile: " << cmdFile << endl;
  kDebug(1402) << "indexDir: " << indexDir << endl;

  QFile file( indexDir + "/testaccess" );
  if ( !file.open( QIODevice::WriteOnly ) || file.putch( ' ' ) < 0 ) {
    kDebug(1402) << "access denied" << endl;
    return 2;
  } else {
    kDebug(1402) << "can access" << endl;
    file.remove();
  }
  
  app.dcopClient()->registerAs( "khc_indexbuilder", false );

  IndexBuilder builder;

  builder.buildIndices( cmdFile );

  return app.exec();
}

#include "khc_indexbuilder.moc"

// vim:ts=2:sw=2:et
