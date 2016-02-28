
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

#include <KAboutData>
#include <KLocalizedString>
#include <KDBusService>
#include <KUrlAuthorized>
#include <QCommandLineParser>
#include <QDebug>
#include <QDir>

using namespace KHC;

Application::Application(int& argc, char** argv)
    : QApplication(argc, argv), mMainWindow( 0 )
{
  mCmdParser.addPositionalArgument( "url", i18n( "Documentation to open" ) );
  mCmdParser.addHelpOption();
  mCmdParser.addVersionOption();

  // allow redirecting from internal pages to known protocols
  KUrlAuthorized::allowUrlAction( QLatin1Literal( "redirect" ), QUrl( QLatin1Literal( "khelpcenter:" ) ), QUrl( QLatin1Literal( "ghelp:" ) ) );
  KUrlAuthorized::allowUrlAction( QLatin1Literal( "redirect" ), QUrl( QLatin1Literal( "khelpcenter:" ) ), QUrl( QLatin1Literal( "help:" ) ) );
  KUrlAuthorized::allowUrlAction( QLatin1Literal( "redirect" ), QUrl( QLatin1Literal( "khelpcenter:" ) ), QUrl( QLatin1Literal( "info:" ) ) );
  KUrlAuthorized::allowUrlAction( QLatin1Literal( "redirect" ), QUrl( QLatin1Literal( "khelpcenter:" ) ), QUrl( QLatin1Literal( "man:" ) ) );
}

QCommandLineParser *Application::cmdParser()
{
  return &mCmdParser;
}

void Application::activate(const QStringList& args, const QString &workingDirectory)
{
  mCmdParser.process( args );

  QStringList urls = mCmdParser.positionalArguments();

  if( !mMainWindow )
  {
    if (qApp->isSessionRestored()) {
        // The kRestoreMainWindows call will do the rest.
        return;
    }
    mMainWindow = new MainWindow;
  }

  QUrl url;
  if ( !urls.isEmpty() ) {
    url = QUrl::fromUserInput( urls.at( 0 ), workingDirectory );
  }

  mMainWindow->openUrl( url );

  mMainWindow->show();
}

extern "C" int Q_DECL_EXPORT kdemain( int argc, char **argv )
{
  KHC::Application app(argc, argv);
  KLocalizedString::setApplicationDomain("khelpcenter");
  KAboutData aboutData( "khelpcenter", i18n("KDE Help Center"),
                        PROJECT_VERSION,
                        i18n("The KDE Help Center"),
                        KAboutLicense::GPL,
                        i18n("(c) 1999-2011, The KHelpCenter developers") );

  aboutData.addAuthor( "Cornelius Schumacher", QString(), "schumacher@kde.org" );
  aboutData.addAuthor( "Frerich Raabe", QString(), "raabe@kde.org" );
  aboutData.addAuthor( "Matthias Elter", i18n("Original Author"), "me@kde.org" );
  aboutData.addAuthor( "Wojciech Smigaj", i18n("Info page support"), "achu@klub.chip.pl" );
  app.setWindowIcon( QIcon::fromTheme("help-browser") );
  KAboutData::setApplicationData(aboutData);

  QCommandLineParser *cmd = app.cmdParser();
  KAboutData::applicationData().setupCommandLine( cmd );
  cmd->process( app );

  KDBusService service( KDBusService::Unique );

  KAboutData::applicationData().processCommandLine( cmd );

  app.activate(app.arguments(), QDir::currentPath());

  QObject::connect( &service, &KDBusService::activateRequested, &app, &Application::activate );

  if ( app.isSessionRestored() )
  {
     kRestoreMainWindows<MainWindow>();
  }

  return app.exec();
}

// vim:ts=2:sw=2:et
