/*
    SPDX-FileCopyrightText: 1999 Matthias Elter <me@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KHC_SEARCHWIDGET_H
#define KHC_SEARCHWIDGET_H

#include <QWidget>

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

  public:
    explicit SearchWidget ( SearchEngine *, QWidget *parent = nullptr );
    ~SearchWidget();

    QString method() const;
    int pages() const;
    QStringList scope() const;

    enum { ScopeDefault, ScopeAll, ScopeNone, ScopeCustom, ScopeNum };

    void readConfig( KConfig * );
    void writeConfig( KConfig * );

    int scopeCount() const;

    void searchIndexUpdated();

  Q_SIGNALS:
    void searchResult( const QString &url );
    void scopeCountChanged( int );

  public Q_SLOTS:
    void slotSwitchBoxes();
    void scopeSelectionChanged( int );
    void updateScopeList();

  protected:
    void checkScope();
    static QString scopeSelectionLabel( int );

  protected Q_SLOTS:
    void scopeDoubleClicked( QTreeWidgetItem* );
    void scopeClicked( QTreeWidgetItem* );

  private:
    SearchEngine *const mEngine;

    QComboBox *mMethodCombo = nullptr;
    QComboBox *mPagesCombo = nullptr;
    QComboBox *mScopeCombo = nullptr;
    QTreeWidget *mScopeListView = nullptr;

    int mScopeCount = 0;
};

}

#endif //KHC_SEARCHWIDGET_H
// vim:ts=2:sw=2:et
