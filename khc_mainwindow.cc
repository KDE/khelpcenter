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

#include "khc_mainwindow.h"
#include "khc_mainview.h"

#include <kapp.h>
#include <kconfig.h>
#include <klocale.h>
#include <kdebug.h>

#include <qkeycode.h>

#include <komShutdownManager.h>

#include <opApplication.h>
#include <opMenuBarManager.h>
#include <opToolBarManager.h>
#include <opStatusBarManager.h>
#include <opMenu.h>

khcMainWindow::khcMainWindow( const char *url )
{
  kdebug(KDEBUG_INFO,1400,"khcMainWindow::khcMainWindow()");
  
  setCaption(i18n("KDE Helpcenter"));

  resize(800, 580);
  setMinimumSize(200, 100);

  m_pFileMenu = 0L;
  m_pHelpMenu = 0L;
  
  connect(this, SIGNAL(activePartChanged(unsigned long, unsigned long)),
	  this, SLOT(slotActivePartChanged(unsigned long, unsigned long)));
  
  // dummy calls to instantiate the UI managers
  (void)menuBarManager();
  (void)toolBarManager();  
  (void)statusBarManager();

  // setup view
  m_pMainView = new khcMainView(url, this);
  setView(m_pMainView);

  m_vMainView = KHelpCenter::MainView::_duplicate(m_pMainView);
  m_vMainView->incRef();
  m_vMainView->setMainWindow(interface());

  interface()->setActivePart(m_vMainView->id());
  interface()->setActivePart(m_vMainView->childViewId());
}

khcMainWindow::~khcMainWindow()
{ 
  m_vMainView->decRef();
  m_vMainView = 0L;

  if (m_pInterface)
    {
      m_pInterface->cleanUp();
      delete m_pInterface;
      m_pInterface = 0L;
    }
}

OPMainWindowIf* khcMainWindow::interface()
{
  if (m_pInterface == 0L)
  {    
    m_pkhcInterface = new khcMainWindowIf(this);
    m_pInterface = m_pkhcInterface;
  }
  return m_pInterface;
}

khcMainWindowIf* khcMainWindow::khcInterface()
{
  if ( m_pInterface == 0L )
    {
      m_pkhcInterface = new khcMainWindowIf(this);
      m_pInterface = m_pkhcInterface;
    }
  return m_pkhcInterface;
}

KHelpCenter::MainView_ptr khcMainWindow::mainView()
{
  return KHelpCenter::MainView::_duplicate(m_vMainView);
}

void khcMainWindow::createFileMenu(OPMenuBar* _menubar)
{
  // Do we lose control over the menubar ?
  if (_menubar == 0L)
    {
      m_pFileMenu = 0L;
      return;
    }

  m_pFileMenu = _menubar->fileMenu();
  if (m_pFileMenu == 0L)
    {
      m_pFileMenu = new OPMenu;
      _menubar->insertItem( i18n("&File"), m_pFileMenu );
    }
  else
    m_pFileMenu->insertSeparator();

  m_pFileMenu->insertItem(i18n("&Close"), this, SLOT(slotClose()), CTRL+Key_C);
  m_pFileMenu->insertSeparator();
  m_pFileMenu->insertItem(i18n("&Quit..."), this, SLOT(slotQuit()), CTRL+Key_Q);
}

void khcMainWindow::createHelpMenu(OPMenuBar* _menubar)
{
  // Do we lose control over the menubar ?
  if (_menubar == 0L)
    {
      m_pHelpMenu = 0L;
      return;
    }
  
  // Ask for the help menu.
  m_pHelpMenu = _menubar->helpMenu();
  // No help menu yet ?
  if (m_pHelpMenu == 0L)
  {    
    m_pHelpMenu = new OPMenu;

    _menubar->insertSeparator();
    _menubar->insertItem(i18n("&Help"), m_pHelpMenu);
  }
  else
    m_pHelpMenu->insertSeparator();
  
  m_pHelpMenu->insertItem(i18n("About &KDE..."), kapp, SLOT(aboutKDE()));
}

void khcMainWindow::slotClose()
{
  delete this;
}

void khcMainWindow::slotQuit()
{
  kapp->exit();
}

void khcMainWindow::slotActivePartChanged(unsigned long new_id, unsigned long /*old_id*/)
{
  menuBarManager()->clear();
  toolBarManager()->clear();
  statusBarManager()->clear();
  menuBarManager()->create(new_id);
  toolBarManager()->create(new_id);
  statusBarManager()->create(new_id);
}

void khcMainWindow::readProperties(KConfig * config)
{
  //m_pMainView->readProperties(config);
}

void khcMainWindow::saveProperties(KConfig * config)
{
  //m_pMainView->saveProperties(config);
}

khcMainWindowIf::khcMainWindowIf(khcMainWindow* _main) : OPMainWindowIf(_main)
{
  ADD_INTERFACE("IDL:KHelpCenter/MainWindow:1.0");

  m_pkhcWin = _main;
}

khcMainWindowIf::~khcMainWindowIf()
{
  cleanUp();
}

KHelpCenter::MainView_ptr khcMainWindowIf::mainView()
{
  return m_pkhcWin->mainView();
}

#include "khc_mainwindow.moc"

