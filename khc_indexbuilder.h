/*
 *  This file is part of the KDE Help Center
 *
 *  Copyright (C) 2003 Cornelius Schumacher <schumacher@kde.org>
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
 *  Foundation, Inc.,
 *  59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#ifndef KHC_INDEXBUILDER_H
#define KHC_INDEXBUILDER_H

#include <kuniqueapplication.h>

#include <qobject.h>
#include <qtimer.h>

class KProcess;

namespace KHC {

class IndexBuilder : public QObject
{
    Q_OBJECT
  public:
    IndexBuilder();

    void sendProgressSignal();
    void quit();

    void buildIndices( const QString &cmdFile );

    void processCmdQueue();

  protected slots:
    void slotProcessExited( KProcess * );
    void slotReceivedStdout( KProcess *, char *buffer, int buflen );
    void slotReceivedStderr( KProcess *, char *buffer, int buflen );

  private:
    QTimer mTimer;
    QStringList mCmdQueue;
};

}

#endif

// vim:ts=2:sw=2:et
