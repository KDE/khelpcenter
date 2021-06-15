/*
    SPDX-FileCopyrightText: 2003 Frerich Raabe <raabe@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "fontdialog.h"

#include <KCharsets>
#include <KComboBox>
#include <KConfig>
#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>

#include <khtmldefaults.h>

#include <QDialogButtonBox>
#include <QFontComboBox>
#include <QFontDatabase>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

#include <prefs.h>

using namespace KHC;

FontDialog::FontDialog( QWidget *parent )
	: QDialog( parent )
{
  setModal( true );
  setWindowTitle( i18n( "Font Configuration" ) );
  QVBoxLayout *mainLayout = new QVBoxLayout(this);

  setupFontSizesBox();
  setupFontTypesBox();
  setupFontEncodingBox();

  QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
  QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
  okButton->setDefault(true);
  okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
  connect(buttonBox, &QDialogButtonBox::accepted, this, &FontDialog::slotOk);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &FontDialog::reject);
  mainLayout->addWidget(buttonBox);

  load();
}

void FontDialog::slotOk()
{
  save();
  accept();
}

void FontDialog::setupFontSizesBox()
{
  QGroupBox *gb = new QGroupBox( i18n( "Sizes" ), this );
  layout()->addWidget( gb );

  QGridLayout *layout = new QGridLayout( gb );
  QLabel *lMinFontSize = new QLabel( i18nc( "The smallest size a will have", "M&inimum font size:" ), gb );
  layout->addWidget( lMinFontSize, 0, 0 );
  m_minFontSize = new QSpinBox( gb );
  layout->addWidget( m_minFontSize, 0, 1 );
  m_minFontSize->setRange( 1, 20 );
  lMinFontSize->setBuddy( m_minFontSize );

  QLabel *lMedFontSize = new QLabel( i18nc( "The normal size a font will have", "M&edium font size:" ), gb );
  layout->addWidget( lMedFontSize, 1, 0 );
  m_medFontSize = new QSpinBox( gb );
  layout->addWidget( m_medFontSize, 1, 1 );
  m_medFontSize->setRange( 4, 28 );
  lMedFontSize->setBuddy( m_medFontSize );
}

void FontDialog::setupFontTypesBox()
{
  QGroupBox *gb = new QGroupBox( i18n( "Fonts" ), this );
  layout()->addWidget( gb );

  QGridLayout *layout = new QGridLayout( gb );
  QLabel *lStandardFont = new QLabel( i18n( "S&tandard font:" ), gb );
  layout->addWidget( lStandardFont, 0, 0 );
  m_standardFontCombo = new QFontComboBox( gb );
  layout->addWidget( m_standardFontCombo, 0, 1 );
  lStandardFont->setBuddy( m_standardFontCombo );

  QLabel *lFixedFont = new QLabel( i18n( "F&ixed font:" ), gb );
  layout->addWidget( lFixedFont, 1, 0 );
  m_fixedFontCombo = new QFontComboBox( gb );
  layout->addWidget( m_fixedFontCombo, 1, 1 );
  lFixedFont->setBuddy( m_fixedFontCombo );

  QLabel *lSerifFont = new QLabel( i18n( "S&erif font:" ), gb );
  layout->addWidget( lSerifFont, 2, 0 );
  m_serifFontCombo = new QFontComboBox( gb );
  layout->addWidget( m_serifFontCombo, 2, 1 );
  lSerifFont->setBuddy( m_serifFontCombo );

  QLabel *lSansSerifFont = new QLabel( i18n( "S&ans serif font:" ), gb );
  layout->addWidget( lSansSerifFont, 3, 0 );
  m_sansSerifFontCombo = new QFontComboBox( gb );
  layout->addWidget( m_sansSerifFontCombo, 3, 1 );
  lSansSerifFont->setBuddy( m_sansSerifFontCombo );

  QLabel *lItalicFont = new QLabel( i18n( "&Italic font:" ), gb );
  layout->addWidget( lItalicFont, 4, 0 );
  m_italicFontCombo = new QFontComboBox( gb );
  layout->addWidget( m_italicFontCombo, 4, 1 );
  lItalicFont->setBuddy( m_italicFontCombo );

  QLabel *lFantasyFont = new QLabel( i18n( "&Fantasy font:" ), gb );
  layout->addWidget( lFantasyFont, 5, 0 );
  m_fantasyFontCombo = new QFontComboBox( gb );
  layout->addWidget( m_fantasyFontCombo, 5, 1 );
  lFantasyFont->setBuddy( m_fantasyFontCombo );
}

void FontDialog::setupFontEncodingBox()
{
  QGroupBox *gb = new QGroupBox( i18n( "Encoding" ), this );
  layout()->addWidget( gb );

  QGridLayout *layout = new QGridLayout( gb );

  QLabel *lDefaultEncoding = new QLabel( i18n( "&Default encoding:" ), gb );
  layout->addWidget( lDefaultEncoding, 0, 0 );
  m_defaultEncoding = new KComboBox( false, gb );
  layout->addWidget( m_defaultEncoding, 0, 1 );
  QStringList encodings = KCharsets::charsets()->availableEncodingNames();
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
  KSharedConfig::Ptr cfg = KSharedConfig::openConfig();
  {
    KConfigGroup configGroup( cfg, "HTML Settings" );

    m_minFontSize->setValue( configGroup.readEntry( "MinimumFontSize", static_cast<int>(HTML_DEFAULT_MIN_FONT_SIZE) ) );
    m_medFontSize->setValue( configGroup.readEntry( "MediumFontSize", 10 ) );

    QStringList fonts = configGroup.readEntry( "Fonts" , QStringList() );
      if ( fonts.isEmpty() ) {
          fonts << QFontDatabase::systemFont(QFontDatabase::GeneralFont).family()
          << QFontDatabase::systemFont(QFontDatabase::FixedFont).family()
		  << QStringLiteral(HTML_DEFAULT_VIEW_SERIF_FONT)
		  << QStringLiteral(HTML_DEFAULT_VIEW_SANSSERIF_FONT)
		  << QStringLiteral(HTML_DEFAULT_VIEW_CURSIVE_FONT)
		  << QStringLiteral(HTML_DEFAULT_VIEW_FANTASY_FONT)
		  << QString();
      }

    m_standardFontCombo->setCurrentFont( fonts[ 0 ] );
    m_fixedFontCombo->setCurrentFont( fonts[ 1 ] );
    m_serifFontCombo->setCurrentFont( fonts[ 2 ] );
    m_sansSerifFontCombo->setCurrentFont( fonts[ 3 ] );
    m_italicFontCombo->setCurrentFont( fonts[ 4 ] );
    m_fantasyFontCombo->setCurrentFont( fonts[ 5 ] );

    QString encoding = configGroup.readEntry( "DefaultEncoding" );
    if (encoding.isEmpty()) m_defaultEncoding->setCurrentIndex( 0 );
    else m_defaultEncoding->setCurrentItem( encoding );
    m_fontSizeAdjustement->setValue( fonts[ 6 ].toInt() );
  }
}

void FontDialog::save()
{
  KSharedConfig::Ptr cfg = KSharedConfig::openConfig();
  Prefs::setUseKonqSettings( false );
  {
    KConfigGroup configGroup( cfg, "HTML Settings" );

    configGroup.writeEntry( "MinimumFontSize", m_minFontSize->value() );
    configGroup.writeEntry( "MediumFontSize", m_medFontSize->value() );

    QStringList fonts;
    fonts << m_standardFontCombo->currentFont().family()
	  << m_fixedFontCombo->currentFont().family()
	  << m_serifFontCombo->currentFont().family()
	  << m_sansSerifFontCombo->currentFont().family()
	  << m_italicFontCombo->currentFont().family()
	  << m_fantasyFontCombo->currentFont().family()
	  << QString::number( m_fontSizeAdjustement->value() );

    configGroup.writeEntry( "Fonts", fonts );

    if ( m_defaultEncoding->currentText() == i18n( "Use Language Encoding" ) )
	    configGroup.writeEntry( "DefaultEncoding", QString() );
    else
	    configGroup.writeEntry( "DefaultEncoding", m_defaultEncoding->currentText() );
  }
  cfg->sync();
}

// vim:ts=4:sw=4:noet
