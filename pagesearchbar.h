/*
    SPDX-FileCopyrightText: 2017, 2024 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KHC_PAGESEARCHBAR_H
#define KHC_PAGESEARCHBAR_H

#include <QWidget>
#include <memory>

namespace KHC
{

namespace Ui
{
class PageSearchBar;
}
class View;

class PageSearchBar : public QWidget
{
    Q_OBJECT

public:
    explicit PageSearchBar(View *view, QWidget *parent = nullptr);
    ~PageSearchBar() override;

    void hideEvent(QHideEvent *event) override;

public Q_SLOTS:
    void startSearch();
    void searchNext();
    void searchPrevious();

private Q_SLOTS:
    void searchIncrementally();

private:
    std::unique_ptr<Ui::PageSearchBar> const m_ui;
    View *const m_view;
};

}

#endif
