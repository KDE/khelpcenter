/*
    SPDX-FileCopyrightText: 2002 Frerich Raabe <raabe@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "application.h"
#include "mainwindow.h"

#include <KAboutData>
#include <KDBusService>
#include <KLocalizedString>
#include <KUrlAuthorized>

#include <QDir>
#include <QMetaType>
#include <QTreeWidgetItem>

Q_DECLARE_METATYPE(const QTreeWidgetItem *)

using namespace KHC;

Application::Application(int& argc, char** argv)
    : QApplication(argc, argv), mMainWindow( nullptr )
{
  setAttribute(Qt::AA_UseHighDpiPixmaps);

  mCmdParser.addPositionalArgument( QStringLiteral("url"), i18n( "Documentation to open" ) );

  // allow redirecting from internal pages to known protocols
  KUrlAuthorized::allowUrlAction( QStringLiteral( "redirect" ), QUrl( QStringLiteral( "khelpcenter:" ) ), QUrl( QStringLiteral( "ghelp:" ) ) );
  KUrlAuthorized::allowUrlAction( QStringLiteral( "redirect" ), QUrl( QStringLiteral( "khelpcenter:" ) ), QUrl( QStringLiteral( "help:" ) ) );
  KUrlAuthorized::allowUrlAction( QStringLiteral( "redirect" ), QUrl( QStringLiteral( "khelpcenter:" ) ), QUrl( QStringLiteral( "info:" ) ) );
  KUrlAuthorized::allowUrlAction( QStringLiteral( "redirect" ), QUrl( QStringLiteral( "khelpcenter:" ) ), QUrl( QStringLiteral( "man:" ) ) );
  KUrlAuthorized::allowUrlAction( QStringLiteral( "redirect" ), QUrl( QStringLiteral( "glossentry:" ) ), QUrl( QStringLiteral( "help:" ) ) );

  // registration of meta types
  qRegisterMetaType<const QTreeWidgetItem *>();
}

QCommandLineParser *Application::cmdParser()
{
  return &mCmdParser;
}

void Application::activate(const QStringList& args, const QString &workingDirectory)
{
  mCmdParser.parse( args );

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

int main( int argc, char **argv )
{
  KHC::Application app(argc, argv);
  KLocalizedString::setApplicationDomain("khelpcenter5");
  KAboutData aboutData( QStringLiteral("khelpcenter"), i18n("Help Center"),
                        QStringLiteral(PROJECT_VERSION),
                        i18n("Help Center"),
                        KAboutLicense::GPL,
                        i18n("(c) 1999-2020, The KHelpCenter developers") );

  aboutData.addAuthor( QStringLiteral("Luigi Toscano"), i18n("Current maintainer"), QStringLiteral("luigi.toscano@tiscali.it") );
  aboutData.addAuthor( QStringLiteral("Pino Toscano"), i18n("Xapian-based search, lot of bugfixes"), QStringLiteral("pino@kde.org") );
  aboutData.addAuthor( QStringLiteral("Cornelius Schumacher"), i18n("Former maintainer"), QStringLiteral("schumacher@kde.org") );
  aboutData.addAuthor( QStringLiteral("Frerich Raabe"), QString(), QStringLiteral("raabe@kde.org") );
  aboutData.addAuthor( QStringLiteral("Matthias Elter"), i18n("Original Author"), QStringLiteral("me@kde.org") );
  aboutData.addAuthor( QStringLiteral("Wojciech Smigaj"), i18n("Info page support"), QStringLiteral("achu@klub.chip.pl") );
  app.setWindowIcon( QIcon::fromTheme(QStringLiteral("help-browser")) );
  aboutData.setDesktopFileName ( QStringLiteral("org.kde.Help") );
  KAboutData::setApplicationData(aboutData);

  QCommandLineParser *cmd = app.cmdParser();
  aboutData.setupCommandLine( cmd );
  cmd->process( app );

  aboutData.processCommandLine( cmd );

  KDBusService service( KDBusService::Unique );

  app.activate(app.arguments(), QDir::currentPath());

  QObject::connect( &service, &KDBusService::activateRequested, &app, &Application::activate );

  if ( app.isSessionRestored() )
  {
     kRestoreMainWindows<MainWindow>();
  }

  return app.exec();
}

// vim:ts=2:sw=2:et
