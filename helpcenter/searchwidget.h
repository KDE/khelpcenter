/*
 *  searchwidget.h - part of the KDE Help Center
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

#ifndef __searchwidget_h__
#define __searchwidget_h__

#include <qwidget.h>
#include "searchmatch.h"

class QLabel;
class QPushButton;
class QListBox;
class QLineEdit;
class QCheckBox;

class SearchWidget : public QWidget
{
  Q_OBJECT

public:
  SearchWidget (QWidget *parent = 0);
  ~SearchWidget();
  void tabSelected();

signals:
 void matchSelected(QString match);

protected:
  void resizeEvent (QResizeEvent *);

public slots:
  void slotSearch();

private slots:
  void slotMatchSelected(int index);

private:
  QLabel *keyWordLabel, *resultLabel;
  QPushButton *searchButton;
  QListBox *resultList;
  QLineEdit *searchString;
  QCheckBox *docCheck, *manCheck, *infoCheck;
  MatchList matchList;
};

#endif
