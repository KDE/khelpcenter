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

class QListView;
class QPushButton;
class QProgressBar;
class QTextEdit;
class QLabel;

class KProcess;
class KConfig;
class KAboutData;
class KTempFile;
class KURLRequester;

namespace KHC {
class HtmlSearchConfig;
class DocEntry;
}

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
};

class KCMHelpCenter : public KDialogBase, virtual public KCMHelpCenterIface
{
    Q_OBJECT
  public:
    KCMHelpCenter( QWidget* parent = 0, const char* name = 0 );
    ~KCMHelpCenter();
    
    void load();
    void save();
    void defaults();

    QString indexDir();

  public slots:

  signals:
    void searchIndexUpdated();

  protected slots:
    void buildIndex();
    void cancelBuildIndex();
    void slotIndexFinished( KProcess * );
    void slotIndexProgress();
    void slotReceivedStdout(KProcess *proc, char *buffer, int buflen);
    void slotReceivedStderr(KProcess *proc, char *buffer, int buflen);
    void slotProgressClosed();

    void slotOk();
    void slotApply();

  protected:
    QWidget *createScopeTab( QWidget *parent );
    void updateStatus();
    void startIndexProcess();

  private:
    QListView *mListView;
    KURLRequester *mIndexUrlRequester;
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
