/*
 * This file is part of the KDE Help Center
 *
 * Copyright (c) 1999, René Beutler (rbeutler@g26.ethz.ch)
 *
 */


#ifndef __KBROWSERCONFIG_H__
#define __KBROWSERCONFIG_H__

#include <kcontrol.h>
#include <kconfig.h>
#include <kcolorbtn.h>

#include <qcheckbox.h>
#include <qcolor.h>


extern KConfig *pConfig;


class KBrowserConfig : public KConfigWidget
{
	Q_OBJECT

public:

	KBrowserConfig( QWidget *parent=0L, const char *name=0L, bool init=FALSE );

public slots:

	void loadSettings();
	void saveSettings();
	void applySettings();
	void defaultSettings();
    
private slots:

	void slotBgColorChanged(    const QColor &col );
	void slotTextColorChanged(  const QColor &col );
	void slotLinkColorChanged(  const QColor &col );
	void slotVLinkColorChanged( const QColor &col );

private:
	void updateGUI();

	KColorButton* m_pBg;
	KColorButton* m_pText;
	KColorButton* m_pLink;
	KColorButton* m_pVLink;
	QCheckBox *cursorbox;
	QCheckBox *underlinebox;
	QCheckBox *forceDefaultsbox;

	QColor bgColor;
	QColor textColor;
	QColor linkColor;
	QColor vLinkColor;
	bool changeCursor;
	bool underlineLinks;
	bool forceDefaults;
};

#endif