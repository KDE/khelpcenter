/*
 *  searchwidget.cpp - part of the KDE Help Center
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
#include <qlistbox.h>
#include <qlineedit.h>
#include <qcheckbox.h>

#include <stdio.h>

#include "searchwidget.h"
#include "htmlsearch.h"
#include "mansearch.h"

ResultBox::ResultBox(QWidget *parent)
    : QListBox(parent)
{
    setMouseTracking(true);
}

void ResultBox::mouseMoveEvent(QMouseEvent *e)
{
    emit mouseOver(findItem(e->pos().y()));
}

void ResultBox::leaveEvent(QEvent *)
{
    emit mouseOver(-1);
}

int ResultBox::getItemYPos(int index)
{
    // FIXME: please note that this assumes all items are the fixed height.
    // Adding up heights is required for variable-height listbox items.
    // -Taj.
 
    return index ? (( index - 1 ) * itemHeight()) : 0;

#if 0
    if(itemYPos(index, &pos))
	return pos;
    else
	return -1;
#endif

}

SearchWidget::SearchWidget(QWidget *parent)
    : QWidget(parent)
{
    keyWordLabel = new QLabel(i18n("Enter keyword:"), this);
    searchString = new QLineEdit(this);
    connect(searchString, SIGNAL(returnPressed()), this, SLOT(slotSearch()));
   
    searchButton = new QPushButton(i18n("&Search"), this);
    searchButton->setFixedWidth(90);
    searchButton->setFixedHeight(24);
    connect(searchButton, SIGNAL(clicked()), this, SLOT(slotSearch()));

    docCheck = new QCheckBox (i18n("KDE &documentation"), this);
    manCheck = new QCheckBox (i18n("Unix &man pages"), this);
    infoCheck = new QCheckBox (i18n("GNU &info pages"), this);
    infoCheck->setEnabled(false);

    docCheck->setChecked(true);

    resultLabel = new QLabel(i18n("Results:"), this);
    resultList = new ResultBox(this);

    connect(resultList, SIGNAL(mouseOver(int)), this, SLOT(slotMouseOver(int)));
    connect(resultList, SIGNAL(highlighted(int)), this, SLOT(slotMatchSelected(int)));
    connect(resultList, SIGNAL(selected(int)), this, SLOT(slotMatchSelected(int)));

    tipLabel = new TipLabel;
    tipLabel->hide();
}

SearchWidget::~SearchWidget()
{
    matchList.clear(); //auto delete matches
    delete tipLabel;
}

void SearchWidget::resizeEvent(QResizeEvent *)
{
    keyWordLabel->setGeometry(2, 2, width(), 20);
    searchString->setGeometry(2, 22, width() - 4, 24);
    searchButton->move(width()- 92, 50);
    docCheck->setGeometry(2, 75, width() - 4, 20);
    manCheck->setGeometry(2, 95, width() - 4, 20);
    infoCheck->setGeometry(2, 115, width() - 4, 20);
    resultLabel->setGeometry(2, 138, width()-4, 20);
    resultList->setGeometry(2, 160, width()-4, height()-160);
}

void SearchWidget::tabSelected()
{
    searchString->setFocus();
}

void SearchWidget::slotSearch()
{
    resultList->clear();
    QString query = searchString->text();

    matchList.setAutoDelete(TRUE);
    matchList.clear();
    
    if (docCheck->isChecked())
    {
	HTMLSearch htmlSearch(&matchList);
	htmlSearch.search(query);
    }
  
    if (manCheck->isChecked())
    {
	ManSearch manSearch(&matchList);
	manSearch.search(query);
    }
  
    resultList->setAutoUpdate(false);
    for (Match *match = matchList.first(); match != 0; match = matchList.next())
	resultList->insertItem(match->getTitle());
    resultList->setAutoUpdate(true);
    resultList->repaint();
}

void SearchWidget::slotMatchSelected(int index)
{
    int count = 0;

    for (Match *match = matchList.first(); match != 0; match = matchList.next())
    {
	if (count == index)
	{
	    QString url = match->getURL();
	    emit matchSelected(url);
	    break;
	}
	count++;
    }
}

void SearchWidget::slotMouseOver(int index)
{
    if (index < 0)
    {
	tipLabel->hide();
	return;
    }

    QString text = resultList->text(index);
    if (text.isEmpty())
	return;

    int yPos = resultList->getItemYPos(index);
    QPoint gPos = resultList->mapToGlobal(QPoint(4, yPos));

    tipLabel->setText(text);
    if (tipLabel->width() > (resultList->width()-16))
    {
	tipLabel->move(gPos.x(),gPos.y()+25);
	tipLabel->show();
    } 
    else
	tipLabel->hide();

    //printf("mouseover: %d\n",yPos);fflush(stdout);
}
