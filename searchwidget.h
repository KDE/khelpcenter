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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef KHC_SEARCHWIDGET_H
#define KHC_SEARCHWIDGET_H

#include <QWidget>
#include <QtDBus/QtDBus>

#include "docmetainfo.h"

class QTreeWidgetItem;
class QTreeWidget;
class QTreeWidgetItem;
class QComboBox;

class KConfig;


namespace KHC {

class SearchEngine;

class SearchWidget : public QWidget
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.khelpcenter.searchwidget")

public Q_SLOTS:
    Q_SCRIPTABLE Q_NOREPLY void searchIndexUpdated(); // called from kcmhelpcenter

  public:
    explicit SearchWidget ( SearchEngine *, QWidget *parent = 0 );
    ~SearchWidget();

    QString method();
    int pages();
    QString scope();

    QTreeWidget *listView() { return mScopeListView; }

    enum { ScopeDefault, ScopeAll, ScopeNone, ScopeCustom, ScopeNum };

    QString scopeSelectionLabel( int ) const;

    void readConfig( KConfig * );
    void writeConfig( KConfig * );

    int scopeCount() const;

    SearchEngine *engine() const { return mEngine; }

  Q_SIGNALS:
    void searchResult( const QString &url );
    void scopeCountChanged( int );
    void showIndexDialog();

  public Q_SLOTS:
    void slotSwitchBoxes();
    void scopeSelectionChanged( int );
    void updateScopeList();

  protected:
    void checkScope();

  protected Q_SLOTS:
    void scopeDoubleClicked( QTreeWidgetItem* );
    void scopeClicked( QTreeWidgetItem* );

  private:
    SearchEngine *mEngine;

    QComboBox *mMethodCombo;
    QComboBox *mPagesCombo;
    QComboBox *mScopeCombo;
    QTreeWidget *mScopeListView;

    int mScopeCount;
};

}

#endif //KHC_SEARCHWIDGET_H
// vim:ts=2:sw=2:et
