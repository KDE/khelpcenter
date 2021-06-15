/*
    SPDX-FileCopyrightText: 2003 Frerich Raabe <raabe@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KHC_FONTDIALOG_H
#define KHC_FONTDIALOG_H

#include <QDialog>

class QSpinBox;

class KComboBox;
class QFontComboBox;

namespace KHC {

  class FontDialog : public QDialog
  {
    Q_OBJECT
    
    public:
      explicit FontDialog( QWidget *parent = nullptr);

    protected Q_SLOTS:
      virtual void slotOk();

    private:
      void setupFontSizesBox();
      void setupFontTypesBox();
      void setupFontEncodingBox();

      void load();
      void save();

      QSpinBox *m_minFontSize = nullptr;
      QSpinBox *m_medFontSize = nullptr;
      QFontComboBox *m_standardFontCombo = nullptr;
      QFontComboBox *m_fixedFontCombo = nullptr;
      QFontComboBox *m_serifFontCombo = nullptr;
      QFontComboBox *m_sansSerifFontCombo = nullptr;
      QFontComboBox *m_italicFontCombo = nullptr;
      QFontComboBox *m_fantasyFontCombo = nullptr;
      KComboBox *m_defaultEncoding = nullptr;
      QSpinBox *m_fontSizeAdjustement = nullptr;
  };

}

#endif // KHC_FONTDIALOG_H
// vim:ts=4:sw=4:noet
