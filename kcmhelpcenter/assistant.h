/*
 * This file is part of the KDE Help Center
 *
 * Copyright (c) 1999, René Beutler (rbeutler@g26.ethz.ch)
 *
 */


#ifndef __KASSISTANTCONFIG_H__
#define __KASSISTANTCONFIG_H__

#include <kapp.h>
#include <kcontrol.h>

class KAssistantConfig : public KConfigWidget
{
	Q_OBJECT

public:
	KAssistantConfig( QWidget *parent=0, const char *name=0, bool init=FALSE );
	~KAssistantConfig( );
	void saveParams( void );

public slots:

	void loadSettings();
	void applySettings();
	void defaultSettings();
    
private slots:

private:
	void GetSettings( void );

	KConfig *config;
};

#endif
