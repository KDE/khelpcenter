/*
 *  khc_finddlg.cc - part of the KDE HelpCenter
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

#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <kapp.h>
#include "khc_finddlg.h"
#include <klocale.h>

KFindTextDialog::KFindTextDialog( QWidget *parent, const char *name )
    : QDialog( parent, name )
{
    // mimimise initial size of dialog
    resize( 0, 0 );

    QVBoxLayout *vl = new QVBoxLayout( this, 10 );

    QHBoxLayout *hl = new QHBoxLayout;
    vl->addLayout( hl );

    QLabel *label = new QLabel( i18n( "Find:" ), this );
    label->setFixedSize( label->sizeHint() );
    hl->addWidget( label );

    QLineEdit *edit = new QLineEdit( this );
    edit->setFixedHeight( edit->sizeHint().height() );
    edit->setFocus();
    connect( edit, SIGNAL( textChanged( const QString & ) ),
	     SLOT( slotTextChanged( const QString & ) ) );
    hl->addWidget( edit );

    hl = new QHBoxLayout;
    vl->addLayout( hl );

    QCheckBox *cb = new QCheckBox( i18n( "Case &sensitive" ), this );
    connect( cb, SIGNAL( toggled( bool ) ), this, SLOT( slotCase( bool ) ) );
    cb->setFixedSize( cb->sizeHint() );
    hl->addWidget( cb );

    hl->addStretch();

    hl = new QHBoxLayout;
    vl->addLayout( hl );

    QPushButton *btn = new QPushButton( i18n( "&Find" ), this );
    btn->setFixedSize( btn->sizeHint() );
    btn->setDefault( true );
    connect( btn, SIGNAL( clicked() ), this, SLOT( slotFind() ) );
    hl->addWidget( btn );

    hl->addStretch();

    btn = new QPushButton( i18n( "&Close" ), this );
    btn->setFixedSize( btn->sizeHint() );
    connect( btn, SIGNAL( clicked() ), this, SLOT( slotClose() ) );
    hl->addWidget( btn );

    vl->activate();
    
    setMinimumSize(sizeHint());

    rExp.setCaseSensitive( false );
}

void KFindTextDialog::slotTextChanged( const QString &t )
{
    rExp = t;
}

void KFindTextDialog::slotCase( bool c )
{
    rExp.setCaseSensitive( c );
}

void KFindTextDialog::slotClose()
{
    hide();
}

void KFindTextDialog::slotFind()
{
    emit find( rExp );
}

#include "khc_finddlg.moc"
