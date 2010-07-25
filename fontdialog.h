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

#include <KDialog>

class KIntSpinBox;

class KComboBox;
class KFontComboBox;
class KIntNumInput;
class KVBox;

namespace KHC {

  class FontDialog : public KDialog
  {
    Q_OBJECT
    
    public:
      FontDialog( QWidget *parent );

    protected Q_SLOTS:
      virtual void slotOk();

    private:
      void setupFontSizesBox();
      void setupFontTypesBox();
      void setupFontEncodingBox();

      void load();
      void save();

      KIntNumInput *m_minFontSize;
      KIntNumInput *m_medFontSize;
      KFontComboBox *m_standardFontCombo;
      KFontComboBox *m_fixedFontCombo;
      KFontComboBox *m_serifFontCombo;
      KFontComboBox *m_sansSerifFontCombo;
      KFontComboBox *m_italicFontCombo;
      KFontComboBox *m_fantasyFontCombo;
      KComboBox *m_defaultEncoding;
      KIntSpinBox *m_fontSizeAdjustement;
      KVBox *m_mainWidget;
  };

}

#endif // KHC_FONTDIALOG_H
// vim:ts=4:sw=4:noet
