/*
 * This file is part of the KDE Help Center
 *
 * Copyright (c) 1999, René Beutler (rbeutler@g26.ethz.ch)
 *
 */

#include <qdir.h>
#include <kprocess.h>
#include <qlayout.h>

#include "widget.h"
#include "../kassistant/config-kassistant.h"  // include default values directly from kassistant
#include <klocale.h>


KWidgetConfig::KWidgetConfig( QWidget* parent, const char* name, bool ) : KConfigWidget( parent, name )
{
  // main groupboxes
  pHighGB    = newGroupBox( "Highlighting" );
  pEyesGB    = newGroupBox( "Eyes"         );
  pPointerGB = newGroupBox( "Pointer"      ); 
  pTestGB    = newGroupBox( "Test"         );
  
  // all widgets
  pNumberLabel  = newLabel( "Number:", pHighGB );
  pTimesLabel   = newLabel( "times",   pHighGB );
  pDelayLabel   = newLabel( "Delay:",  pHighGB );
  pMsLabel      = newLabel( "ms",      pHighGB );
  pFindMeLabel  = newLabel( "Find Me", pTestGB );

  pNumberLCD    = newLCD( pHighGB );
  pDelayLCD     = newLCD( pHighGB );

  pNumberSlider = new QSlider(   1,   10,  1,   1, QSlider::Horizontal, pHighGB );
  pDelaySlider  = new QSlider( 100, 1000, 50, 100, QSlider::Horizontal, pHighGB );
  pNumberSlider->setTickmarks( QSlider::Below );
  pDelaySlider ->setTickmarks( QSlider::Below );

  pHighlightCheckbox = newCheckbox( "Enable Highlighting", pHighGB );
  pEyesCheckbox      = newCheckbox( "Enable Eyes",         pEyesGB );
  pWarpCheckbox      = newCheckbox( "Warp pointer to center of widget", pPointerGB );
  
  pTestButton   = new QPushButton( i18n("Test"), pTestGB );
  pTestButton->setFixedSize( pTestButton->sizeHint() );
 
  // layout management
  QVBoxLayout *pTop = new QVBoxLayout( this, 10, 5  );
 
  pTop->addWidget( pHighGB,    3 );
  QHBoxLayout *hbox = new QHBoxLayout();
  pTop->addLayout( hbox );
  hbox->addWidget( pEyesGB,    2 );
  hbox->addSpacing( 10 );
  hbox->addWidget( pPointerGB, 3 );
  pTop->addWidget( pTestGB,    2 );

  // Create a layout for the Highlight check box
  QVBoxLayout *vbox = new QVBoxLayout( pHighGB, 10 );
  vbox->addSpacing( pHighGB->fontMetrics().height() );
  vbox->addWidget( pHighlightCheckbox );
  vbox->addStretch();
 
  QGridLayout *grid = new QGridLayout( 6, 3 );
  vbox->addLayout( grid );

  grid->addWidget( pNumberLabel,  0, 0 );
  grid->addWidget( pNumberLCD,    0, 2 );
  grid->addWidget( pTimesLabel,   0, 4 );
  grid->addWidget( pNumberSlider, 0, 6 );

  grid->addRowSpacing( 1, pNumberLCD->height() );

  grid->addWidget( pDelayLabel,   2, 0 );
  grid->addWidget( pDelayLCD,     2, 2 );
  grid->addWidget( pMsLabel,      2, 4 );
  grid->addWidget( pDelaySlider,  2, 6 );

  vbox->addStretch();

  // Create a layout for the Eyes group box
  vbox = new QVBoxLayout( pEyesGB, 10 );
  vbox->addSpacing( pEyesGB->fontMetrics().height() );
  vbox->addWidget( pEyesCheckbox );

  // Create a layout for the Pointer group box
  vbox = new QVBoxLayout( pPointerGB, 10 );
  vbox->addSpacing( pPointerGB->fontMetrics().height() );
  vbox->addWidget( pWarpCheckbox );
 
  // Create a layout for the test group box
  hbox = new QHBoxLayout( pTestGB, 10 );
  hbox->addSpacing( pTestGB->fontMetrics().height() );
  hbox->addWidget( pTestButton );
  hbox->addStretch();
  hbox->addWidget( pFindMeLabel );
  hbox->addStretch();
  
  // signal/slot
  connect( pNumberSlider,      SIGNAL(valueChanged(int)), SLOT(slotNumberChanged(int)) );
  connect( pDelaySlider,       SIGNAL(valueChanged(int)), SLOT(slotDelayChanged(int)) );
  connect( pEyesCheckbox,      SIGNAL(toggled(bool)),     SLOT(slotEyesCheckbox(bool)) );
  connect( pTestButton,        SIGNAL(pressed()),         SLOT(slotTest()) );
  connect( pHighlightCheckbox, SIGNAL(toggled(bool)),     SLOT(slotHighlightCheckbox(bool)) );
  connect( pWarpCheckbox,      SIGNAL(toggled(bool)),     SLOT(slotWarpCheckbox(bool)) );

  eyes  = 0L;
  timer = 0L;

  // settings and GUI stuff
  loadSettings();
  updateGUI();
}

KWidgetConfig::~KWidgetConfig()
{
}

QLabel *KWidgetConfig::newLabel( const char *text, QWidget *parent )
{
  QLabel *l = new QLabel( i18n(text), parent, text );
  CHECK_PTR( l );
  l->setMinimumSize( l->sizeHint() );
  return l;
}

QGroupBox *KWidgetConfig::newGroupBox( const char *text )
{
  QGroupBox *b = new QGroupBox( i18n(text), this, text );
  CHECK_PTR( b );
  return b;
}

QCheckBox *KWidgetConfig::newCheckbox( const char *text, QWidget *parent )
{
  QCheckBox *c = new QCheckBox( i18n(text), parent, text );
  CHECK_PTR( c );
  c->setMinimumSize( c->sizeHint() );
  return c;
}

QLCDNumber *KWidgetConfig::newLCD( QWidget *parent )
{
  QLCDNumber *n = new QLCDNumber( 4, parent );
  CHECK_PTR( n );
  n->setSegmentStyle( QLCDNumber::Flat );
  n->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  n->setMaximumHeight( parent->fontMetrics().height() );
  return n;
}

void KWidgetConfig::slotNumberChanged( int nr )
{
  number = nr;
  pNumberLCD->display( number );
}

void KWidgetConfig::slotDelayChanged( int d )
{
  delay = d;
  pDelayLCD->display( delay );
}

void KWidgetConfig::slotEyesCheckbox( bool enable )
{
  bEyesEnabled = enable;
}

void KWidgetConfig::slotHighlightCheckbox( bool enable )
{
  bHighlightEnabled = enable;
  
  setHighlightEnabled( enable );
}

void KWidgetConfig::slotWarpCheckbox( bool enable )
{
  bWarpPointer = enable;
}

#include <kwm.h>

void KWidgetConfig::slotTest()
{
  int eyesWidth = 150, eyesHeight = 100, eyesPad = 20;

  // center of found widget
  QPoint pos = pTestGB->mapToGlobal( pFindMeLabel->pos() );
  QPoint center = QPoint( pos.x() + pFindMeLabel->width()/2, 
			  pos.y() + pFindMeLabel->height()/2 );
  
  if( bEyesEnabled )
    {
      if( eyes )
	delete eyes;

      eyes = new KEyesWidget();
      CHECK_PTR( eyes );
                  
      // placement of eyes: horizontal: align centered with widget
      //                    vertical:   choose side with more space
      QRect geom;
      QRect region = KWM::windowRegion( KWM::currentDesktop() );
      
      if( center.y() - region.top() > region.bottom() - center.y() )
	{
	  // place above
	  geom = QRect( center.x() - eyesWidth/2, pos.y() - eyesHeight - eyesPad,
			eyesWidth, eyesHeight );
	} else {
	  // place below
	  geom = QRect( center.x() - eyesWidth/2, pos.y() + pFindMeLabel->height() + eyesPad,
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
	timer->start( number*delay << 1 );
      }
    }

  if( bHighlightEnabled ) {
    count = number;
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

#include <X11/Xlib.h>

void KWidgetConfig::slotAddHighlight()
{
  QPainter paint( pFindMeLabel );
  paint.setRasterOp( QPainter::XorROP );
  paint.fillRect( 0, 0, 100, 100, QColor(255,255,255) );

  disconnect( timer, SIGNAL(timeout()), this, SLOT(slotAddHighlight()) );
  connect(    timer, SIGNAL(timeout()), this, SLOT(slotRemoveHighlight()) );
  timer->start( delay, TRUE );
}

void KWidgetConfig::slotRemoveHighlight()
{
  QPainter paint( pFindMeLabel );
  paint.setRasterOp( QPainter::XorROP );
  paint.fillRect( 0, 0, 100, 100, QColor(255,255,255) );

  disconnect( timer, SIGNAL(timeout()), this, SLOT(slotRemoveHighlight()) );

  count--;
  if( count > 0 ) {
    connect( timer, SIGNAL(timeout()), this, SLOT(slotAddHighlight()) );
    timer->start( delay, TRUE );
  }
  else {
    if( eyes ) {
      delete eyes;
      eyes = 0L;
    }
    if( timer ) {
      delete timer;
      timer = 0L;
    }
  }
}

void KWidgetConfig::slotRemoveEyes()
{
  disconnect( timer, SIGNAL(timeout()), this, SLOT(slotRemoveEyes()) );
  delete timer;
  timer = 0L;

  if( eyes ) {
    delete eyes;
    eyes = 0L;
  }
}

void KWidgetConfig::updateGUI()
{
  // highlight
  pHighlightCheckbox->setChecked( bHighlightEnabled );
  pNumberLCD->display( number );
  pNumberSlider->setValue( number );
  pDelayLCD->display( delay );
  pDelaySlider->setValue( delay );
  setHighlightEnabled( bHighlightEnabled );
  
  // eyes
  pEyesCheckbox->setChecked( bEyesEnabled );

  // pointer
  pWarpCheckbox->setChecked( bWarpPointer );
}

void KWidgetConfig::setHighlightEnabled( bool e )
{
  pNumberLCD->setEnabled( e );
  pNumberSlider->setEnabled( e );
  pDelayLCD->setEnabled( e );
  pDelaySlider->setEnabled( e );
  
  pNumberLabel->setEnabled( e );
  pDelayLabel->setEnabled( e );
  pTimesLabel->setEnabled( e );
  pMsLabel->setEnabled( e );
}

void KWidgetConfig::loadSettings()
{
  pConfig->setGroup("Widget");

  // highlight
  bHighlightEnabled = pConfig->readBoolEntry( "HighlightEnabled", WIS_DEFAULT_HIGHLIGHT_ENABLED );
  number      = pConfig->readNumEntry(  "HighlightNumber", WIS_DEFAULT_HIGHLIGHT_NUMBER );
  delay       = pConfig->readNumEntry(  "HighlightDelay",  WIS_DEFAULT_HIGHLIGHT_DELAY  );

  // eyes
  bEyesEnabled = pConfig->readBoolEntry( "EyesEnabled",     WIS_DEFAULT_EYES_ENABLED     );

  // pointer
  bWarpPointer = pConfig->readBoolEntry( "WarpPointer",     WIS_DEFAULT_WARP_POINTER     );
}


void KWidgetConfig::saveSettings()
{
  pConfig->setGroup("Widget");

  // highlight
  pConfig->writeEntry( "HighlightEnabled", bHighlightEnabled );
  pConfig->writeEntry( "HighlightNumber", number      );
  pConfig->writeEntry( "HighlightDelay",  delay       );
  
  // eyes
  pConfig->writeEntry( "EyesEnabled",     bEyesEnabled );

  // pointer
  pConfig->writeEntry( "WarpPointer",     bWarpPointer );
  
  pConfig->sync();
}

void KWidgetConfig::applySettings()
{
  saveSettings();
}

void KWidgetConfig::defaultSettings()
{
  bHighlightEnabled = WIS_DEFAULT_HIGHLIGHT_ENABLED;
  number       = WIS_DEFAULT_HIGHLIGHT_NUMBER;
  delay        = WIS_DEFAULT_HIGHLIGHT_DELAY;
  bEyesEnabled = WIS_DEFAULT_EYES_ENABLED;
  bWarpPointer = WIS_DEFAULT_WARP_POINTER;
  
  updateGUI();
}
