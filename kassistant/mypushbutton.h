// this file has been taken from knotes!

#ifndef __MYPUSHBUTTON_H
#define __MYPUSHBUTTON_H

#include <qpushbutton.h>

class myPushButton: public QPushButton
{
	Q_OBJECT

public:
	myPushButton ( QWidget *parent=0, const char* name=0 );
	~myPushButton () {}
	bool flat;
	int last_button;

protected:
	void enterEvent( QEvent * );
	void leaveEvent( QEvent * );
	void mousePressEvent( QMouseEvent *e);
	void mouseReleaseEvent( QMouseEvent *e);
	void mouseMoveEvent( QMouseEvent *e);
	void paint( QPainter *_painter );
	void drawButton( QPainter *p ){paint(p);}
	void drawButtonLabel( QPainter *p ){paint(p);}
};

#endif
