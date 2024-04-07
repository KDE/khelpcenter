/*
    SPDX-FileCopyrightText: 2024 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KHC_VIEWCONTAINER_H
#define KHC_VIEWCONTAINER_H

#include <QWidget>
#include <QPointer>

namespace KHC {

class ViewContainer : public QWidget
{
    Q_OBJECT

  public:
    ViewContainer(QWidget *parentWidget);

public:
    void setView(QWidget *view);
    void setBottomBar(QWidget *bottomBar);

    QWidget *view() const;
    QWidget *bottomBar() const;

private Q_SLOTS:
    void hideBottomBar();

private:
  QPointer<QWidget> m_view;
  QPointer<QWidget> m_bottomBar;
};

}

#endif
