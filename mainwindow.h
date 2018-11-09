 /*
 *  This file is part of the KDE Help Center
 *
 *  Copyright (C) 1999 Matthias Elter (me@kde.org)
 *                2001 Stephan Kulow (coolo@kde.org)
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

#ifndef KHC_MAINWINDOW_H
#define KHC_MAINWINDOW_H

#include <KXmlGuiWindow>

#include <KParts/BrowserExtension>

#include "glossary.h"

class QSplitter;

class LogDialog;

namespace KIO {

class Job;

}

namespace KHC {

class Navigator;
class View;

class MainWindow : public KXmlGuiWindow
{
    Q_OBJECT
	Q_CLASSINFO("D-Bus Interface", "org.kde.khelpcenter.khelpcenter")
  public:
    MainWindow();
    ~MainWindow() override;

  public Q_SLOTS:
    Q_SCRIPTABLE void openUrl( const QString &url );
    Q_SCRIPTABLE void openUrl( const QString &url, const QByteArray& startup_id );
    Q_SCRIPTABLE void showHome();
    Q_SCRIPTABLE void lastSearch();

  public Q_SLOTS:
    void print();
    void statusBarRichTextMessage(const QString &m);
    void statusBarMessage(const QString &m);
    void slotShowHome();
    void slotLastSearch();
    void showSearchStderr();
    /**
      Show document corresponding to given URL in viewer part.
    */
    void viewUrl( const QString & );

    /**
      Open document corresponding to given URL, i.e. show it in the viewer part
      and select the corresponding entry in the navigator widget.
    */
    void openUrl( const QUrl &url );

  protected:
    void setupActions();
    void setupBookmarks();
    /**
      Show document corresponding to given URL in viewer part.
    */
    void viewUrl( const QUrl &url,
                  const KParts::OpenUrlArguments &args = KParts::OpenUrlArguments(),
                  const KParts::BrowserArguments &browserArgs = KParts::BrowserArguments() );

    void saveProperties( KConfigGroup &config ) override;
    void readProperties( const KConfigGroup &config ) override;

    void readConfig();
    void writeConfig();

  protected Q_SLOTS:
    void enableLastSearchAction();
    void enableCopyTextAction();

  private:
    void stop();

  private Q_SLOTS:
    void slotGlossSelected(const GlossaryEntry &entry);
    void slotStarted(KIO::Job *job);
    void slotInfoMessage(KJob *, const QString &);
    void goInternalUrl( const QUrl & );
    /**
      This function is called when the user clicks on a link in the viewer part.
    */
    void slotOpenURLRequest( const QUrl &url,
                             const KParts::OpenUrlArguments &args = KParts::OpenUrlArguments(),
                             const KParts::BrowserArguments &browserArgs = KParts::BrowserArguments());
    void documentCompleted();
    void slotIncFontSizes();
    void slotDecFontSizes();
    void slotConfigureFonts();
    void slotCopySelectedText();

private:
    void updateFontScaleActions();

    QSplitter *mSplitter;
    View *mDoc;
    Navigator *mNavigator;

    QAction *mLastSearchAction;
    QAction *mCopyText;
    LogDialog *mLogDialog;
};

}

#endif //KHC_MAINWINDOW_H
// vim:ts=2:sw=2:et
