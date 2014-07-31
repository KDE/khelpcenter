
/*
 *  This file is part of the KDE Help Center
 *
 *  Copyright (c) 2002 Frerich Raabe <raabe@kde.org>
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "application.h"
#include "mainwindow.h"
#include "version.h"

#include <KAboutData>
#include <KLocalizedString>
#include <KDBusService>
#include <QCommandLineParser>
#include <QDebug>
#include <QDir>

using namespace KHC;

Application::Application(int& argc, char** argv)
    : QApplication(argc, argv), mMainWindow( 0 )
{
    KDBusService* s = new KDBusService(KDBusService::Unique);
    connect(this, SIGNAL(aboutToQuit()), s, SLOT(deleteLater()));
    connect(s, SIGNAL(activateRequested(QStringList, QString)), this, SLOT(activate(QStringList, QString)));
}

void Application::activate(const QStringList& args, const QString &workingDirectory)
{
  QCommandLineParser cmd;
  cmd.addPositionalArgument("url", i18n("Documentation to open"));
  KAboutData::applicationData().setupCommandLine(&cmd);
  cmd.process(args);
  KAboutData::applicationData().processCommandLine(&cmd);
  QStringList urls = cmd.positionalArguments();

  if( !mMainWindow )
  {
    if (qApp->isSessionRestored()) {
        // The kRestoreMainWindows call will do the rest.
        return;
    }
    mMainWindow = new MainWindow;
  }

  foreach(const QString& arg, urls) {
#if QT_VERSION > QT_VERSION_CHECK(5, 4, 0)
    QUrl url = QUrl::fromUserInput(arg, workingDirectory);
#else
    Q_UNUSED(workingDirectory);
    QUrl url(arg);
#endif
    mMainWindow->openUrl( url );
  }

  mMainWindow->show();
}

extern "C" int Q_DECL_EXPORT kdemain( int argc, char **argv )
{
  KHC::Application app(argc, argv);
  KAboutData aboutData( "khelpcenter", i18n("KDE Help Center"),
                        HELPCENTER_VERSION,
                        i18n("The KDE Help Center"),
                        KAboutLicense::GPL,
                        i18n("(c) 1999-2011, The KHelpCenter developers") );

  aboutData.addAuthor( "Cornelius Schumacher", QString(), "schumacher@kde.org" );
  aboutData.addAuthor( "Frerich Raabe", QString(), "raabe@kde.org" );
  aboutData.addAuthor( "Matthias Elter", i18n("Original Author"), "me@kde.org" );
  aboutData.addAuthor( "Wojciech Smigaj", i18n("Info page support"), "achu@klub.chip.pl" );
  aboutData.setProgramIconName( "help-browser" );
  KAboutData::setApplicationData(aboutData);

  app.activate(app.arguments(), QDir::currentPath());

  if ( app.isSessionRestored() )
  {
     kRestoreMainWindows<MainWindow>();
  }

  return app.exec();
}

// vim:ts=2:sw=2:et
