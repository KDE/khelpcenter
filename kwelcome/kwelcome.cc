/*
 *  kwelcome.cc - part of the KDE Help Center
 *
 *  Copyright (C) 1999 Matthias Elter (me@kde.org)
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

#include "kwelcome.h"

#include <kstddirs.h>
#include <kprocess.h>
#include <klocale.h>
#include <kconfig.h>
#include <kapp.h>

#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qmsgbox.h>

#include <fstream.h>

KWelcome::KWelcome(QWidget *parent, const char *name)
	: QWidget(parent, name, WStyle_Tool)
{
  setCaption(i18n("Welcome to the K Desktop Environment!"));
  setFixedSize(559, 416);
  setGeometry(QApplication::desktop()->width()/2 - width()/2,
			  QApplication::desktop()->height()/2 - height()/2,
			  590, 416);

  // setup topView
  topView = new QWidget(this);
  topView->setBackgroundColor(QColor(255,255,255));
  
  // setup bottomView
  bottomView = new QWidget(this);
  bottomView->setMaximumHeight(30);
  bottomView->setMinimumHeight(30);
  
  // setup the top level layout manager
  QVBoxLayout *toplevel_l = new QVBoxLayout(this);
  toplevel_l->addWidget(topView);
  toplevel_l->addWidget(bottomView);
  toplevel_l->activate();
  
  // create quitButton
  quitButton = new QPushButton(i18n("&Quit"), bottomView);
  connect(quitButton, SIGNAL(clicked()), kapp, SLOT(quit()));
  quitButton->setFixedWidth(85);
  quitButton->setFixedHeight(26);
  quitButton->move(bottomView->width() - 85, 4);
 
  // create abouButton
  aboutButton = new QPushButton(i18n("&About KDE"), bottomView);
  connect(aboutButton, SIGNAL(clicked()), this, SLOT(slotAboutKDE()));
  aboutButton->setFixedWidth(95);
  aboutButton->setFixedHeight(26);
  aboutButton->move(bottomView->width() - 182, 4);
  
  // create 'start on every KDE startup' checkbox
  autostart_kwelcome = new QCheckBox(i18n("Show this dialog on KDE startup."), bottomView);
  autostart_kwelcome->setGeometry(2,7,200,28);
  autostart_kwelcome->setChecked(TRUE);
  autostart_kwelcome->setAutoResize(TRUE);
  
  // welcome image
  QLabel *welcome = new QLabel(topView);
  welcome->setGeometry(2,2,557,386);

  QPixmap welcome_pm(locate("data", "kwelcome/pics/welcome.png"));	
  welcome->setPixmap(welcome_pm);

  // create help center button
  helpcenterButton = new QPushButton(i18n("Get &help"), topView);
  connect(helpcenterButton, SIGNAL(clicked()), this, SLOT(slotHelpCenterStart()));
  helpcenterButton->setFixedWidth(133);
  helpcenterButton->setFixedHeight(26);
  helpcenterButton->move(15 ,330);
  
  // create configuration wizard button
  wizardButton = new QPushButton(i18n("Configuration &wizard"), topView);
  connect(wizardButton, SIGNAL(clicked()), this, SLOT(slotWizardStart()));
  wizardButton->setFixedWidth(173);
  wizardButton->setFixedHeight(26);
  wizardButton->move(17 + helpcenterButton->width(), 330);
	
  // read settings
  readSettings();
}

KWelcome::~KWelcome()
{
  saveSettings();
}

void KWelcome::slotAboutKDE()
{
  QMessageBox::about(0L, i18n("About KDE"),
		     i18n("\nThe KDE Desktop Environment was written by the KDE Team,\n"
			  "a world-wide network of software engineers committed to\n"
			  "free software development.\n\n"
			  "Visit http://www.kde.org for more information on the KDE\n"
			  "Project. Please consider joining and supporting KDE.\n\n"
			  "Please report bugs at http://bugs.kde.org.\n"
			  ));
}

void KWelcome::slotWizardStart()
{
  KProcess proc;	
  proc << "kdewizard";
  proc.start(KProcess::DontCare);
}

void KWelcome::slotHelpCenterStart()
{
  KProcess proc;	
  proc << "khelpcenter";
  proc.start(KProcess::DontCare);
}

void KWelcome::saveSettings()
{
  KConfig *conf = kapp->getConfig();
  conf->setGroup("General Settings");
  if (autostart_kwelcome->isChecked())
	conf->writeEntry("AutostartOnKDEStartup", "true");
  else
	conf->writeEntry("AutostartOnKDEStartup", "false");
  conf->sync();
  cout << "KWelcome: configuration written." << endl;
}

void KWelcome::readSettings()
{
  KConfig *conf = kapp->getConfig();
  conf->setGroup("General Settings");
  QString tmp = conf->readEntry("AutostartOnKDEStartup", "true");
  
  if (tmp == "true")
	autostart_kwelcome->setChecked(true);
  else
	autostart_kwelcome->setChecked(false);	
}

#include "kwelcome.moc"
