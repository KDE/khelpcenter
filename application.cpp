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
#include <KWindowSystem>

#include <QDir>
#include <QMetaType>
#include <QTreeWidgetItem>

Q_DECLARE_METATYPE(const QTreeWidgetItem *)

using namespace KHC;

Application::Application(int &argc, char **argv)
    : QApplication(argc, argv)
    , mMainWindow(nullptr)
{
    KLocalizedString::setApplicationDomain("khelpcenter6");

    mCmdParser.addPositionalArgument(QStringLiteral("url"), i18n("Documentation to open"));

    // allow redirecting from internal pages to known protocols
    KUrlAuthorized::allowUrlAction(QStringLiteral("redirect"), QUrl(QStringLiteral("khelpcenter:")), QUrl(QStringLiteral("ghelp:")));
    KUrlAuthorized::allowUrlAction(QStringLiteral("redirect"), QUrl(QStringLiteral("khelpcenter:")), QUrl(QStringLiteral("help:")));
    KUrlAuthorized::allowUrlAction(QStringLiteral("redirect"), QUrl(QStringLiteral("khelpcenter:")), QUrl(QStringLiteral("info:")));
    KUrlAuthorized::allowUrlAction(QStringLiteral("redirect"), QUrl(QStringLiteral("khelpcenter:")), QUrl(QStringLiteral("man:")));
    KUrlAuthorized::allowUrlAction(QStringLiteral("redirect"), QUrl(QStringLiteral("glossentry:")), QUrl(QStringLiteral("help:")));

    // registration of meta types
    qRegisterMetaType<const QTreeWidgetItem *>();
}

QCommandLineParser *Application::cmdParser()
{
    return &mCmdParser;
}

void Application::activateForStartLikeCall()
{
    mMainWindow->show();
    KWindowSystem::updateStartupId(mMainWindow->windowHandle());

    mMainWindow->raise();
    KWindowSystem::activateWindow(mMainWindow->windowHandle());
}

void Application::newInstance()
{
    mMainWindow = new MainWindow;

    mMainWindow->show();

    const QStringList urls = mCmdParser.positionalArguments();
    if (!urls.isEmpty()) {
        const QUrl url = QUrl::fromUserInput(urls.at(0), QDir::currentPath());
        mMainWindow->openUrl(url);
    }
}

void Application::restoreInstance()
{
    mMainWindow = new MainWindow;
    mMainWindow->restore(1);
}

void Application::activate(const QStringList &arguments, const QString &workingDirectory)
{
    if (!arguments.isEmpty()) {
        mCmdParser.parse(arguments);
        const QStringList urls = mCmdParser.positionalArguments();
        if (!urls.isEmpty()) {
            const QUrl url = QUrl::fromUserInput(urls.at(0), workingDirectory);
            mMainWindow->openUrl(url);
        }
    }

    activateForStartLikeCall();
}

void Application::open(const QList<QUrl> &urls)
{
    if (!urls.isEmpty()) {
        mMainWindow->openUrl(urls.first());
    }

    activateForStartLikeCall();
}

int main(int argc, char **argv)
{
    KHC::Application app(argc, argv);
    KAboutData aboutData(QStringLiteral("khelpcenter"),
                         i18n("Help Center"),
                         QStringLiteral(PROJECT_VERSION),
                         i18n("Help Center"),
                         KAboutLicense::GPL,
                         i18n("(c) 1999-2023, The KHelpCenter developers"));

    aboutData.addAuthor(QStringLiteral("Luigi Toscano"), i18n("Current maintainer"), QStringLiteral("luigi.toscano@tiscali.it"));
    aboutData.addAuthor(QStringLiteral("Pino Toscano"), i18n("Xapian-based search, lot of bugfixes"), QStringLiteral("pino@kde.org"));
    aboutData.addAuthor(QStringLiteral("Cornelius Schumacher"), i18n("Former maintainer"), QStringLiteral("schumacher@kde.org"));
    aboutData.addAuthor(QStringLiteral("Frerich Raabe"), QString(), QStringLiteral("raabe@kde.org"));
    aboutData.addAuthor(QStringLiteral("Matthias Elter"), i18n("Original Author"), QStringLiteral("me@kde.org"));
    aboutData.addAuthor(QStringLiteral("Wojciech Smigaj"), i18n("Info page support"), QStringLiteral("achu@klub.chip.pl"));
    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("help-browser")));
    KAboutData::setApplicationData(aboutData);

    QCommandLineParser *cmd = app.cmdParser();
    aboutData.setupCommandLine(cmd);
    cmd->process(app);

    aboutData.processCommandLine(cmd);

    KDBusService service(KDBusService::Unique);

    if (app.isSessionRestored() && KMainWindow::canBeRestored(1))
        app.restoreInstance();
    else
        app.newInstance();

    QObject::connect(&service, &KDBusService::activateRequested, &app, &Application::activate);
    QObject::connect(&service, &KDBusService::openRequested, &app, &Application::open);

    return app.exec();
}
#include "moc_application.cpp"

// vim:ts=2:sw=2:et
