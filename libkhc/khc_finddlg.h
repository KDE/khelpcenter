/*
 *  khc_finddlg.h - part of the KDE HelpCenter
 *
 *  Copyright (c) 1999 Matthias Elter (me@kde.org)
 *  Copyright (C) 1998 Martin Jones (mjones@kde.org)
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
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __khc_finddlg_h__
#define __khc_finddlg_h__

#include <qdialog.h>
#include <qregexp.h>

class KFindTextDialog : public QDialog
{
    Q_OBJECT

 public:
    KFindTextDialog( QWidget *parent = 0, const char *name = 0 );

    const QRegExp &regExp() const { return rExp; }

 signals:
    /*
     * connect to me to find out when the user has pressed the find button.
     */
    void find( const QRegExp &re );

 public slots:
    void slotTextChanged( const QString& t );
    void slotCase( bool c );
    void slotClose();
    void slotFind();

 protected:
    QRegExp rExp;
};

#endif

