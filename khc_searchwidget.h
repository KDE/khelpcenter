/*
 *  khc_searchwidget.h - part of the KDE Help Center
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

#ifndef __khc_searchwidget_h__
#define __khc_searchwidget_h__

#include <qwidget.h>
#include <qlabel.h>
#include <qlistbox.h>

#include "khc_searchmatch.h"

class QPushButton;
class QLineEdit;
class QCheckBox;

class TipLabel : public QLabel
{
    Q_OBJECT

 public:
    TipLabel()
	: QLabel(0, "TipLabel",
		 WStyle_Customize | WStyle_NoBorder | WStyle_Tool)
	{
	    setAlignment(AlignVCenter | AlignLeft);
	    setAutoResize(true);
	    setBackgroundColor(QColor(240,255,240));
	    setFrameStyle(Box | Raised);
	    setLineWidth(1);
	    //setAutoMask(FALSE);
	}
};


class ResultBox : public QListBox
{
    Q_OBJECT

	public:
    ResultBox(QWidget *parent = 0);
    int getItemYPos(int index);

 protected:
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void leaveEvent(QEvent *e);

 signals:
    void mouseOver(int index);
};  


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
    void slotMouseOver(int index); 

 private:
    QLabel *keyWordLabel, *resultLabel;
    TipLabel *tipLabel;
    QPushButton *searchButton;
    ResultBox *resultList;
    QLineEdit *searchString;
    QCheckBox *docCheck, *manCheck, *infoCheck;
    MatchList matchList;
};

#endif
