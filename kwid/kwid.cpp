/*
    This file is part of the KDE Help Center

    Copyright (c) 1998 René Beutler  (rbeutler@g26.ethz.ch)

    Parts of this code are taken from xwininfo. see copyright notice below.
    If something goes wrong please contact me, not the X Consortium!

    ----------------------------------------------------------------------------

    Copyright (c) 1987  X Consortium

    Permission is hereby granted, free of charge, to any person obtaining
    a copy of this software and associated documentation files (the
    "Software"), to deal in the Software without restriction, including
    without limitation the rights to use, copy, modify, merge, publish,
    distribute, sublicense, and/or sell copies of the Software, and to
    permit persons to whom the Software is furnished to do so, subject to
    the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
    OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR
    OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
    ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.

    Except as contained in this notice, the name of the X Consortium shall
    not be used in advertising or otherwise to promote the sale, use or
    other dealings in this Software without prior written authorization
    from the X Consortium.

*/


#include <stdio.h>
#include <strings.h>
#include <unistd.h>

#include <qrect.h>
#include <qtimer.h>

#include <kconfig.h>
#include <kapp.h>
#include <kwm.h>
#include "kwid.h"

#include <X11/Xutil.h>
#include <X11/keysymdef.h>
#include <X11/Xmu/WinUtil.h>
#include <X11/cursorfont.h>
#include <stdio.h>
#include <strings.h>

#include "../kassistant/config-kassistant.h"

KWidgetIdentification::KWidgetIdentification()
{
	dpy = XOpenDisplay( XDisplayName( 0L ) );	
	if( dpy == 0L )
		Fatal_Error( "unable to open display" );

	screen = DefaultScreen(dpy);           	

	eyes = 0L;
	timer = 0L;
}

void KWidgetIdentification::getIdentifierByUser( char *caption )
{	
	Window window;
	int x, y;

	if( Select_Window( &window, &x, &y ) == 1 )
	{
		if( searchWindow( window, x, y, caption ) == 1 )
		{
		} else {
			caption[0] = 0;
		}
	}
}

void KWidgetIdentification::showWidget( char *caption, bool exitApplication )
{
	exitApp = exitApplication;

	if( !getWidgetCoordinates( caption, &g ) )
	{
		cleanUp();
	}

	KConfig *pConfig = new KConfig( KApplication::kde_configdir()  + "/khelpcenterrc",
                               KApplication::localconfigdir() + "/khelpcenterrc" );
	CHECK_PTR( pConfig );
        pConfig->setGroup( "Widget" );

	bool bEyes      = pConfig->readBoolEntry( "EyesEnabled", WIS_DEFAULT_EYES_ENABLED );
	bool bHighlight = pConfig->readBoolEntry( "HighlightEnabled", WIS_DEFAULT_HIGHLIGHT_ENABLED );

	if( bHighlight ) {
		count = pConfig->readNumEntry( "HighlightNumber", WIS_DEFAULT_HIGHLIGHT_NUMBER );
		delay = pConfig->readNumEntry( "HighlightDelay",  WIS_DEFAULT_HIGHLIGHT_DELAY  );  
	}

	bool bWarp = pConfig->readBoolEntry( "WarpPointer", WIS_DEFAULT_WARP_POINTER );

	showWidget( g, bHighlight, count, delay, bEyes, bWarp );

	delete pConfig;	
}

void KWidgetIdentification::showWidget( QWidget *w, bool bHighlightEnabled, int count, int delay,
				       bool bEyesEnabled, bool bWarpPointer )
{
  geometry g;
  g.id = w->winId();
  g.x = w->x();
  g.y = w->y();
  g.width = w->width();
  g.height = w->height();

  showWidget( g, bHighlightEnabled, count, delay, bEyesEnabled, bWarpPointer );
}

void KWidgetIdentification::showWidget( geometry &g, bool bHighlightEnabled, int count, int delay,
				        bool bEyesEnabled, bool bWarpPointer )
{
  // center of found widget
  QPoint pos = QPoint( g.x, g.y );
  QPoint center = QPoint( pos.x() + g.width/2, 
			  pos.y() + g.height/2 );
  
  if( bEyesEnabled )
    {
      if( eyes )
	delete eyes;

      eyes = new KEyesWidget();
      CHECK_PTR( eyes );
                  
      // placement of eyes: horizontal: align centered with widget
      //                    vertical:   choose side with more space
      QRect geom;
      QRect region = KWM::getWindowRegion( KWM::currentDesktop() );
      
      if( center.y() - region.top() > region.bottom() - center.y() )
	{
	  // place above
	  geom = QRect( center.x() - eyesWidth/2, pos.y() - eyesHeight - eyesPad,
			eyesWidth, eyesHeight );
	} else {
	  // place below
	  geom = QRect( center.x() - eyesWidth/2, pos.y() + g.height + eyesPad,
			eyesWidth, eyesHeight );
	}
      // check wheter the eyes widget is fully visible
      if( region.contains( geom ) == FALSE ) {
	int d, dx = 0, dy = 0;
	if( (d = region.top() - geom.top() ) > 0 )
	  dy = d;
	if( (d = region.bottom() - geom.bottom() ) < 0 )
	  dy = d;
	if( (d = region.left() - geom.left() ) > 0 )
	  dx = d;
	if( (d = region.right() - geom.right() ) < 0 )
	  dx = d;
	geom.moveBy( dx, dy );
      }
      eyes->setGeometry( geom );
      eyes->lookAt( center );
      eyes->show();

      if( !bHighlightEnabled ) {
	if( timer )
	  delete timer;
	timer = new QTimer( this );
	connect( timer, SIGNAL(timeout()), this, SLOT(slotRemoveEyes()) );
	timer->start( count*delay << 1 );
      }
    }

  if( bHighlightEnabled ) {
    if( timer )
      delete timer;
    timer = new QTimer( this );
    connect( timer, SIGNAL(timeout()), this, SLOT(slotAddHighlight()) );
    timer->start( 0, TRUE );
  }
  
  if( bWarpPointer ) {
    XWarpPointer( qt_xdisplay(), qt_xrootwin(), qt_xrootwin(), 0, 0, 0, 0, center.x(), center.y() );
  }
}

void KWidgetIdentification::slotAddHighlight()
{
	GC gc = XCreateGC( dpy, g.id, 0, 0 );
	XSetState( dpy, gc, WhitePixel( dpy, screen ), WhitePixel(dpy,screen), GXxor, AllPlanes );
	XFillRectangle( dpy, g.id, gc, 0, 0, g.width, g.height );
	XFlush( dpy );

	disconnect( timer, SIGNAL(timeout()), this, SLOT(slotAddHighlight()) );
	connect( timer, SIGNAL(timeout()), this, SLOT(slotRemoveHighlight()) );
	timer->start( delay, TRUE );
	XFree( gc );
}

void KWidgetIdentification::slotRemoveHighlight()
{
	GC gc = XCreateGC( dpy, g.id, 0, 0 );
	XSetState( dpy, gc, WhitePixel( dpy, screen ), WhitePixel(dpy,screen), GXxor, AllPlanes );
	XFillRectangle( dpy, g.id, gc, 0, 0, g.width, g.height );	
	XFlush( dpy );

	disconnect( timer, SIGNAL(timeout()), this, SLOT(slotRemoveHighlight()) );
	count--;
	if( count > 0 )
	{
		connect( timer, SIGNAL(timeout()), this, SLOT(slotAddHighlight()) );
		timer->start( delay, TRUE );
	} else
		cleanUp();		

        XFree( gc );
}

void KWidgetIdentification::slotRemoveEyes()
{
  disconnect( timer, SIGNAL(timeout()), this, SLOT(slotRemoveEyes()) );
  delete timer;
  timer = 0L;
  
  if( eyes ) {
    delete eyes;
    eyes = 0L;
  }
}

void KWidgetIdentification::cleanUp()
{
	if( eyes )
	{
		delete( eyes );
		eyes = 0L;
	}

	if( timer )
	{
		delete( timer );
		timer = 0L;
	}

	if( exitApp )
	{
		kapp->exit();
		exit(0);	// needed, if event loop not running
	}
}

// ----------------------------------------------------------------------------
// common stuff

void KWidgetIdentification::Beep()
{
	XBell(dpy, 50);
}


void KWidgetIdentification::Fatal_Error(char *msg) const
{
	printf( "%s\n", msg );
	exit( 1 );
}


void KWidgetIdentification::fetchName( Window window, char *name ) const
{
	char *winName;

        name[0] = '\0';
	
	if( XFetchName(dpy, window, &winName) )
	{
		if( winName != 0L )
		{
			strncpy( name, winName, 256 );
			XFree( winName );
		}
	}

}


// ----------------------------------------------------------------------------
// get widget by name stuff


bool KWidgetIdentification::getWidgetCoordinatesRecursively( Window window, char *caption,
                                                            struct geometry *g, int level ) const
{
	int i, rel_x, rel_y, abs_x, abs_y;
	bool valid = FALSE;
	unsigned int width, height, border, depth, num_children;
	XWindowAttributes win_attributes;
	Window root_win, parent_win, *child_list = 0L;
	char name[256];

	// query the windows tree
	if (!XQueryTree(dpy, window, &root_win, &parent_win, &child_list,
	                &num_children))
		Fatal_Error("Can't query window tree.");

	// check all childrens
	for (i = (int)num_children - 1; i >= 0; i--)
	{
		// visible?
		if ( !XGetWindowAttributes(dpy, window, &win_attributes) )
			Fatal_Error("unable to get window attributes");
		if( win_attributes.map_state == IsViewable )
		{
			fetchName( child_list[i], name );
			if( strcmp( name, caption ) == 0 )
			{
       				// get the geometry of the current child window
				if (XGetGeometry(dpy, child_list[i], &root_win,
				    &rel_x, &rel_y, &width, &height, &border, &depth))
				{
					Window child;
	
					// get absolute coordinates
					if (XTranslateCoordinates (dpy, child_list[i], root_win,
					    0 ,0, &abs_x, &abs_y, &child))
					{
						g->x = abs_x - border;
						g->y = abs_y - border;
						g->width  = width;
						g->height = height;
						g->id     = child_list[i];
						valid = TRUE;
					}
				}
				if (child_list)
					XFree((char *)child_list);
				return valid;
			}
			// do the same for the subtree
			valid = getWidgetCoordinatesRecursively(child_list[i], caption, g, level+1);
			if( valid )
			{
				if (child_list)
					XFree((char *)child_list);
				return valid;
			}
		}
    	}

	if (child_list)
		XFree((char *)child_list);

	return valid;
}


bool KWidgetIdentification::getWidgetCoordinates( char *caption, struct geometry *g ) const
{
	Window root;

	root = RootWindow( dpy, screen );
	return getWidgetCoordinatesRecursively( root, caption, g, 0 );
}



// ----------------------------------------------------------------------------
// get name by mouseclick stuff


void KWidgetIdentification::searchWindowRecursively( Window window, int x, int y, char *caption, signed int level ) const
{
	int i;
	int rel_x, rel_y, abs_x, abs_y, tx, ty;
	unsigned int width, height, border, depth;
	Window root_win, parent_win;
	unsigned int num_children;
	Window *child_list;
	signed int deepestLevel = -1;

	// query the windows tree
	if (!XQueryTree(dpy, window, &root_win, &parent_win, &child_list,
	                &num_children))
		Fatal_Error("Can't query window tree.");

	// check all childrens
	for (i = (int)num_children - 1; i >= 0; i--)
	{
		// get the geometry of the current child window
		if (XGetGeometry(dpy, child_list[i], &root_win,
		    &rel_x, &rel_y, &width, &height, &border, &depth))
		{
			Window child;

			// get absolute coordinates
			if (XTranslateCoordinates (dpy, child_list[i], root_win,
			    0 ,0, &abs_x, &abs_y, &child))
			{
				// is our searched point inside?
				tx = x - (abs_x - border);
				ty = y - (abs_y - border);
				if( (tx >=0) && (ty >= 0) && (tx <= (signed)width) && (ty <= (signed)height) )
				{	
					// assume that the deepest level is always on top
					// so we look for the window with the deepest level
					if( level > deepestLevel )
					{
						//	printf("0x%lx", child_list[i]);  for debugging, print window id
						deepestLevel = level;
						fetchName( child_list[i], caption );
					}
					// do the same for the subtree
					searchWindowRecursively(child_list[i], x, y, caption, level+1);
				}
			}
		}
    	}

	if (child_list) XFree((char *)child_list);
}


int KWidgetIdentification::searchWindow( Window window, int x, int y, char *caption ) const
{
	caption[0] = '\0';

	if( window == RootWindow(dpy, screen) )
		fetchName( window, caption );
	else
		searchWindowRecursively( window, x, y, caption, 0 );

	return (caption[0] == '\0') ? 0 : 1;
}



int KWidgetIdentification::Select_Window( Window *target, int *x, int *y ) const
{
	int status;
	int xdown = 0, ydown = 0, xup = 0, yup = 0;
	Cursor cursor;
	XEvent event;
	KeySym keysym;
	Window target_win = None, root = RootWindow(dpy,screen), move;
	int buttons = 0, keys = 0, escapeIt = 0, valid = 0, lastValid = 0;
	char caption[256];

	// Make the target cursor
	cursor = XCreateFontCursor(dpy, XC_question_arrow);

	// Grab the pointer using target cursor, letting it room all over
	status = XGrabPointer(dpy, root, False,
			PointerMotionMask|ButtonPressMask|ButtonReleaseMask, GrabModeAsync,
			GrabModeAsync, root, cursor, CurrentTime);
	if (status != GrabSuccess) Fatal_Error("Can't grab the mouse.");

	// Grab the keyboard
	status = XGrabKeyboard(dpy, root, False, GrabModeAsync,
	                GrabModeAsync, CurrentTime);
	if (status != GrabSuccess) Fatal_Error("Can't grab the keyboard.");

	// Let the user select a window...
	while ( ((target_win == None) || (buttons != 0)) & (escapeIt == 0) )
	{
		// allow one more event for both Keyboard and Buttons
		XAllowEvents(dpy, SyncBoth, CurrentTime);
		XWindowEvent(dpy, root, PointerMotionMask|ButtonPressMask|ButtonReleaseMask|KeyPressMask|KeyReleaseMask, &event);
		switch (event.type) {
			case MotionNotify:
				move = event.xmotion.subwindow;
				if (move == None) move = root;				
				valid = searchWindow( move, event.xmotion.x, event.xmotion.y, caption );
				if( valid != lastValid )
				{
					lastValid = valid;
					cursor = XCreateFontCursor( dpy, (valid) ? XC_hand1 : XC_question_arrow );
					XChangeActivePointerGrab( dpy, PointerMotionMask|ButtonPressMask|ButtonReleaseMask, cursor, CurrentTime );
				}
				break;
			case ButtonPress:
				if (target_win == None) {
					xdown = event.xbutton.x;
					ydown = event.xbutton.y;
					target_win = event.xbutton.subwindow; // window selected
					if (target_win == None) target_win = root;
				}
				buttons++;
				break;
			case ButtonRelease:
				if (buttons > 0) // there may have been some down before we started
				{
					if( event.xbutton.button > Button1 )
					{
						// other button than button 1 pressed, exit
						escapeIt = 1;
					}
					xup = event.xbutton.x;
					yup = event.xbutton.y;
					buttons--;
				}
				break;
			case KeyPress:
				keys++;				
				break;
			case KeyRelease:				
				if( keys > 0 ) {
					XLookupString( (XKeyEvent *)&event.xkey, 0L, 0, &keysym, 0L);
					if( keysym == 0xFF1B )
					{
						// escape key pressed
						escapeIt = 1;
					}
				}
				break;
		}
	}

	XUngrabPointer(dpy, CurrentTime);       // Done with pointer
	XUngrabKeyboard(dpy, CurrentTime);	// Done with keyboard

	// Check whether the values are valid
	if( (escapeIt == 0) && ((xup-xdown + yup-ydown) < 20) )
	{
		*x = (xup+xdown) >> 1;
		*y = (yup+ydown) >> 1;
		*target = target_win;
		valid = 1;
        } else {
		*target = 0L;
		*x = *y = 0;
		valid = 0;
	}

	return valid;
}

#include "kwid.moc"
