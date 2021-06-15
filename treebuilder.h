/*
    SPDX-FileCopyrightText: 2002 Frerich Raabe <raabe@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KHC_TREEBUILDER_H
#define KHC_TREEBUILDER_H

#include <QObject>
#include <QUrl>

namespace KHC
{

class NavigatorItem;
class TreeBuilder : public QObject
{
	Q_OBJECT
	public:
		explicit TreeBuilder( QObject *parent );

		virtual void build( NavigatorItem *item ) = 0;

  public Q_SLOTS:
    virtual void selectURL( const QString &url );

  Q_SIGNALS:
    void urlSelected( const QUrl &url );
};

}

#endif // TREEBUILDER_H
// vim:ts=2:sw=2:et
