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

#include <kcmodule.h>

class QListView;
class QPushButton;
class QProgressDialog;
class KProcess;

class KCMHelpCenter : public KCModule
{
    Q_OBJECT
  public:
    KCMHelpCenter( QWidget* parent = 0, const char* name = 0 );
    ~KCMHelpCenter();
    
    virtual const KAboutData * aboutData () const;

    void load();
    void save();
    void defaults();
    QString quickHelp() const;

  public slots:

  protected slots:
    void buildIndex();
    void cancelBuildIndex();
    void slotIndexFinished( KProcess * );

  protected:
    void processIndexQueue();
    void updateStatus();

  private:
    QListView *mListView;
    QPushButton *mBuildButton;
    QProgressDialog *mProgressDialog;
    
    QStringList mIndexQueue;
    
    KConfig *mConfig;
    
    KAboutData *mAboutData;
};

#endif
