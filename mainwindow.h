/*
    SPDX-FileCopyrightText: 1999 Matthias Elter <me@kde.org>
    SPDX-FileCopyrightText: 2001 Stephan Kulow <coolo@kde.org>
    SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KHC_MAINWINDOW_H
#define KHC_MAINWINDOW_H

#include <KBookmarkManager>
#include <KXmlGuiWindow>

#include "glossary.h"

class QSplitter;

class LogDialog;
class KJob;

namespace KIO
{

class Job;

}

namespace KHC
{

class Navigator;
class View;
class ViewContainer;
class PageSearchBar;

class MainWindow : public KXmlGuiWindow
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.khelpcenter.khelpcenter")
public:
    MainWindow();
    ~MainWindow() override;

public Q_SLOTS:
    Q_SCRIPTABLE void openUrl(const QString &url);
    Q_SCRIPTABLE void openUrl(const QString &url, const QByteArray &startup_id);
    Q_SCRIPTABLE void showHome();
    Q_SCRIPTABLE void lastSearch();

public Q_SLOTS:
    void statusBarRichTextMessage(const QString &m);
    void statusBarMessage(const QString &m);
    void slotShowHome();
    void slotLastSearch();
    void slotFullScreen();
    void showSearchStderr();
    /**
      Show document corresponding to given URL in viewer part.
    */
    void viewUrl(const QString &);

    /**
      Open document corresponding to given URL, i.e. show it in the viewer part
      and select the corresponding entry in the navigator widget.
    */
    void openUrl(const QUrl &url);

protected:
    void setupActions();
    void setupBookmarks();
    /**
      Show document corresponding to given URL in viewer part.
    */
    void viewUrl(const QUrl &url);

    void saveProperties(KConfigGroup &config) override;
    void readProperties(const KConfigGroup &config) override;

    void readConfig();
    void writeConfig();

protected Q_SLOTS:
    void enableLastSearchAction();
    void enableCopyTextAction();

private:
    void stop();

private Q_SLOTS:
    void slotPrintPreview();
    void slotPrint();
    void slotGlossSelected(const GlossaryEntry &entry);
    void slotStarted();
    void slotInfoMessage(KJob *, const QString &);
    void goInternalUrl(const QUrl &);
    /**
      This function is called when the user clicks on a link in the viewer part.
    */
    void slotOpenURLRequest(const QUrl &url);
    void documentCompleted();
    void slotCopySelectedText();

private:
    void updateFontScaleActions();

    QSplitter *mSplitter = nullptr;
    ViewContainer *mViewContainer = nullptr;
    View *mDoc = nullptr;
    Navigator *mNavigator = nullptr;
    PageSearchBar *mPageSearchBar = nullptr;

    QAction *mLastSearchAction = nullptr;
    QAction *mCopyText = nullptr;
    LogDialog *mLogDialog = nullptr;
    bool mFullScreen = false;
    KBookmarkManager *mBookmarkManager;
};

}

#endif // KHC_MAINWINDOW_H
// vim:ts=2:sw=2:et
