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

#include <KAboutData>
#include <KLocale>
#include <KCmdLineArgs>
#include <KUniqueApplication>
#include <KDebug>
#include <KProcess>
#include <KConfig>
#include <KShell>

#include <QFile>
#include <QTextStream>
#include <QDBusMessage>
#include <QDBusConnection>

#include <unistd.h>
#include <stdlib.h>
#include <iostream>

using namespace KHC;

IndexBuilder::IndexBuilder(const QString& cmdFile)
{
  m_cmdFile = cmdFile;
  kDebug(1402) << "IndexBuilder()";
}

void IndexBuilder::buildIndices()
{
  QFile f( m_cmdFile );
  if ( !f.open( QIODevice::ReadOnly ) ) {
    kError() << "Unable to open file '" << m_cmdFile << "'" << endl;
    exit( 1 );
  }
  kDebug(1402) << "Opened file '" << m_cmdFile << "'";
  QTextStream ts( &f );
  QString line = ts.readLine();
  while ( !line.isNull() ) {
    kDebug(1402) << "LINE: " << line;
    mCmdQueue.append( line );
    line = ts.readLine();
  }

  processCmdQueue();
}

void IndexBuilder::processCmdQueue()
{
  kDebug(1402) << "IndexBuilder::processCmdQueue()";

  QStringList::Iterator it = mCmdQueue.begin();

  if ( it == mCmdQueue.end() ) {
    quit();
    return;
  }

  QString cmd = *it;

  kDebug(1402) << "PROCESS: " << cmd;

  KProcess *proc = new KProcess;

  *proc << KShell::splitArgs(cmd);

  connect( proc, SIGNAL( finished( int, QProcess::ExitStatus) ),
           SLOT( slotProcessExited( int, QProcess::ExitStatus) ) );

  mCmdQueue.erase( it );

  proc->start();

  if ( !proc->waitForStarted() )  {
    sendErrorSignal( i18n("Unable to start command '%1'.", cmd ) );
    processCmdQueue();
    delete proc;
  }
}

void IndexBuilder::slotProcessExited( int exitCode, QProcess::ExitStatus exitStatus )
{
  KProcess *proc = static_cast<KProcess *>(sender());

  if ( exitStatus != QProcess::NormalExit ) {
    kError(1402) << "Process failed" << endl;
    kError(1402) << "stdout output:" << proc->readAllStandardOutput(); 
    kError(1402) << "stderr output:" << proc->readAllStandardError();  
  }
  else if (exitCode != 0 ) {
    kError(1402) << "running" << proc->program() << "failed with exitCode" << exitCode;
    kError(1402) << "stdout output:" << proc->readAllStandardOutput(); 
    kError(1402) << "stderr output:" << proc->readAllStandardError(); 
  }
  delete proc;

  sendProgressSignal();

  processCmdQueue();
}

void IndexBuilder::sendErrorSignal( const QString &error )
{
  kDebug(1402) << "IndexBuilder::sendErrorSignal()";
  QDBusMessage message =
     QDBusMessage::createSignal("/kcmhelpcenter", "org.kde.kcmhelpcenter", "buildIndexError");
  message <<error;
  QDBusConnection::sessionBus().send(message);

}

void IndexBuilder::sendProgressSignal()
{
  kDebug(1402) << "IndexBuilder::sendProgressSignal()";
  QDBusMessage message =
        QDBusMessage::createSignal("/kcmhelpcenter", "org.kde.kcmhelpcenter", "buildIndexProgress");
  QDBusConnection::sessionBus().send(message);
}

void IndexBuilder::quit()
{
  kDebug(1402) << "IndexBuilder::quit()";

  qApp->quit();
}


int main( int argc, char **argv )
{
  KAboutData aboutData( "khc_indexbuilder", 0,
                        ki18n("KHelpCenter Index Builder"),
                        HELPCENTER_VERSION,
                        ki18n("The KDE Help Center"),
                        KAboutData::License_GPL,
                        ki18n("(c) 2003, The KHelpCenter developers") );

  aboutData.addAuthor( ki18n("Cornelius Schumacher"), KLocalizedString(), "schumacher@kde.org" );

  KCmdLineArgs::init( argc, argv, &aboutData );

  KCmdLineOptions options;
  options.add("+cmdfile", ki18n("Document to be indexed"));
  options.add("+indexdir", ki18n("Index directory"));
  KCmdLineArgs::addCmdLineOptions( options );

  // Note: no KComponentData seems necessary
  QCoreApplication app( KCmdLineArgs::qtArgc(), KCmdLineArgs::qtArgv() );

  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

  if ( args->count() != 2 ) {
    kDebug(1402) << "Wrong number of arguments.";
    return 1;
  }

  QString cmdFile = args->arg( 0 );
  QString indexDir = args->arg( 1 );

  kDebug(1402) << "cmdFile: " << cmdFile;
  kDebug(1402) << "indexDir: " << indexDir;

  QFile file( indexDir + "/testaccess" );
  if ( !file.open( QIODevice::WriteOnly ) || !file.putChar( ' ' ) ) {
    kDebug(1402) << "access denied";
    return 2;
  } else {
    kDebug(1402) << "can access";
    file.remove();
  }

  IndexBuilder builder(cmdFile);

  QTimer::singleShot(0, &builder, SLOT(buildIndices()));

  return app.exec();
}

#include "khc_indexbuilder.moc"

// vim:ts=2:sw=2:et
