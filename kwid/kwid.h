/*
 *   This file is part of the KDE Help Center
 *
 *   Copyright (c) 1998 René Beutler  (rbeutler@g26.ethz.ch)
 *
 */

#ifndef __KWID_H
#define __KWID_H

#include <qobject.h>
#include <qwidget.h>

#include "eyes.h"

#include <X11/Xlib.h>

typedef struct _geometry {
	Window id;
	int x, y, width, height;
} geometry;


class KWidgetIdentification : public QObject {

	Q_OBJECT

public:
	
	KWidgetIdentification();
	void getIdentifierByUser( char *caption );
	void showWidget( char *caption, bool exit = FALSE );
	void showWidget( geometry &g,
	                 bool bHighlightEnabled, int count, int delay,
	                 bool bEyesEnabled,
                         bool bWarpPointer );
	void showWidget( QWidget *w, 
                         bool bHighlightEnabled, int count, int delay, 
                         bool bEyesEnabled, 
                         bool bWarpPointer );

private slots:
	
	void slotAddHighlight();
	void slotRemoveHighlight();
	void slotRemoveEyes();

protected:

	void Beep();
	void Fatal_Error( char *msg ) const;

private:
	void cleanUp();

	/**
	 *  return the name of a window given its window id
	 *  internal use only.
	 */
	void fetchName( Window window, char *name ) const;

	/**
	 * internal use only.
	 * Limitations: finds the widgets only if it is on the current desktop
	 *
	 */
	bool getWidgetCoordinatesRecursively( Window window, char *caption,
                                              geometry *g, int level ) const;

	/**
	 * internal use only.
	 *
	 */
	bool getWidgetCoordinates( char *caption, geometry *g ) const;

	/**
	 *   This routine searches all child windows of window, which include the point given by
	 *   absolute coordinates (x,y).
	 *   It then returns the name of the child which is on top.
	 *
	 *   internal use only.
	 */
	void searchWindowRecursively( Window window, int x, int y,
	                              char *caption, signed int level )	const;
	
	int searchWindow( Window window, int x, int y, char *caption ) const;

	/*
	 * This routine lets the user click anywhere on his screen using button 1.
	 * He can exit by either pressing the <ESCAPE> key, pressing a mouse button
	 * other than button 1, or by dragging the mouse too far.
	 *
	 * If successful the routine returns 1 and sets target to the window
	 * where he pointed to, and the mouse coordinates are stored in x, y.
	 *
	 * Otherwise it returns 0.
	 */
        int Select_Window( Window *target, int *x, int *y ) const;
	
	geometry g;
	Display *dpy;
	int screen;
	KEyesWidget *eyes;
	int count, delay;	// highlight
	QTimer *timer;
	bool exitApp;

	static const int eyesWidth = 150, eyesHeight = 100, eyesPad = 20;
};

#endif
