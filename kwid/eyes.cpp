/*
    This file is part of the KDE Help Center

    Copyright (c) 1999 René Beutler (rbeutler@g26.ethz.ch)
*/

#include <math.h>
#include <stdio.h>

#include <qbitmap.h>

#include "eyes.h"


KEyesWidget::KEyesWidget( QWidget *parent, const char * name, WFlags f) :
             QFrame( parent, name/*, WType_TopLevel | WStyle_Customize | WStyle_Tool*/ )
{
	setBackgroundMode( NoBackground );	

	resize( 150, 100 );
}

KEyesWidget::~KEyesWidget()
{
}

void KEyesWidget::lookAt( QPoint &p )
{
	mouse = p;
}

void KEyesWidget::paintEvent( QPaintEvent * )
{
	computePupils( mouse, pupil );

	QPainter painter( this );
	painter.translate( xTranslate, yTranslate );
	eyeLiner( painter );
	eyeBalls( painter );
}

void KEyesWidget::resizeEvent( QResizeEvent * )
{
	xTranslate = width() / 2.0;
        yTranslate = height() / 2.0;
	
	xScale = width() / eyesWidth;
	yScale = height() / eyesHeight;

	QBitmap shapeMask( width(), height() );
	shapeMask.fill( color0 );
	QPainter painter( &shapeMask );
	painter.translate( xTranslate, yTranslate );
	eyeShape( painter );
	painter.end();
	setMask( shapeMask );
}

myPoint KEyesWidget::computePupil( double eye_x, QPoint mouse ) const
{
	mouse -= ( geometry().topLeft() + QPoint( xTranslate, yTranslate ));

	ASSERT( xScale != 0.0 );
	ASSERT( yScale != 0.0 );
	double mx = (double) mouse.x() / xScale - eye_x,
	       my = (double) mouse.y() / yScale;

	// (mx, my) is the mouse position relative to the origin

	myPoint ret;

	if( mx == 0.0 && my == 0.0 )
	{
		ret.x = eye_x;
		ret.y = 0.0;
	} else {
		double r = hypot( mx, my );
		if( r <= ballAllowed )
		{
			ret.x = mx + eye_x;
			ret.y = my;
		} else {
			double phi = atan2( my, mx );
			ret.x = eye_x + ballAllowed * cos(phi);
			ret.y = ballAllowed * sin( phi );
		}
	}

	return ret;
}

void KEyesWidget::computePupils( QPoint mouse, myPoint pupils[2] ) const
{
	pupils[0] = computePupil( -eyeX, mouse );
	pupils[1] = computePupil( +eyeX, mouse );
}

void KEyesWidget::eyeLiner( QPainter &painter )
{
	// black outline
	painter.setPen( black );
	painter.setBrush( black );
	// left
	painter.drawEllipse( xScale * (-eyeX - eyeRadius), -yScale * eyeRadius,
	                     xScale * eyeDiameter, yScale * eyeDiameter );
	// right
	painter.drawEllipse( xScale * (eyeX - eyeRadius), -yScale * eyeRadius,
	                     xScale * eyeDiameter, yScale * eyeDiameter );

	// white iris
	painter.setPen( white );
	painter.setBrush( white );
	// left
	painter.drawEllipse( xScale * (-eyeX - irisRadius), -yScale * irisRadius,
	                     xScale * irisDiameter, yScale * irisDiameter );
	// right
	painter.drawEllipse( xScale * (eyeX - irisRadius), -yScale * irisRadius,
	                     xScale * irisDiameter, yScale * irisDiameter );
}

void KEyesWidget::eyeBalls( QPainter &painter )
{
	painter.setPen( black );
	painter.setBrush( black );

	// left
	painter.drawEllipse( xScale * (pupil[0].x - ballRadius), yScale * (pupil[0].y - ballRadius),
	                     xScale * ballDiameter, yScale * ballDiameter );
	// right
	painter.drawEllipse( xScale * (pupil[1].x - ballRadius), yScale * (pupil[1].y - ballRadius),
	                     xScale * ballDiameter, yScale * ballDiameter );
}

void KEyesWidget::eyeShape( QPainter &painter )
{
	painter.setPen( color1 );
	painter.setBrush( color1 );

	painter.drawEllipse( xScale * (eyeX - eyeRadius), -yScale * eyeRadius,
	                     xScale * eyeDiameter, yScale * eyeDiameter );
	painter.drawEllipse( xScale * (-eyeX - eyeRadius), -yScale * eyeRadius,
	                     xScale * eyeDiameter, yScale * eyeDiameter );
}

#include "eyes.moc"
