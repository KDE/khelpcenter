/* 
 * This file is part of the KDE Help Center
 *
 * Copyright (c) 1999, René Beutler (rbeutler@g26.ethz.ch)
 *
 */

#include <kcontrol.h>
#include <kconfig.h>
#include <kprocess.h>

#include "assistant.h"
#include "browser.h"
#include "widget.h"
#include <klocale.h>


KConfig *pConfig = 0L;


class KHelpCenterApplication : public KControlApplication
{
public:

	KHelpCenterApplication(int &argc, char **arg, const char *name);

	void apply();
	void defaultValues();

private:

	KAssistantConfig *assistant;
	KBrowserConfig   *browser;
	KWidgetConfig    *widget;
};


KHelpCenterApplication::KHelpCenterApplication(int &argc, char **argv, const char *name)
  : KControlApplication(argc, argv, name)
{
	assistant = 0L;
	browser = 0L;
	widget = 0L;

	if (runGUI())
	{
		if (!pages || pages->contains("assistant"))
		addPage(assistant = new KAssistantConfig(dialog, "assistant", FALSE), 
			i18n("&Assistant"), "assistant-1.html");

		if( !pages || pages->contains("browser"))
		addPage(browser = new KBrowserConfig(dialog, "browser", FALSE),
			i18n("&Browser"), "browser-1.html");

		if (!pages || pages->contains("widget"))
		addPage(widget = new KWidgetConfig(dialog, "widget", FALSE),
			i18n("&Widget Identification"), "widget-1.html");
		
		if( assistant || browser || widget )
			dialog->show();
		else
		{
			fprintf(stderr, i18n("usage: kcmhelpcenter [-init | {assistant, browser, widget}]\n"));
			justInit = TRUE;
		}
	}
}

void KHelpCenterApplication::apply()
{
	if( assistant )
		assistant->applySettings();

	if( browser )
		browser->applySettings();

	if( widget )
		widget->applySettings();

	KProcess proc;
	proc << "kassistantcom";
	proc << "configure";
	proc.start( KProcess::DontCare, KProcess::NoCommunication );
}


void KHelpCenterApplication::defaultValues()
{
	if( assistant )
		assistant->defaultSettings();
	if( browser )
		browser->defaultSettings();
	if( widget )
		widget->defaultSettings();
}


int main(int argc, char **argv)
{
	int ret = 0;

	pConfig = new KConfig( KApplication::kde_configdir()  + "/khelpcenterrc",
	                       KApplication::localconfigdir() + "/khelpcenterrc" );

	KHelpCenterApplication app(argc, argv, "kcmhelpcenter");
	app.setTitle( i18n("HelpCenter Configuration") );

	if (app.runGUI())
		ret = app.exec();
	else {
	   	KProcess proc;
		proc << "kassistantcom";
		proc << "configure";
		proc.start( KProcess::DontCare, KProcess::NoCommunication );
		return 0;
	}

	delete( pConfig );
	return ret;
}
