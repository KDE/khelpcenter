/*
 *  indexwidget.cpp - part of the KDE Help Center
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

#include <klocale.h>

#include <qlabel.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlistbox.h>

#include "indexwidget.h"

IndexWidget::IndexWidget(QWidget *parent)
    : QWidget(parent)
{
    keyWordLabel = new QLabel(i18n("Enter keyword:"), this);
    keyWordString = new QLineEdit(this);
    connect(keyWordString, SIGNAL(returnPressed()), this, SLOT(slotFind()));

    topicLabel = new QLabel(i18n("Topics (man/info pages):"), this);

    indexList = new QListBox(this);

    rebuildButton = new QPushButton(i18n("&Reload topics"), this);
    rebuildButton->setFixedWidth(120);
    rebuildButton->setFixedHeight(24);
    connect(rebuildButton, SIGNAL(clicked()), this, SLOT(slotBuildIndex()));
}

IndexWidget::~IndexWidget()
{

}

void IndexWidget::resizeEvent(QResizeEvent *)
{
    keyWordLabel->setGeometry(2, 2, width(), 20);
    keyWordString->setGeometry(2, 22, width() - 4, 24);
    topicLabel->setGeometry(2, 52, width(), 20);

    indexList->setGeometry(2, 74, width()-4, height()-102);
    rebuildButton->move(2, height() - 24);
}

void IndexWidget::tabSelected()
{
    keyWordString->setFocus();
    slotBuildIndex();
}

void IndexWidget::slotBuildIndex()
{
    //indexList->clear();
  
    //indexList->setAutoUpdate(false);
    //for (int i=0;i<5000;i++)
    //	indexList->insertItem("Test item");
    //indexList->setAutoUpdate(true);
    //indexList->repaint();
}

void IndexWidget::slotFind()
{
}
