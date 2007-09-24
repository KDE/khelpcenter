/*
  This file is part of KHelpcenter.

  Copyright (C) 2002 Cornelius Schumacher <schumacher@kde.org>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public
  License version 2 as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; see the file COPYING.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/
#ifndef KCMHELPCENTER_H
#define KCMHELPCENTER_H

#include <kdialog.h>
#include <ksharedconfig.h>

#include "scopeitem.h"

class QPushButton;
class QProgressBar;
class QTextEdit;
class QLabel;

class K3Process;
class KAboutData;
class KTemporaryFile;
class KUrlRequester;
class K3ListView;

namespace KHC {
class HtmlSearchConfig;
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
    void slotIndexFinished( K3Process * );
    void slotReceivedStdout(K3Process *proc, char *buffer, int buflen);
    void slotReceivedStderr(K3Process *proc, char *buffer, int buflen);
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
  
    K3ListView *mListView;
    QLabel *mIndexDirLabel;
    QPushButton *mBuildButton;
    IndexProgressDialog *mProgressDialog;
    
    QList<KHC::DocEntry *> mIndexQueue;
    QList<KHC::DocEntry *>::ConstIterator mCurrentEntry;
    
    KSharedConfigPtr mConfig;
    
    KAboutData *mAboutData;

    KHC::HtmlSearchConfig *mHtmlSearchTab;
    QWidget *mScopeTab;

    KTemporaryFile *mCmdFile;

    K3Process *mProcess;

    bool mIsClosing;

    QString mStdOut;
    QString mStdErr;

    bool mRunAsRoot;
};

#endif
// vim:ts=2:sw=2:et
