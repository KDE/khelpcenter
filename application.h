
/*
 *  This file is part of the KDE Help Center
 *
 *  Copyright (C) 2002 Frerich Raabe <raabe@kde.org>
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

#ifndef KHC_APPLICATION_H
#define KHC_APPLICATION_H

#include <QApplication>
#include <QUrl>
#include <QCommandLineParser>

namespace KHC {

  class MainWindow;

  class Application : public QApplication
  {
    Q_OBJECT
    public:
      Application(int& argc, char** argv);

      QCommandLineParser *cmdParser();

    public Q_SLOTS:
        void activate(const QStringList& args, const QString &workingDirectory);

    private:
      MainWindow *mMainWindow;
      QUrl mOpen;
      QCommandLineParser mCmdParser;
  };

}

extern "C" { int Q_DECL_EXPORT kdemain(int argc, char **argv); }

#endif // KHC_APPLICATION_H
// vim:ts=2:sw=2:et
