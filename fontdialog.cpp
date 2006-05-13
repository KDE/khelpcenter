/*
 *  This file is part of the KDE Help Center
 *
 *  Copyright (C) 2003 Frerich Raabe <raabe@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include "fontdialog.h"

#include <kapplication.h>
#include <kcharsets.h>
#include <kcombobox.h>
#include <kconfig.h>
#include <kfontcombo.h>
#include <khtmldefaults.h>
#include <klocale.h>
#include <knuminput.h>

#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QSpinBox>
//Added by qt3to4:
#include <QGridLayout>
#include <kglobal.h>

using namespace KHC;

FontDialog::FontDialog( QWidget *parent, const char *name )
	: KDialogBase( Swallow, 0, parent, name, true, i18n( "Font Configuration" ),
	               Ok | Cancel )
{
	makeVBoxMainWidget();

	setupFontSizesBox();
	setupFontTypesBox();
	setupFontEncodingBox();

	load();
}

void FontDialog::slotOk()
{
	save();
	accept();
}

void FontDialog::setupFontSizesBox()
{
	QGroupBox *gb = new QGroupBox( i18n( "Sizes" ), mainWidget() );

	QGridLayout *layout = new QGridLayout( gb );
	layout->setSpacing( KDialog::spacingHint() );
	layout->setMargin( KDialog::marginHint() * 2 );

	QLabel *lMinFontSize = new QLabel( i18n( "M&inimum font size:" ), gb );
	layout->addWidget( lMinFontSize, 0, 0 );
	m_minFontSize = new KIntNumInput( gb );
	layout->addWidget( m_minFontSize, 0, 1 );
	m_minFontSize->setRange( 1, 20 );
	lMinFontSize->setBuddy( m_minFontSize );

	QLabel *lMedFontSize = new QLabel( i18n( "M&edium font size:" ), gb );
	layout->addWidget( lMedFontSize, 1, 0 );
	m_medFontSize = new KIntNumInput( gb );
	layout->addWidget( m_medFontSize, 1, 1 );
	m_medFontSize->setRange( 4, 28 );
	lMedFontSize->setBuddy( m_medFontSize );
}

void FontDialog::setupFontTypesBox()
{
	QGroupBox *gb = new QGroupBox( i18n( "Fonts" ), mainWidget() );

	QGridLayout *layout = new QGridLayout( gb );
	layout->setSpacing( KDialog::spacingHint() );
	layout->setMargin( KDialog::marginHint() * 2 );

	QLabel *lStandardFont = new QLabel( i18n( "S&tandard font:" ), gb );
	layout->addWidget( lStandardFont, 0, 0 );
	m_standardFontCombo = new KFontCombo( gb );
	layout->addWidget( m_standardFontCombo, 0, 1 );
	lStandardFont->setBuddy( m_standardFontCombo );

	QLabel *lFixedFont = new QLabel( i18n( "F&ixed font:" ), gb );
	layout->addWidget( lFixedFont, 1, 0 );
	m_fixedFontCombo = new KFontCombo( gb );
	layout->addWidget( m_fixedFontCombo, 1, 1 );
	lFixedFont->setBuddy( m_fixedFontCombo );

	QLabel *lSerifFont = new QLabel( i18n( "S&erif font:" ), gb );
	layout->addWidget( lSerifFont, 2, 0 );
	m_serifFontCombo = new KFontCombo( gb );
	layout->addWidget( m_serifFontCombo, 2, 1 );
	lSerifFont->setBuddy( m_serifFontCombo );

	QLabel *lSansSerifFont = new QLabel( i18n( "S&ans serif font:" ), gb );
	layout->addWidget( lSansSerifFont, 3, 0 );
	m_sansSerifFontCombo = new KFontCombo( gb );
	layout->addWidget( m_sansSerifFontCombo, 3, 1 );
	lSansSerifFont->setBuddy( m_sansSerifFontCombo );

	QLabel *lItalicFont = new QLabel( i18n( "&Italic font:" ), gb );
	layout->addWidget( lItalicFont, 4, 0 );
	m_italicFontCombo = new KFontCombo( gb );
	layout->addWidget( m_italicFontCombo, 4, 1 );
	lItalicFont->setBuddy( m_italicFontCombo );

	QLabel *lFantasyFont = new QLabel( i18n( "&Fantasy font:" ), gb );
	layout->addWidget( lFantasyFont, 5, 0 );
	m_fantasyFontCombo = new KFontCombo( gb );
	layout->addWidget( m_fantasyFontCombo, 5, 1 );
	lFantasyFont->setBuddy( m_fantasyFontCombo );
}

void FontDialog::setupFontEncodingBox()
{
	QGroupBox *gb = new QGroupBox( i18n( "Encoding" ), mainWidget() );

	QGridLayout *layout = new QGridLayout( gb );
	layout->setSpacing( KDialog::spacingHint() );
	layout->setMargin( KDialog::marginHint() * 2 );

	QLabel *lDefaultEncoding = new QLabel( i18n( "&Default encoding:" ), gb );
	layout->addWidget( lDefaultEncoding, 0, 0 );
	m_defaultEncoding = new KComboBox( false, gb );
	layout->addWidget( m_defaultEncoding, 0, 1 );
	QStringList encodings = KGlobal::charsets()->availableEncodingNames();
	encodings.prepend( i18n( "Use Language Encoding" ) );
	m_defaultEncoding->addItems( encodings );
	lDefaultEncoding->setBuddy( m_defaultEncoding );

	QLabel *lFontSizeAdjustement = new QLabel( i18n( "&Font size adjustment:" ), gb );
	layout->addWidget( lFontSizeAdjustement, 1, 0 );
	m_fontSizeAdjustement = new QSpinBox( gb );
	m_fontSizeAdjustement->setRange( -5, 5 );
	m_fontSizeAdjustement->setSingleStep( 1 );
	layout->addWidget( m_fontSizeAdjustement, 1, 1 );
	lFontSizeAdjustement->setBuddy( m_fontSizeAdjustement );
}

void FontDialog::load()
{
	KConfig *cfg = KGlobal::config();
	{
		KConfigGroup configGroup( cfg, "HTML Settings" );

		m_minFontSize->setValue( configGroup.readEntry( "MinimumFontSize", (int)HTML_DEFAULT_MIN_FONT_SIZE ) );
		m_medFontSize->setValue( configGroup.readEntry( "MediumFontSize", 10 ) );

		QStringList fonts = configGroup.readEntry( "Fonts" , QStringList() );
		if ( fonts.isEmpty() )
			fonts << KGlobalSettings::generalFont().family()
			      << KGlobalSettings::fixedFont().family()
			      << HTML_DEFAULT_VIEW_SERIF_FONT
			      << HTML_DEFAULT_VIEW_SANSSERIF_FONT
			      << HTML_DEFAULT_VIEW_CURSIVE_FONT
			      << HTML_DEFAULT_VIEW_FANTASY_FONT
                              << QString();

		m_standardFontCombo->setCurrentFont( fonts[ 0 ] );
		m_fixedFontCombo->setCurrentFont( fonts[ 1 ] );
		m_serifFontCombo->setCurrentFont( fonts[ 2 ] );
		m_sansSerifFontCombo->setCurrentFont( fonts[ 3 ] );
		m_italicFontCombo->setCurrentFont( fonts[ 4 ] );
		m_fantasyFontCombo->setCurrentFont( fonts[ 5 ] );

		m_defaultEncoding->setCurrentItem( configGroup.readEntry( "DefaultEncoding" ) );
		m_fontSizeAdjustement->setValue( fonts[ 6 ].toInt() );
	}
}

void FontDialog::save()
{
	KConfig *cfg = KGlobal::config();
	{
		KConfigGroup configGroup( cfg, "General" );
		configGroup.writeEntry( "UseKonqSettings", false );
	}
	{
		KConfigGroup configGroup( cfg, "HTML Settings" );

		configGroup.writeEntry( "MinimumFontSize", m_minFontSize->value() );
		configGroup.writeEntry( "MediumFontSize", m_medFontSize->value() );

		QStringList fonts;
		fonts << m_standardFontCombo->currentText()
		      << m_fixedFontCombo->currentText()
		      << m_serifFontCombo->currentText()
		      << m_sansSerifFontCombo->currentText()
		      << m_italicFontCombo->currentText()
		      << m_fantasyFontCombo->currentText()
		      << QString::number( m_fontSizeAdjustement->value() );

		configGroup.writeEntry( "Fonts", fonts );

		if ( m_defaultEncoding->currentText() == i18n( "Use Language Encoding" ) )
			configGroup.writeEntry( "DefaultEncoding", QString() );
		else
			configGroup.writeEntry( "DefaultEncoding", m_defaultEncoding->currentText() );
	}
	cfg->sync();
}

#include "fontdialog.moc"
// vim:ts=4:sw=4:noet
