/*
 *  khc_mainwindow.cc - part of the KDE Helpenter
 *
 *  Copyright (c) 1999 Matthias Elter (me@kde.org)
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

#ifndef __khc_mainwindow_h__
#define __khc_mainwindow_h__

class khcMainWindow;
class khcMainView;

#include <opMainWindow.h>
#include <opMainWindowIf.h>
class OPMenu;

#include "khelpcenter.h"

class khcMainWindowIf : virtual public OPMainWindowIf,
			virtual public KHelpCenter::MainWindow_skel
{
public:
  khcMainWindowIf(khcMainWindow* _main);
  virtual ~khcMainWindowIf();

  KHelpCenter::MainView_ptr mainView();
  
protected:
  khcMainWindow* m_pkhcWin;
};

class khcMainWindow : public OPMainWindow
{
  Q_OBJECT
public:
  khcMainWindow(const char *url = 0L);
  ~khcMainWindow();

  virtual OPMainWindowIf* interface();
  virtual khcMainWindowIf* khcInterface();

  KHelpCenter::MainView_ptr mainView();
  
protected slots:
  void slotClose();
  void slotQuit();
  void slotActivePartChanged( unsigned long, unsigned long );

protected:
  void createFileMenu( OPMenuBar* _menubar );
  void createHelpMenu( OPMenuBar* _menubar );

  void readProperties( KConfig * config );
  void saveProperties( KConfig * config );
 
  khcMainWindowIf* m_pkhcInterface;

  OPMenu* m_pFileMenu;
  OPMenu* m_pHelpMenu;

  long int m_idMenuHelp_About;

  khcMainView *m_pMainView;
  KHelpCenter::MainView_var m_vMainView;
};

#endif
