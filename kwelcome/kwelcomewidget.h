/*
 *  kwelcomewidget.h - part of KWelcome
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

#ifndef KWELCOMEWIDGET_H
#define KWELCOMEWIDGET_H 

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

#include <kapp.h>
#include <qwidget.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qscrollview.h>
#include <qcheckbox.h>

class KWelcomeWidget : public QWidget
{
  Q_OBJECT

public:
  KWelcomeWidget(QWidget *parent = 0, const char *name = 0);
  virtual ~KWelcomeWidget();

public slots:
  void slotAboutButtonClicked();
  void slotAboutKWelcome();
  void slotAboutKDE();
  void slotWizardStart();
  void slotHelpCenterStart();

private:
  QWidget *topView, *bottomView;
  QCheckBox *autostart_kwelcome;
  QPopupMenu *aboutMenu;
  QPushButton *aboutButton, *quitButton, *wizardButton, *helpcenterButton;
  QScrollView *welcomesv;
  QLabel *welcometext, *bigklogo;

protected:
  void resizeEvent (QResizeEvent *);
  void saveSettings();
  void readSettings();
};

#endif // KWELCOMEWIDGET_H 
