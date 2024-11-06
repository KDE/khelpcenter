/*
    SPDX-FileCopyrightText: 2024 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "viewcontainer.h"

#include <QShortcut>
#include <QVBoxLayout>

namespace KHC
{

ViewContainer::ViewContainer(QWidget *parentWidget)
    : QWidget(parentWidget)
{
    auto *l = new QVBoxLayout(this);
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(0);

    auto *closeBottomBarShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    closeBottomBarShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    connect(closeBottomBarShortcut, &QShortcut::activated, this, &ViewContainer::hideBottomBar);
}

void ViewContainer::setView(QWidget *view)
{
    auto *l = static_cast<QVBoxLayout *>(layout());
    if (m_view) {
        l->removeWidget(m_view.data());
    }
    m_view = view;
    if (m_view) {
        // insert at start, stretching
        l->insertWidget(0, m_view.data(), 1);
    }
}

void ViewContainer::setBottomBar(QWidget *bottomBar)
{
    auto *l = static_cast<QVBoxLayout *>(layout());
    if (m_bottomBar) {
        l->removeWidget(m_bottomBar.data());
    }
    m_bottomBar = bottomBar;
    if (m_bottomBar) {
        // append without stretch facror
        l->addWidget(m_bottomBar.data());
    }
}

void ViewContainer::hideBottomBar()
{
    if (m_bottomBar) {
        m_bottomBar->hide();
    }
}

QWidget *ViewContainer::view() const
{
    return m_view.data();
}

QWidget *ViewContainer::bottomBar() const
{
    return m_bottomBar.data();
}

}

#include "moc_viewcontainer.cpp"
