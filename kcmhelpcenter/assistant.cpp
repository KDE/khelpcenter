/* 
 * This file is part of the KDE Help Center
 *
 * Copyright (c) 1999, René Beutler (rbeutler@g26.ethz.ch)
 *
*/

#include "assistant.h"

static bool GUI;

KAssistantConfig::~KAssistantConfig ()
{
  if (GUI)
    {
    }
}

KAssistantConfig::KAssistantConfig (QWidget * parent, const char *name, bool init)
    : KConfigWidget (parent, name)
{
	if (init)
		GUI = FALSE;
	else
		GUI = TRUE;

	if (GUI)
	{
		// set up GUI and connect signals to slots
	}

	config = kapp->getConfig();

	GetSettings();

	if (init)
		saveParams();
}

void KAssistantConfig::GetSettings( void )
{
	config->setGroup("Assistant");

	// the GUI should reflect the real values
	if (GUI)
	{
	}
}

void KAssistantConfig::saveParams( void )
{
	if (GUI)
	{
	}

	config->sync();
}

void KAssistantConfig::loadSettings()
{
	GetSettings();
}

void KAssistantConfig::applySettings()
{
	saveParams();
}

void KAssistantConfig::defaultSettings()
{
}

