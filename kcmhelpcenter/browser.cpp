/* 
 * This file is part of the KDE Help Center
 *
 * Copyright (c) 1999, René Beutler (rbeutler@g26.ethz.ch)
 *
 */

#include <qlabel.h>
#include <qgroupbox.h>
#include <qlayout.h>

#include <kseparator.h>

#include "browser.h"
#include "../kassistant/config-kassistant.h"  // include default values directly from kassistant


KBrowserConfig::KBrowserConfig (QWidget * parent, const char *name, bool init)
    : KConfigWidget (parent, name)
{
	// set up GUI and connect signals to slots
	QLabel *label;

	//CT 12Nov1998 layout management
	QGridLayout *lay = new QGridLayout(this,12,5,10,5);
	lay->addRowSpacing(0,10);
	lay->addRowSpacing(1,30);
	lay->addRowSpacing(2, 5);
	lay->addRowSpacing(3,30);
	lay->addRowSpacing(4, 5);
	lay->addRowSpacing(5,30);
	lay->addRowSpacing(6, 5);
	lay->addRowSpacing(7,30);
	lay->addRowSpacing(11,10);
	lay->addColSpacing(0,10);
	lay->addColSpacing(2,20);
	lay->addColSpacing(3,80);
	lay->addColSpacing(4,10);

	lay->setRowStretch(0,0);
	lay->setRowStretch(1,0);
	lay->setRowStretch(2,1);
	lay->setRowStretch(3,0);
	lay->setRowStretch(4,1);
	lay->setRowStretch(5,0);
	lay->setRowStretch(6,1);
	lay->setRowStretch(7,0);
	lay->setRowStretch(8,1);
	lay->setRowStretch(9,1);
	lay->setRowStretch(10,1);
	lay->setRowStretch(11,0);

	lay->setColStretch(0,0);
	lay->setColStretch(1,0);
	lay->setColStretch(2,1);
	lay->setColStretch(3,0);
	lay->setColStretch(4,1);
	//CT

	label = new QLabel( i18n("Background Color:"), this );
	label->adjustSize();
	label->setMinimumSize(label->size());
	lay->addWidget(label,1,1);

	m_pBg = new KColorButton( bgColor, this );
	m_pBg->adjustSize();
	m_pBg->setMinimumSize(m_pBg->size());
	lay->addWidget(m_pBg,1,3);
	connect( m_pBg, SIGNAL( changed( const QColor & ) ),
	         SLOT( slotBgColorChanged( const QColor & ) ) );

	label = new QLabel( i18n("Normal Text Color:"), this );
	label->adjustSize();
	label->setMinimumSize(label->size());
	lay->addWidget(label,3,1);

	m_pText = new KColorButton( textColor, this );
	m_pText->adjustSize();
	m_pText->setMinimumSize(m_pText->size());
	lay->addWidget(m_pText,3,3);
	connect( m_pText, SIGNAL( changed( const QColor & ) ),
	         SLOT( slotTextColorChanged( const QColor & ) ) );

	label = new QLabel( i18n("URL Link Color:"), this );
	label->adjustSize();
	label->setMinimumSize(label->size());
	lay->addWidget(label,5,1);

	m_pLink = new KColorButton( linkColor, this );
	m_pLink->adjustSize();
	m_pLink->setMinimumSize(m_pLink->size());
	lay->addWidget(m_pLink,5,3);
	connect( m_pLink, SIGNAL( changed( const QColor & ) ),
	         SLOT( slotLinkColorChanged( const QColor & ) ) );

/*
	label = new QLabel( i18n("Followed Link Color:"), this );
	label->adjustSize();
	label->setMinimumSize(label->size());
	lay->addWidget(label,7,1);

	m_pVLink = new KColorButton( vLinkColor, this );
	m_pVLink->adjustSize();
	m_pVLink->setMinimumSize(m_pVLink->size());
	lay->addWidget(m_pVLink,7,3);
	connect( m_pVLink, SIGNAL( changed( const QColor & ) ),
        	 SLOT( slotVLinkColorChanged( const QColor & ) ) );
*/

	cursorbox = new QCheckBox(i18n("Change cursor over link."), this);
	cursorbox->adjustSize();
	cursorbox->setMinimumSize(cursorbox->size());
	lay->addMultiCellWidget(cursorbox,8,8,1,3);

	underlinebox = new QCheckBox(i18n("Underline links"), this);

	underlinebox->adjustSize();
	underlinebox->setMinimumSize(underlinebox->size());
	lay->addMultiCellWidget(underlinebox,9,9,1,3);

	forceDefaultsbox = new QCheckBox(i18n("Always use my colors"),this);

	forceDefaultsbox->adjustSize();
	forceDefaultsbox->setMinimumSize(forceDefaultsbox->size());
	lay->addMultiCellWidget(forceDefaultsbox,10,10,1,3);

	loadSettings();

	setMinimumSize( 400, 180 );
}

void KBrowserConfig::slotBgColorChanged( const QColor &col )
{
	if ( bgColor != col )
		bgColor = col;
}

void KBrowserConfig::slotTextColorChanged( const QColor &col )
{
	if ( textColor != col )
		textColor = col;
}

void KBrowserConfig::slotLinkColorChanged( const QColor &col )
{
	if ( linkColor != col )
		linkColor = col;
}

void KBrowserConfig::slotVLinkColorChanged( const QColor &col )
{
	if ( vLinkColor != col )
		vLinkColor = col;
}

void KBrowserConfig::updateGUI()
{
	m_pBg->setColor( bgColor );
	m_pText->setColor( textColor );
	m_pLink->setColor( linkColor );
//	m_pVLink->setColor( vLinkColor );
	cursorbox->setChecked( changeCursor );
	underlinebox->setChecked( underlineLinks );
	forceDefaultsbox->setChecked( forceDefaults );
}

void KBrowserConfig::loadSettings()
{
	pConfig->setGroup("Browser");
	bgColor        =  pConfig->readColorEntry( "BgColor",           &HTML_DEFAULT_BG_COLOR );
	textColor      =  pConfig->readColorEntry( "TextColor",         &HTML_DEFAULT_TXT_COLOR );
	linkColor      =  pConfig->readColorEntry( "LinkColor",         &HTML_DEFAULT_LNK_COLOR );
	vLinkColor     =  pConfig->readColorEntry( "VLinkColor",        &HTML_DEFAULT_VLNK_COLOR);
	changeCursor   =  pConfig->readBoolEntry(  "ChangeCursor",       HTML_DEFAULT_CHANGE_CURSOR);
	underlineLinks =  pConfig->readBoolEntry(  "UnderlineLinks",     HTML_DEFAULT_UNDERLINE_LINKS);
	forceDefaults  =  pConfig->readBoolEntry(  "ForceDefaultColors", HTML_DEFAULT_FORCE_DFLT_COLORS);

	updateGUI();
}

void KBrowserConfig::saveSettings()
{
       	changeCursor   = cursorbox->isChecked();
	underlineLinks = underlinebox->isChecked();
	forceDefaults  = forceDefaultsbox->isChecked();

	pConfig->setGroup("Browser");
	pConfig->writeEntry( "BgColor", bgColor );
	pConfig->writeEntry( "TextColor", textColor );
	pConfig->writeEntry( "LinkColor", linkColor);
	pConfig->writeEntry( "VLinkColor", vLinkColor );
	pConfig->writeEntry( "ChangeCursor", changeCursor );
	pConfig->writeEntry( "UnderlineLinks", underlineLinks );
	pConfig->writeEntry( "ForceDefaultColors", forceDefaults );
	pConfig->sync();
}

void KBrowserConfig::applySettings()
{
	saveSettings();
}

void KBrowserConfig::defaultSettings()
{
	bgColor    = HTML_DEFAULT_BG_COLOR;
	textColor  = HTML_DEFAULT_TXT_COLOR;
	linkColor  = HTML_DEFAULT_LNK_COLOR;
	vLinkColor = HTML_DEFAULT_VLNK_COLOR;

	changeCursor   = HTML_DEFAULT_CHANGE_CURSOR;
	underlineLinks = HTML_DEFAULT_UNDERLINE_LINKS;
	forceDefaults  = HTML_DEFAULT_FORCE_DFLT_COLORS;

	updateGUI();
}

