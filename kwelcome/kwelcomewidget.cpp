/*
 *  kwelcomewidget.cpp - part of the KDE Help Center
 *
 *  Copyright (C) 1998,99 Matthias Elter (me@kde.org)
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

#include "kwelcomewidget.h"
#include <kprocess.h>
#include <qlayout.h>
#include <qcheckbox.h>
#include <qmsgbox.h>

KWelcomeWidget::KWelcomeWidget(QWidget *parent, const char *name)
	: QWidget(parent, name)
{
  // initial window size is 600x420
  
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
  
  // create aboutMenu
  aboutMenu = new QPopupMenu;
  CHECK_PTR(aboutMenu);
  aboutMenu->insertItem(i18n("About K&Welcome"), this,
                        SLOT(slotAboutKWelcome())); 
  aboutMenu->insertSeparator();
  aboutMenu->insertItem(i18n("&About KDE"), this, SLOT(slotAboutKDE()));
  
  // create abouButton, connect to aboutMenu
  aboutButton = new QPushButton(i18n("&About"), bottomView);
  connect(aboutButton, SIGNAL(clicked()), this, SLOT(slotAboutButtonClicked()));
  aboutButton->setFixedWidth(85);
  aboutButton->setFixedHeight(26);
  
  // create 'start on every KDE startup' checkbox
  autostart_kwelcome = new QCheckBox(i18n("Show this dialog on every KDE startup."),
									 bottomView);
  autostart_kwelcome->setGeometry(2,7,200,28);
  autostart_kwelcome->setChecked(TRUE);
  autostart_kwelcome->setAutoResize(TRUE);
  
  // tiny KDE-logo label
  QLabel *kdelogo = new QLabel(topView);
  kdelogo->setGeometry(2,2,238,70);
	
  QPixmap kdelogo_pm;
  QString tmp = kapp->kde_datadir().copy() + "/kwelcome/pics/kdelogo.gif";
  if(!kdelogo_pm.load(tmp))
	printf("Image load failure, could not load %s\n", tmp.data());
  else
	kdelogo->setPixmap(kdelogo_pm);
  
  // black seperator line
  QLabel *bline = new QLabel(topView);
  bline->setGeometry(8,75,250,2);
  bline->setBackgroundColor(QColor(20,20,20));
  
  // large KDE-logo label
  bigklogo = new QLabel(topView);
  bigklogo->setGeometry(0,0,250,302);
  
  QPixmap bigklogo_pm;
  tmp = kapp->kde_datadir().copy() + "/kwelcome/pics/kdelogo_big.gif";
  if(!bigklogo_pm.load(tmp))
	printf("Image load failure, could not load %s\n", tmp.data());
  else
	bigklogo->setPixmap(bigklogo_pm);
  
  // create configuration wizard button
  wizardButton = new QPushButton(i18n("Configuration &wizard"), topView);
  connect(wizardButton, SIGNAL(clicked()), this, SLOT(slotWizardStart()));
  wizardButton->setFixedWidth(173);
  wizardButton->setFixedHeight(26);
  
  // create help center button
  helpcenterButton = new QPushButton(i18n("Help &Center"), topView);
  connect(helpcenterButton, SIGNAL(clicked()), this, SLOT(slotHelpCenterStart()));
  helpcenterButton->setFixedWidth(133);
  helpcenterButton->setFixedHeight(26);
  
  // create welcome text label
  welcometext = new QLabel(topView);
  welcometext->setBackgroundColor(QColor(250,250,235));
  welcometext->setText(i18n("Welcome to the K Desktop Environment!\n\n"
							"If you are using KDE the first time please hit the button "
							"'Configuration Wizard' below to start a step "
							"by step guide to configure your desktop environment.\n\n"
							"If you want to learn more about KDE please hit the "
							"button 'Help Center' below to start the KDE Help "
							"Center.\n\n"
							"With KDE there is now an easy to use contemporary"
							" desktop environment available for Unix. "
							"Together with a free implementation of Unix, KDE "
							"constitutes a completely free and open computing "
							"platform available to anyone free of charge including "
							"its source code for anyone to modify.\n\n"
							"While there will always be room for improvement "
							"we believe to have delivered a viable alternative "
							"to some of the more commonly found and commercial "
							"operating systems/desktops combinations available "
							"today.\n\n"
							));
 		
  welcometext->setAlignment(AlignLeft | AlignTop | ExpandTabs | WordBreak);
  welcometext->setMargin(2);
  welcometext->setGeometry(0,0,300,500);
  
  // create scroll view for welcome text
  welcomesv = new QScrollView(topView);
  welcomesv->setGeometry(5, 92,340,324);
  welcomesv->setFrameStyle(0x0001 | 0x0020);
  welcomesv->setLineWidth(2);
  welcomesv->setMidLineWidth(0);
  welcomesv->addChild(welcometext);
  welcomesv->show();
  welcomesv->setResizePolicy(QScrollView::Manual);
  welcomesv->resizeContents(300,480);
	
  // read settings
  readSettings();
}

KWelcomeWidget::~KWelcomeWidget()
{
  saveSettings();
}

void KWelcomeWidget::resizeEvent(QResizeEvent *)
{
  quitButton->move(bottomView->width() - 85, 4);
  aboutButton->move(bottomView->width() - 172, 4);
  bigklogo->move(topView->width() - 250, topView->height() - 302);
  welcomesv->setGeometry(5,88,topView->width() - 260, topView->height() - 125);
  wizardButton->move(5, 89 + welcomesv->height());
  helpcenterButton->move(5 + wizardButton->width(),89 + welcomesv->height() );
  welcometext->setGeometry(0,0,welcomesv->width()-24, 600);
}

void KWelcomeWidget::slotAboutButtonClicked()
{
  aboutMenu->popup(aboutButton->mapToGlobal(QPoint(0,26)));
}

void KWelcomeWidget::slotAboutKDE()
{
  QMessageBox::about( 0L, i18n( "About KDE" ),
					  i18n("\nThe KDE Desktop Environment was written by the KDE Team,\n"
						   "a world-wide network of software engineers committed to\n"
						   "free software development.\n\n"
						   "Visit http://www.kde.org for more information on the KDE\n"
						   "Project. Please consider joining and supporting KDE.\n\n"
						   "Please report bugs at http://bugs.kde.org.\n"
						   ));
}

void KWelcomeWidget::slotAboutKWelcome()
{
  QMessageBox::about( 0L, i18n( "About KWelcome" ),
					  i18n("KWelcome\n\n"
						   "(c) 1998,99 Matthias Elter\nme@kde.org\n"));
}

void KWelcomeWidget::slotWizardStart()
{
  KProcess proc;	
  proc << "kdewizard";
  proc.start(KProcess::DontCare);
}

void KWelcomeWidget::slotHelpCenterStart()
{
  KProcess proc;	
  proc << "khelpcenter";
  proc.start(KProcess::DontCare);
}

void KWelcomeWidget::saveSettings()
{
  KConfig *conf = kapp->getConfig();
  conf->setGroup("General Settings");
  if (autostart_kwelcome->isChecked())
	conf->writeEntry("AutostartOnKDEStartup", "true");
  else
	conf->writeEntry("AutostartOnKDEStartup", "false");
  conf->sync();
  printf("KWelcome: Configuration written.\n");
}

void KWelcomeWidget::readSettings()
{
  KConfig *conf = kapp->getConfig();
  conf->setGroup("General Settings");
  QString tmp = conf->readEntry("AutostartOnKDEStartup", "true");
  
  if (tmp == "true")
	autostart_kwelcome->setChecked(true);
  else
	autostart_kwelcome->setChecked(false);	
}
