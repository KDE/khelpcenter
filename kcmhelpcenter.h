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
  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
  Boston, MA 02111-1307, USA.
*/
#ifndef KCMHELPCENTER_H
#define KCMHELPCENTER_H

#include <kdialog.h>
#include <kdialogbase.h>

#include <dcopobject.h>

#include "scopeitem.h"

class QPushButton;
class QProgressBar;
class QTextEdit;
class QLabel;

class KProcess;
class KConfig;
class KAboutData;
class KTempFile;
class KURLRequester;
class KListView;

namespace KHC {
class HtmlSearchConfig;
class DocEntry;
class SearchEngine;
}

class IndexDirDialog : public KDialogBase
{
    Q_OBJECT
  public:
    IndexDirDialog( QWidget *parent );

  protected slots:
    void slotOk();
    
  private:
    KURLRequester *mIndexUrlRequester;
};

class IndexProgressDialog : public KDialog
{
    Q_OBJECT
  public:
    IndexProgressDialog( QWidget *parent );

    void setTotalSteps( int );
    void advanceProgress();
    void setLabelText( const QString & );
    void setMinimumLabelWidth( int width );
    void setFinished( bool );

    void appendLog( const QString &text );

  signals:
    void closed();
    void cancelled();

  protected slots:
    void slotEnd();
    void toggleDetails();

  private:
    QLabel *mLabel;
    QProgressBar *mProgressBar;
    QLabel *mLogLabel;
    QTextEdit *mLogView;
    QPushButton *mDetailsButton;
    QPushButton *mEndButton;

    bool mFinished;
};

class KCMHelpCenterIface : virtual public DCOPObject
{
    K_DCOP
  k_dcop:
    virtual void slotIndexProgress() = 0;
    virtual void slotIndexError( const QString & ) = 0;
};

class KCMHelpCenter : public KDialogBase, virtual public KCMHelpCenterIface
{
    Q_OBJECT
  public:
    KCMHelpCenter( KHC::SearchEngine *, QWidget *parent = 0,
      const char *name = 0 );
    ~KCMHelpCenter();
    
    void load();
    bool save();
    void defaults();

  public slots:

  signals:
    void searchIndexUpdated();

  protected slots:
    bool buildIndex();
    void cancelBuildIndex();
    void slotIndexFinished( KProcess * );
    void slotIndexProgress();
    void slotIndexError( const QString & );
    void slotReceivedStdout(KProcess *proc, char *buffer, int buflen);
    void slotReceivedStderr(KProcess *proc, char *buffer, int buflen);
    void slotProgressClosed();

    void slotOk();

    void showIndexDirDialog();

  protected:
    void setupMainWidget( QWidget *parent );
    void updateStatus();
    void startIndexProcess();

    void deleteProcess();
    void deleteCmdFile();

    void advanceProgress();

  private:
    KHC::SearchEngine *mEngine;
  
    KListView *mListView;
    QLabel *mIndexDirLabel;
    QPushButton *mBuildButton;
    IndexProgressDialog *mProgressDialog;
    
    QValueList<KHC::DocEntry *> mIndexQueue;
    QValueList<KHC::DocEntry *>::ConstIterator mCurrentEntry;
    
    KConfig *mConfig;
    
    KAboutData *mAboutData;

    KHC::HtmlSearchConfig *mHtmlSearchTab;
    QWidget *mScopeTab;

    KTempFile *mCmdFile;

    KProcess *mProcess;

    bool mIsClosing;

    QString mStdOut;
    QString mStdErr;

    bool mRunAsRoot;
};

#endif
// vim:ts=2:sw=2:et
