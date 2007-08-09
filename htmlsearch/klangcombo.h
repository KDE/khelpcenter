/*
 * klangcombo.h - Adds some methods for inserting languages.
 *
 * Copyright (c) 1999-2000 Hans Petter Bieker <bieker@kde.org>
 *
 * Requires the Qt widget libraries, available at no cost at
 * http://www.troll.no/
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


#ifndef __KLANGCOMBO_H__
#define __KLANGCOMBO_H__

#include "ktagcombobox.h"

/*
 * Extends KTagCombo to support adding and changing languages.
 *
 * It has also support for sub menus.
 */
class KLanguageCombo : public KTagComboBox
{
  Q_OBJECT

public:
  explicit KLanguageCombo(QWidget *parent=0, const char *name=0);
  ~KLanguageCombo();

  void insertLanguage(const QString& path, const QString& name, const QString& sub = QString(), const QString &submenu = QString(), int index = -1);
  void changeLanguage(const QString& name, int i);
};

#endif
