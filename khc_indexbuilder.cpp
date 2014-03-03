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
#include <QCoreApplication>
#include <KLocalizedString>
#include <qloggingcategory.h>

#include <KAboutData>
#include <KProcess>
#include <KShell>

#include <QFile>
#include <QTextStream>
#include <QDBusMessage>
#include <QDBusConnection>
#include <QDebug>
#include <QCommandLineParser>

#include <unistd.h>
#include <stdlib.h>
#include <iostream>

using namespace KHC;

static QLoggingCategory category("org.kde.KHelpCenter.IndexBuilder");

IndexBuilder::IndexBuilder(const QString& cmdFile)
{
  m_cmdFile = cmdFile;
  qCDebug(category) << "IndexBuilder()";
}

void IndexBuilder::buildIndices()
{
  QFile f( m_cmdFile );
  if ( !f.open( QIODevice::ReadOnly ) ) {
    qWarning() << "Unable to open file '" << m_cmdFile << "'";
    exit( 1 );
  }
  qCDebug(category) << "Opened file '" << m_cmdFile << "'";
  QTextStream ts( &f );
  QString line = ts.readLine();
  while ( !line.isNull() ) {
    qCDebug(category) << "LINE: " << line;
    mCmdQueue.append( line );
    line = ts.readLine();
  }

  processCmdQueue();
}

void IndexBuilder::processCmdQueue()
{
  qCDebug(category) << "IndexBuilder::processCmdQueue()";

  QStringList::Iterator it = mCmdQueue.begin();

  if ( it == mCmdQueue.end() ) {
    quit();
    return;
  }

  QString cmd = *it;

  qCDebug(category) << "PROCESS: " << cmd;

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
    qCWarning(category) << "Process failed" << endl;
    qCWarning(category) << "stdout output:" << proc->readAllStandardOutput();
    qCWarning(category) << "stderr output:" << proc->readAllStandardError();
  }
  else if (exitCode != 0 ) {
    qCWarning(category) << "running" << proc->program() << "failed with exitCode" << exitCode;
    qCWarning(category) << "stdout output:" << proc->readAllStandardOutput();
    qCWarning(category) << "stderr output:" << proc->readAllStandardError();
  }
  delete proc;

  sendProgressSignal();

  processCmdQueue();
}

void IndexBuilder::sendErrorSignal( const QString &error )
{
  qCDebug(category) << "IndexBuilder::sendErrorSignal()";
  QDBusMessage message =
     QDBusMessage::createSignal("/kcmhelpcenter", "org.kde.kcmhelpcenter", "buildIndexError");
  message <<error;
  QDBusConnection::sessionBus().send(message);

}

void IndexBuilder::sendProgressSignal()
{
  qCDebug(category) << "IndexBuilder::sendProgressSignal()";
  QDBusMessage message =
        QDBusMessage::createSignal("/kcmhelpcenter", "org.kde.kcmhelpcenter", "buildIndexProgress");
  QDBusConnection::sessionBus().send(message);
}

void IndexBuilder::quit()
{
  qCDebug(category) << "IndexBuilder::quit()";

  QCoreApplication::instance()->quit();
}


int main( int argc, char **argv )
{
  QCoreApplication app( argc, argv );
  KAboutData aboutData( "khc_indexbuilder", 0,
                        i18n("KHelpCenter Index Builder"),
                        HELPCENTER_VERSION,
                        i18n("The KDE Help Center"),
                        KAboutData::License_GPL,
                        i18n("(c) 2003, The KHelpCenter developers") );

  aboutData.addAuthor( i18n("Cornelius Schumacher"), QString(), "schumacher@kde.org" );
  KAboutData::setApplicationData(aboutData);

  QCommandLineParser options;
  options.addPositionalArgument("cmdfile", i18n("Document to be indexed"));
  options.addPositionalArgument("indexdir", i18n("Index directory"));

  aboutData.setupCommandLine(&options);
  options.process(app);
  aboutData.processCommandLine(&options);

  if ( options.positionalArguments().count() != 2 ) {
    qCDebug(category) << "Wrong number of arguments.";
    options.showHelp(1);
  }

  QString cmdFile = options.positionalArguments().first();
  QString indexDir = options.positionalArguments().last();

  qCDebug(category) << "cmdFile: " << cmdFile;
  qCDebug(category) << "indexDir: " << indexDir;

  QFile file( indexDir + "/testaccess" );
  if ( !file.open( QIODevice::WriteOnly ) || !file.putChar( ' ' ) ) {
    qCDebug(category) << "access denied";
    return 2;
  } else {
    qCDebug(category) << "can access";
    file.remove();
  }

  IndexBuilder builder(cmdFile);

  QTimer::singleShot(0, &builder, SLOT(buildIndices()));

  return app.exec();
}

#include "khc_indexbuilder.moc"

// vim:ts=2:sw=2:et
