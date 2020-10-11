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
