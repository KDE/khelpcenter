/*
 * This file is part of the KDE Help Center
 *
 * Copyright (c) 1999, René Beutler (rbeutler@g26.ethz.ch)
 *
 */

#ifndef KWidgetConfig_included
#define KWidgetConfig_included

#include <kconfig.h>
#include <kcontrol.h>
#include <qlabel.h>
#include <qchkbox.h>
#include <qpushbt.h>
#include <qslider.h>
#include <qlcdnum.h>
#include <qgrpbox.h>
#include <qtimer.h>

#include "eyes.h"

extern KConfig *pConfig;


class KWidgetConfig : public KConfigWidget
{
  Q_OBJECT

public:

  KWidgetConfig( QWidget* parent = 0L, const char* name = NULL, bool init=FALSE );
  virtual ~KWidgetConfig();

public slots:

  void loadSettings();
  void saveSettings();
  void applySettings();
  void defaultSettings();
 
protected slots:

  void slotAddHighlight();
  void slotRemoveHighlight();
  void slotRemoveEyes();

  void slotNumberChanged( int );
  void slotDelayChanged( int );
  void slotEyesCheckbox( bool );
  void slotHighlightCheckbox( bool );
  void slotWarpCheckbox( bool );
  void slotTest();

protected:

  QLabel*     newLabel(    const char *text, QWidget *parent );
  QGroupBox*  newGroupBox( const char *text );
  QCheckBox*  newCheckbox( const char *text, QWidget *parent );
  QLCDNumber* newLCD(     QWidget *parent );

  QLabel      *pNumberLabel, *pTimesLabel, *pMsLabel, *pDelayLabel;
  QLabel      *pFindMeLabel;
  QPushButton *pTestButton;
  QLCDNumber  *pNumberLCD, *pDelayLCD;
  QSlider     *pNumberSlider, *pDelaySlider;
  QCheckBox   *pEyesCheckbox, *pHighlightCheckbox, *pWarpCheckbox;
  QGroupBox   *pHighGB, *pEyesGB, *pPointerGB, *pTestGB;

  KEyesWidget *eyes;
private:
  QTimer      *timer;

 void updateGUI();
 void setHighlightEnabled( bool );
 
 int number, delay, count;
 bool bEyesEnabled, bHighlightEnabled, bWarpPointer;
};

#endif
