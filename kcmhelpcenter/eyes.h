#ifndef __EYES_H
#define __EYES_H

#include <qframe.h>
#include <qevent.h>
#include <qpoint.h>
#include <qpainter.h>

typedef struct _myPoint {
	double x, y;
} myPoint;

class KEyesWidget : public QFrame
{
	Q_OBJECT

public:

	KEyesWidget( QWidget *parent=0, const char * name=0, WFlags f=0 );
	~KEyesWidget();

public slots:

	void lookAt( QPoint &p );

protected:
	virtual void paintEvent( QPaintEvent * );
	virtual void resizeEvent( QResizeEvent * );

private:
	myPoint computePupil( double eyeX, QPoint mouse ) const;	
	void computePupils( QPoint mouse, myPoint pupils[2] ) const;

	void eyeLiner( QPainter & );
	void eyeBalls(  QPainter & );
	void eyeShape( QPainter & );

	double xScale, yScale;
	double xTranslate, yTranslate;

	bool		shapeWindow;
	QPoint		mouse;		// old mouse position
	myPoint		pupil[2];	// pupil position

	// defined constants
	const static float eyePadding   = 0.2,
	                   eyeThickness = 0.4,
	                   eyeRadius    = 2.0,
	                   ballRadius   = 0.32,
	                   ballPad      = 0.4;

	// calculated constants
	const static float eyeX         = eyePadding + eyeRadius,
	                   eyeDiameter  = 2.0 * eyeRadius,
	                   irisRadius   = eyeRadius - eyeThickness,
                           irisDiameter = 2.0 * irisRadius,
	                   eyesWidth    = 2.0 * (eyePadding + eyeDiameter),
	                   eyesHeight   = eyeDiameter,
	                   ballDiameter = 2.0 * ballRadius,
	                   ballAllowed  = eyeRadius - eyeThickness - ballPad - ballRadius;
};

#endif

