/*
    SPDX-FileCopyrightText: 1999 Matthias Elter <me@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "plugintraverser.h"
#include "khc_debug.h"
#include "navigator.h"
#include "navigatorappgroupitem.h"
#include "navigatorappitem.h"
#include "navigatoritem.h"

#include <QIcon>

#include <prefs.h>

using namespace KHC;

PluginTraverser::PluginTraverser(Navigator *navigator, QTreeWidget *parent)
    : DocEntryTraverser()
    , mListView(parent)
    , mParentItem(nullptr)
    , mCurrentItem(nullptr)
    , mNavigator(navigator)
{
}

PluginTraverser::PluginTraverser(Navigator *navigator, NavigatorItem *parent)
    : DocEntryTraverser()
    , mListView(nullptr)
    , mParentItem(parent)
    , mCurrentItem(nullptr)
    , mNavigator(navigator)
{
}

void PluginTraverser::process(DocEntry *entry)
{
    if (!mListView && !mParentItem) {
        qCWarning(KHC_LOG) << "ERROR! Neither mListView nor mParentItem is set.";
        return;
    }

    if (!entry->docExists() && !Prefs::showMissingDocs())
        return;

    if (entry->khelpcenterSpecial().isEmpty()) {
        if (mListView)
            mCurrentItem = new NavigatorAppItem(entry, mListView, mCurrentItem);
        else
            mCurrentItem = new NavigatorAppItem(entry, mParentItem, mCurrentItem);
    } else if (entry->khelpcenterSpecial() == QLatin1String("apps")) {
        NavigatorAppGroupItem *appItem;
        entry->setIcon(QStringLiteral("kde"));
        if (mListView)
            appItem = new NavigatorAppGroupItem(entry, mListView, mCurrentItem);
        else
            appItem = new NavigatorAppGroupItem(entry, mParentItem, mCurrentItem);
        appItem->setRelpath(Prefs::appsRoot());
        mCurrentItem = appItem;
    } else {
        if (mListView)
            mCurrentItem = new NavigatorItem(entry, mListView, mCurrentItem);
        else
            mCurrentItem = new NavigatorItem(entry, mParentItem, mCurrentItem);

        if (entry->khelpcenterSpecial() == QLatin1String("konqueror")) {
            mNavigator->insertParentAppDocs(entry->khelpcenterSpecial(), mCurrentItem);
        } else if (entry->khelpcenterSpecial() == QLatin1String("kcontrol")) {
            mNavigator->insertSystemSettingsDocs(entry->khelpcenterSpecial(), mCurrentItem, Navigator::SystemSettings);
        } else if (entry->khelpcenterSpecial() == QLatin1String("kinfocenter")) {
            mNavigator->insertSystemSettingsDocs(entry->khelpcenterSpecial(), mCurrentItem, Navigator::KInfoCenter);
        } else if (entry->khelpcenterSpecial() == QLatin1String("kioworker")) {
            mNavigator->insertIOWorkerDocs(entry->khelpcenterSpecial(), mCurrentItem);
        } else if (entry->khelpcenterSpecial() == QLatin1String("info")) {
            mNavigator->insertInfoDocs(mCurrentItem);
        } else if (entry->khelpcenterSpecial() == QLatin1String("scrollkeeper")) {
            mNavigator->insertScrollKeeperDocs(mCurrentItem);
        } else {
            return;
        }
        // TODO: was contents2 -> needs to be changed to help-contents-alternate or similar
        mCurrentItem->setIcon(0, QIcon::fromTheme(QStringLiteral("help-contents")));
    }
}

DocEntryTraverser *PluginTraverser::createChild(DocEntry * /*entry*/)
{
    if (mCurrentItem) {
        return new PluginTraverser(mNavigator, mCurrentItem);
    }
    qCWarning(KHC_LOG) << "ERROR! mCurrentItem is not set.";
    return nullptr;
}

// vim:ts=2:sw=2:et
