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
	static const float eyePadding, eyeThickness,
	    eyeRadius, ballRadius, ballPad;

	// calculated constants
	static const float eyeX, eyeDiameter, irisRadius, irisDiameter,
	    eyesWidth, eyesHeight, ballDiameter, ballAllowed;
};

#endif

