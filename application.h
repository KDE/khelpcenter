/*
    SPDX-FileCopyrightText: 2002 Frerich Raabe <raabe@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KHC_APPLICATION_H
#define KHC_APPLICATION_H

#include <QApplication>
#include <QCommandLineParser>
#include <QList>
#include <QUrl>

namespace KHC {

  class MainWindow;

  class Application : public QApplication
  {
    Q_OBJECT
    public:
      Application(int& argc, char** argv);

      QCommandLineParser *cmdParser();

      void newInstance();
      void restoreInstance();

    public Q_SLOTS:
        void activate(const QStringList& args, const QString &workingDirectory);
        void open(const QList<QUrl>& urls);

  private:
        void activateForStartLikeCall();

    private:
      MainWindow *mMainWindow = nullptr;
      QUrl mOpen;
      QCommandLineParser mCmdParser;
  };

}

#endif // KHC_APPLICATION_H
// vim:ts=2:sw=2:et
