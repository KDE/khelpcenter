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

//
// Espen: 2000-04-20
// Coverted to QLayouts and got rid of (almost) all manual geometry calls. 
// Removed the old "About KDE" dialog box (using KAboutKDE instead).
// Added keyboard accels for Escape and F1 as well.
//

#include "kwelcome.h"

#include <kaboutkde.h>
#include <kapp.h>
#include <kconfig.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kprocess.h>
#include <kstddirs.h>
#include <kglobal.h>

#include <qcheckbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>

KWelcome::KWelcome(QWidget *parent, const char *name)
  : QWidget(parent, name, WStyle_Tool )
{
  setCaption(i18n("Welcome to the K Desktop Environment"));

  // setup topView
  topView = new QWidget(this);
  topView->setBackgroundColor( white );
  
  // setup bottomView
  bottomView = new QWidget(this);
  
  // setup the top level layout manager
  QVBoxLayout *topLayout = new QVBoxLayout(this);
  topLayout->addWidget(topView,1);
  topLayout->addWidget(bottomView);

  // welcome image
  QLabel *welcome = new QLabel(topView);
  QVBoxLayout *vlay = new QVBoxLayout( topView );
  vlay->addWidget( welcome, 0, AlignTop|AlignLeft );

  QPixmap welcome_pm(locate("data", "kwelcome/pics/welcome.png"));	
  welcome->setPixmap(welcome_pm);

  //
  // Overlay buttons on welcome pixmap label. We need to do one manual 
  // geometry setting with real pixel coordinates here.
  //
  helpcenterButton = new QPushButton(i18n("Get &help"), topView);
  connect(helpcenterButton,SIGNAL(clicked()),this,SLOT(slotHelpCenterStart()));
  helpcenterButton->adjustSize(); // Since there is no layout manager

  wizardButton = new QPushButton(i18n("Configuration &wizard"), topView);
  connect(wizardButton, SIGNAL(clicked()), this, SLOT(slotWizardStart()));
  wizardButton->adjustSize(); // Since there is no layout manager

  helpcenterButton->move( 15, 330 );
  wizardButton->move( helpcenterButton->x() + 
		      helpcenterButton->sizeHint().width() + 4, 
		      helpcenterButton->y() );

  // create 'start on every KDE startup' checkbox
  autostart_kwelcome = 
    new QCheckBox( i18n("Show this dialog on KDE startup."), bottomView );
  autostart_kwelcome->setChecked(TRUE);

  // create aboutButton
  aboutButton = new QPushButton(i18n("&About KDE"), bottomView);
  connect(aboutButton, SIGNAL(clicked()), this, SLOT(slotAboutKDE()));

  // create quitButton
  quitButton = new QPushButton(i18n("&Close"), bottomView);
  connect(quitButton, SIGNAL(clicked()), kapp, SLOT(quit()));

  // Action bar layout manager
  QHBoxLayout *hlay = new QHBoxLayout(bottomView, 4, 4 );
  hlay->addWidget(autostart_kwelcome, 1, AlignLeft );
  hlay->addWidget(aboutButton);
  hlay->addWidget(quitButton);

  //
  // Espen: Does this look better? I think so. Give action buttons 
  // the same width.
  //
  int w1 = aboutButton->sizeHint().width();
  int w2 = quitButton->sizeHint().width();
  int w3 = QMAX( w1, w2 );
  aboutButton->setFixedWidth( w3 );
  quitButton->setFixedWidth( w3 );


  //
  // Placement on screen
  //
  move( QApplication::desktop()->width()/2 - width()/2,
        QApplication::desktop()->height()/2 - height()/2 );
  setFixedSize( sizeHint() );

  // read settings
  readSettings();
}

KWelcome::~KWelcome()
{
  saveSettings();
}

void KWelcome::slotAboutKDE()
{
  KAboutKDE *dialog = new KAboutKDE( topLevelWidget(), "aboutkde", true );
  dialog->exec();
  delete dialog;
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
  KConfig *conf = KGlobal::config();
  KConfigGroupSaver saver(conf, "General Settings");
  conf->writeEntry( "AutostartOnKDEStartup", autostart_kwelcome->isChecked() );
  conf->sync();
}

void KWelcome::readSettings()
{
  KConfig *conf = KGlobal::config();
  KConfigGroupSaver saver(conf, "General Settings");
  QString tmp = conf->readEntry("AutostartOnKDEStartup", "true");
  
  autostart_kwelcome->setChecked(conf->readBoolEntry("AutostartOnKDEStartup",
                                                     true));
}


void KWelcome::keyPressEvent( QKeyEvent *e )
{
  if( e->key() == Key_Escape )
  {
    quitButton->animateClick();
  }
  else if( e->key() == Key_F1 )
  {
    helpcenterButton->animateClick();
  }
  else
  {
    QWidget::keyPressEvent(e);
  }
}

#include "kwelcome.moc"
