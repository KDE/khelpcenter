/*
  This file is part of KHelpcenter.

  Copyright (C) 2002 Cornelius Schumacher <schumacher@kde.org>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; see the file COPYING.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/
#ifndef KHC_KCMHELPCENTER_H
#define KHC_KCMHELPCENTER_H

#include <KDialog>
#include <KSharedConfig>
#include <KProcess>

#include "scopeitem.h"

class QPushButton;
class QProgressBar;
class QTextEdit;
class QLabel;

class KAboutData;
class KTemporaryFile;
class KUrlRequester;
class QTreeWidget;

namespace KHC {
class DocEntry;
class SearchEngine;
}

class IndexDirDialog : public KDialog
{
    Q_OBJECT
  public:
    IndexDirDialog( QWidget *parent );

  protected Q_SLOTS:
    void slotOk();
    void slotUrlChanged( const QString &_url);
  private:
    KUrlRequester *mIndexUrlRequester;
};

class IndexProgressDialog : public KDialog
{
    Q_OBJECT
  public:
    IndexProgressDialog( QWidget *parent );
    ~IndexProgressDialog();

    void setTotalSteps( int );
    void advanceProgress();
    void setLabelText( const QString & );
    void setMinimumLabelWidth( int width );
    void setFinished( bool );

    void appendLog( const QString &text );

  Q_SIGNALS:
    void closed();
    void cancelled();

  protected:
    void hideDetails();

  protected Q_SLOTS:
    void slotEnd();
    void toggleDetails();

  private:
    QLabel *mLabel;
    QProgressBar *mProgressBar;
    QLabel *mLogLabel;
    QTextEdit *mLogView;

    bool mFinished;
};

class KCMHelpCenter : public KDialog
{
    Q_OBJECT
  public:
    explicit KCMHelpCenter( KHC::SearchEngine *, QWidget *parent = 0,
      const char *name = 0 );
    ~KCMHelpCenter();

    void load();
    bool save();
    void defaults();

  public Q_SLOTS:

  Q_SIGNALS:
    void searchIndexUpdated();
  public Q_SLOTS:
    void slotIndexError( const QString & );
    void slotIndexProgress();
  protected Q_SLOTS:
    bool buildIndex();
    void cancelBuildIndex();
    void slotIndexFinished( int exitCode, QProcess::ExitStatus exitStatus );
    void slotReceivedStdout();
    void slotReceivedStderr();
    void slotProgressClosed();

    void slotOk();

    void showIndexDirDialog();

    void checkSelection();

  protected:
    void setupMainWidget( QWidget *parent );
    void updateStatus();
    void startIndexProcess();

    void deleteProcess();
    void deleteCmdFile();

    void advanceProgress();

  private:
    KHC::SearchEngine *mEngine;

    QTreeWidget *mListView;
    QLabel *mIndexDirLabel;
    QPushButton *mBuildButton;
    IndexProgressDialog *mProgressDialog;

    QList<KHC::DocEntry *> mIndexQueue;
    QList<KHC::DocEntry *>::ConstIterator mCurrentEntry;

    KSharedConfigPtr mConfig;

    KAboutData *mAboutData;

    QWidget *mScopeTab;

    KTemporaryFile *mCmdFile;

    KProcess *mProcess;

    bool mIsClosing;

    QByteArray mStdOut;
    QByteArray mStdErr;

    bool mRunAsRoot;
};

#endif //KHC_KCMHELPCENTER_H
// vim:ts=2:sw=2:et
