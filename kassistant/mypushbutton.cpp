// this file has been taken from knotes!

#include <qwidget.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qdrawutil.h>
#include "mypushbutton.h"

myPushButton::myPushButton(QWidget *parent, const char* name)
 : QPushButton( parent, name ){
    setFocusPolicy(NoFocus);
    flat = TRUE;
    last_button = 0;
}

void myPushButton::enterEvent( QEvent * )
{
	flat = FALSE;
	repaint(FALSE);
}

void myPushButton::leaveEvent( QEvent * )
{
	flat = TRUE;
	repaint();
}

void myPushButton::paint(QPainter *painter)
{
	if ( isDown() || (isOn() && !flat)) {
		if ( style() == WindowsStyle )
		      qDrawWinButton( painter, 0, 0, width(),
			      height(), colorGroup(), TRUE );
		else
		      qDrawShadePanel( painter, 0, 0, width(),
			       height(), colorGroup(), TRUE, 2, 0L );
	}
	else if (!flat ) {
		if ( style() == WindowsStyle )
			qDrawWinButton( painter, 0, 0, width(), height(),
			colorGroup(), FALSE );
		else {
			qDrawShadePanel( painter, 0, 0, width(), height(),
		       		colorGroup(), FALSE, 2, 0L );
//       painter->setPen(black);
//       painter->drawRect(0,0,width(),height());
		}
	}

	int dx = ( width() - pixmap()->width() ) / 2;
	int dy = ( height() - pixmap()->height() ) / 2;
	if ( isDown() && style() == WindowsStyle ) {
		dx++;
		dy++;
	}

  painter->drawPixmap( dx, dy, *pixmap());

  /*
  QPen pen = painter->pen();
  pen.setWidth(2);
  pen.setColor(backgroundColor().light(180));
  //  pen.setColor(white);
  painter->setPen(pen);

  painter->drawLine(2,2,width()-3,height()-2);
  painter->drawLine(width()-3,2,2,height()-2);
  */
}

void myPushButton::mousePressEvent( QMouseEvent *e){

  if ( isDown())
    return;

  bool hit = hitButton( e->pos() );
  if ( hit ){
    last_button = e->button();
    setDown( TRUE );
    repaint( FALSE );
    emit pressed();
  }
}

void myPushButton::mouseReleaseEvent( QMouseEvent *e){
  if ( !isDown() ){
    last_button = 0;
    return;
  }
  bool hit = hitButton( e->pos() );
  setDown( FALSE );
  if ( hit ){
    if ( isToggleButton() )
      setOn( !isOn() );
    repaint( FALSE );
    if ( isToggleButton() )
      emit toggled( isOn() );
    emit released();
    emit clicked();
  }
  else {
    repaint();
    emit released();
  }
  last_button = 0;
}

void myPushButton::mouseMoveEvent( QMouseEvent *e ){

  if (!last_button)
    return;

  if ( !(e->state() & LeftButton) &&
       !(e->state() & MidButton) &&
       !(e->state() & RightButton))
    return;


  bool hit = hitButton( e->pos() );
  if ( hit ) {
    if ( !isDown() ) {
      setDown(TRUE);
      repaint(FALSE);
      emit pressed();
    }
  } else {
    if ( isDown() ) {
      setDown(FALSE);
      repaint();
      emit released();
    }
  }
}

#include "mypushbutton.moc"