/*
 *  searchwidget.h - part of the KDE Help Center
 *
 *  Copyright (C) 1999 Matthias Elter (me@kde.org)
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
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __khc_searchwidget_h__
#define __khc_searchwidget_h__

#include <qwidget.h>

#include "docmetainfo.h"

class QCheckBox;
class QListView;

class KLanguageCombo;

class ScopeItem;

class SearchWidget : public QWidget
{
    Q_OBJECT
  public:
    SearchWidget ( QWidget *parent = 0 );
    ~SearchWidget();

    QString method();
    int pages();
    QString scope();

    void updateScopeList();

    void registerScopeItem( ScopeItem * );

    QListView *listView() { return mScopeListView; }

  signals:
    void searchResult( const QString &url );
    void enableSearch( bool );

  public slots:
    void slotIndex();
    void slotSwitchBoxes();

  protected:
    void updateScopeItem( ScopeItem *item );

  protected slots:
    void scopeDoubleClicked( QListViewItem * );
    void scopeClicked( QListViewItem * );


  private:
    void loadLanguages();

    QComboBox *mMethodCombo;
    QComboBox *mPagesCombo;
    QFrame *mAdvOptions;
    QListView *mScopeListView;

    int mScopeCount;
};

#endif
