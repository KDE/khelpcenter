/*
 *  kib_mainwindow.h - part of KInfoBrowser
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

#ifndef __kib_mainwindow_h__
#define __kib_mainwindow_h__

#include <ktmainwindow.h>

#include "kib_view.h"
#include "khc_history.h"

class KAction;
class KHelpMenu;
class KListAction;
class KToggleAction;

class kibMainWindow : public KTMainWindow
{
  Q_OBJECT

public:

  kibMainWindow( const QString& url = QString(""), char *name = 0 );
  virtual ~kibMainWindow();

  void openURL( const QString& urlRequest);
  void openURL( const QString& url, bool withHistory = true, long xOffset = 0, long yOffset = 0);

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

  void slotCheckHistory();

  void slotOptionsToolbar();
  void slotOptionsLocationbar();
  void slotOptionsStatusbar();
  void slotOptionsGeneral();

  void slotSaveSettings();
  void slotReadSettings();

  void slotAddBookmark();
  void slotClearBookmarks();

  void slotQuit();

  // slots the views connect to
  void slotOpenNewBrowser(const QString& url = QString("") );
  void slotURLSelected(const QString& _url, int _button);
  void slotSetTitle(const QString& _title );
  void slotSetLocation(const QString& _url);
  void slotSetURL( const QString& url);
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

  kibView       *m_pView;
  khcHistory     m_history;
  KAction       *m_stop, *m_zoomIn, *m_zoomOut, *m_back, *m_forward;
  KListAction   *m_bookmarks;
  KToggleAction *m_statusbar, *m_toolbar, *m_location;
  KHelpMenu     *m_pHelpMenu;

  // UI options:
  bool m_showStatusBar, m_showToolBar, m_showLocationBar;
};

#endif
