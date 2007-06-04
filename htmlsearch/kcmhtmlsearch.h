/**
 * kcmhtmlsearch.h
 *
 * Copyright (c) 2000 Matthias Hölzer-Klüpfel <hoelzer@kde.org>
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

#ifndef __kcmhtmlsearch_h__
#define __kcmhtmlsearch_h__


#include <kcmodule.h>


class QCheckBox;
class QPushButton;
class KListWidget;
class K3Process;
class KLanguageCombo;
class KUrlRequester;
class QStringList;

class KHTMLSearchConfig : public KCModule
{
  Q_OBJECT

public:

  KHTMLSearchConfig(QWidget *parent, const QStringList &args);
  virtual ~KHTMLSearchConfig();
  
  void load();
  void save();
  void defaults();

  QString quickHelp() const;
  
  int buttons();

  
protected Q_SLOTS:

  void configChanged();
  void addClicked(); 
  void delClicked();
  void pathSelected(const QString &);
  void urlClicked(const QString&);
  void generateIndex();

  void indexTerminated(K3Process *proc);

      
private:

  void checkButtons();
  void loadLanguages();

  KUrlRequester *htdigBin, *htsearchBin, *htmergeBin;
  QCheckBox *indexKDE, *indexMan, *indexInfo;
  QPushButton *addButton, *delButton, *runButton;
  KListWidget *searchPaths;
  KLanguageCombo *language;

  K3Process *indexProc;

};

#endif
