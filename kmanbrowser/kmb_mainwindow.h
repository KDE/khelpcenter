/*
 *  kmb_mainwindow.h - part of the KDE Help Center
 *
 *  Copyright (c) 199 Matthias Elter (me@kde.org)
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
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __kmb_mainwindow_h__
#define __kmb_mainwindow_h__

#include <ktmainwindow.h>

#include "khc_history.h"

class KAction;
class KToggleAction;
class kmbView;

class kmbMainWindow : public KTMainWindow
{
    Q_OBJECT

 public:

    kmbMainWindow( const QString& url = 0 );
    virtual ~kmbMainWindow();

    void openURL( const QString& url );
    void openURL( const QString& url, bool withHistory = true, long xOffset = 0, long yOffset = 0 );

 protected:

    void setupActions();
    void setupStatusBar();
    void setupLocationBar();
    void setupView();

 public slots:

    // mainwindow related slots
    void slotNewBrowser();
    void slotOpenFile();

    void slotIndex();
    void slotForward();
    void slotBack();

    void slotSetBusy(bool busy);

    void slotAddBookmark();

    void slotCheckHistory();

    void slotOptionsToolbar();
    void slotOptionsLocationbar();
    void slotOptionsStatusbar();
    void slotOptionsGeneral();

    void slotSaveSettings();
    void slotReadSettings();

    void slotQuit();

    // slots the views connect to
    void slotOpenNewBrowser(const QString& url);
    void slotURLSelected(const QString& _url, int _button);
    void slotSetTitle(const QString& _title );
    void slotSetLocation(const QString& _url);
    void slotSetURL(QString url);
    void slotSetStatusText(const QString& text);

    // forward actions to the views
    void slotFind();
    void slotFindNext();
    void slotReload();
    void slotCopy();
    void slotPrint();
    void slotMagMinus();
    void slotMagPlus();
    void slotStopProcessing();

 private slots:
    void slotLocationEntered();
    void slotHistoryFillBack();
    void slotHistoryFillForward();
    void slotHistoryBackActivated(int id);
    void slotHistoryForwardActivated(int id);

 protected:

    kmbView    *m_pView;
    khcHistory history;

    // actions
    KAction *m_zoomIn, *m_zoomOut, *m_stop, *m_back, *m_forward;
    KToggleAction *m_toolBar, *m_statusBar, *m_locationBar;

    // UI options:
    bool m_showStatusBar, m_showToolBar, m_showLocationBar;
};

#endif
