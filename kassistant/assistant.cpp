/*
    This file is part of the KDE Help Center

    Copyright (C) 1998 René Beutler (rbeutler@g26.ethz.ch)
*/


#include <qdir.h>	// don't remove this, it prevents me from those damn header problems
#include <qmovie.h>

#include <komApplication.h>
#include <kiconloader.h>
#include <kprocess.h>
#include <kwm.h>

#include <qtooltip.h>
#include <kmessagebox.h>

#include "assistant.h"
#include "version.h"

#include <X11/Xlib.h>

Assistant::Assistant( QWidget *parent ) : QLabel( parent, "", WStyle_DialogBorder )
{
	setCaption( i18n("Assistant") );
	KWM::setDecoration( winId(), KWM::normalDecoration | KWM::staysOnTop );	
//	setFrameStyle( QFrame::WinPanel | QFrame::Raised );

	mousePressed = mouseDragged = FALSE;
	qmovie = 0L;
	browser = 0L;

	setMovieSource( "knlogo1.gif" );

	QToolTip::add( this, i18n("Click to get Help!") );
}

void Assistant::setMovieSource( const char *name )
{
	QString file = kapp->kde_datadir().copy();
	file += "/kassistant/";
	file += name;

	if( qmovie )
		delete qmovie;

	qmovie = new QMovie( (const char *) file );
	CHECK_PTR( qmovie );
	qmovie->connectStatus( this, SLOT(slotMovieStatus(int)) );
	qmovie->connectResize( this, SLOT(slotMovieResize(const QSize&)) );

	setMovie( *qmovie );
}

void Assistant::mousePressEvent( QMouseEvent *e )
{
	mousePressed = TRUE;

	if( e->button() == LeftButton )
	{
		pointerOffset = e->globalPos();
		geometry = KWM::geometry( winId() );
	}
	else if( e->button() == RightButton )
	{
		QPopupMenu *popup = new QPopupMenu();
		popup->insertItem( i18n("Settings..."), this, SLOT(slotSettings()) );
		popup->insertSeparator();
		popup->insertItem( i18n("About kassistant..."), this, SLOT(slotAbout()) );
		popup->exec(QCursor::pos());
	}
}

void Assistant::mouseReleaseEvent( QMouseEvent *e )
{
	mousePressed = FALSE;

	if( mouseDragged == FALSE )
	{
		if( browser == 0L )
			browser = new Browser();
		else
			browser->show();
	}
        else
	{
		mouseDragged = FALSE;
	}	
}

void Assistant::mouseMoveEvent( QMouseEvent *e )
{
	if( mousePressed )
	{
		mouseDragged = TRUE;

		QPoint d = e->globalPos() - pointerOffset;
		QRect geoMoved = geometry;
		geoMoved.moveBy( d.x(), d.y() );
		KWM::setGeometry( winId(), geoMoved );
	}
}

void Assistant::closeEvent( QCloseEvent * )
{
	kapp->quit();
}

void Assistant::slotMovieStatus( int s )
{
	switch ( s )
	{
		case QMovie::SourceEmpty:
			setText( i18n("Could not load animation\n") );
			adjustSize();
			setAlignment( AlignCenter );
			setBackgroundColor( backgroundColor() );
			show();
			break;
		default:
			if ( movie() )	// for flicker-free animation
				setBackgroundMode( NoBackground );
	}
}

void Assistant::slotMovieResize( const QSize &s )
{
	resize( s );
	setFixedSize( s );
	show();
}

void Assistant::slotConfigure()
{
}

void Assistant::slotSettings() const
{
	Assistant::settings();
}

void Assistant::settings()
{
	KProcess proc;
	proc << "kcmhelpcenter";
	proc << "{" << "assistant" << "," << "browser" << "," << "widget" << "}";
	proc.start( KProcess::DontCare, KProcess::NoCommunication );
}

void Assistant::slotAbout() const
{
	Assistant::about();
}

void Assistant::about()
{
	KMessageBox mb;
	QString at = i18n( "KDE Help Assistant" );
	at += "\n";
	at += i18n( "Version" );
	at += " ";
	at += KASSISTANT_VERSION;
	at += i18n( "\n\nCopyright (c) 1998 René Beutler <rbeutler@g26.ethz.ch>" );
	at += i18n( "\n\nThis program is licensed under the GNU General Public License (GPL)."\
	            "\nKAssistant comes with ABSOLUTELY NO WARRANY to the extent permitted by applicable law.");
	mb.setText( at );
	mb.setCaption( i18n("About KDE Help Assistant") );
	mb.setIcon( QMessageBox::Information );
	mb.show();
}

#include "assistant.moc"
