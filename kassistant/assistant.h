/*
    This file is part of the KDE Help Center

    Copyright (C) 1998 René Beutler (rbeutler@g26.ethz.ch)
*/


#ifndef __ASSISTANT_H
#define __ASSISTANT_H

#include <qlabel.h>
#include <qevent.h>
#include <qmovie.h>

#include "browser.h"


extern Browser *browser;


class Assistant : public QLabel {
        Q_OBJECT

public:

	Assistant( QWidget *parent = 0L );
	void setMovieSource( const char *name );
	static void about();
	static void settings();

protected:

	virtual void mousePressEvent( QMouseEvent * );
	virtual void mouseReleaseEvent( QMouseEvent *);
	virtual void mouseMoveEvent( QMouseEvent *);
	virtual void closeEvent( QCloseEvent *);

public slots:

	void slotConfigure();

private slots:

	void slotMovieStatus( int );
	void slotMovieResize( const QSize & );
	void slotAbout() const;
	void slotSettings() const;

private:

	bool mousePressed, mouseDragged;
	QPoint pointerOffset;
	QRect geometry;
	QMovie *qmovie;
};

#endif
