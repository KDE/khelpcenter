/*
    This file is part of the KDE Help Center

    Copyright (C) 1998 René Beutler (rbeutler@g26.ethz.ch)
*/

#include <qfile.h>
#include <qlabel.h>
#include <qpushbt.h>
#include <qmessagebox.h>
#include <qlayout.h>

#include <kseparator.h>

#include "noteditor.h"

KNoteEditor::KNoteEditor( QWidget* parent, const char* name ) : QDialog( parent, name, TRUE, 12288 )
{

	QLabel *EnterText = new QLabel( i18n("Your annotations:" ) , this );
	EnterText->setMinimumSize( EnterText->sizeHint() );
	EnterText->setMaximumSize( 32767, 32767 );
	
	edit = new KEdit( kapp, this );
	CHECK_PTR( edit );
//	edit->setGeometry( 5, 30, 247, 275 );
	edit->setMinimumSize( 0, 0 );
	edit->setMaximumSize( 32767, 32767 );
	edit->setReduceWhiteOnJustify( TRUE );
	edit->saveBackupCopy( FALSE );	
	edit->newFile();
	QPopupMenu *popup = new QPopupMenu();
	popup->insertItem( "C&ut",   edit, SLOT(cut()), 0 );
	popup->insertItem( "&Copy",  edit, SLOT(copyText()), 0 );
	popup->insertItem( "&Paste", edit, SLOT(paste()),0 );
	edit->installRBPopup( popup );

	KSeparator *separator = new KSeparator( this );
	
	QButton /* *helpButton, */ *deleteButton, *okButton, *cancelButton;

	KButtonBox *bbox = new KButtonBox( this );		CHECK_PTR( bbox );
/*	helpButton   = bbox->addButton( i18n( "Help" ) );	CHECK_PTR( helpButton );
	               bbox->addStretch();
*/
	deleteButton = bbox->addButton( i18n( "Delete" ) );	CHECK_PTR( deleteButton );
	               bbox->addStretch();
	okButton     = bbox->addButton( i18n( "OK" ) );		CHECK_PTR( okButton );
	cancelButton = bbox->addButton( i18n( "Cancel" ) );	CHECK_PTR( cancelButton );
	bbox->layout();
	
	connect( okButton,     SIGNAL(clicked()), SLOT(slotOK()) );
	connect( cancelButton, SIGNAL(clicked()), SLOT(reject()) );
	connect( deleteButton, SIGNAL(clicked()), SLOT(slotDelete()) );

	QVBoxLayout* layout_1 = new QVBoxLayout( this, 10, 10 );
	layout_1->addWidget( EnterText );
	layout_1->addWidget( edit, 1 );
	layout_1->addSpacing( 5 );
	layout_1->addWidget( separator );
	layout_1->addSpacing( 5 );
	layout_1->addWidget( bbox );

	layout_1->activate();

	resize( 280,280 );
	setMinimumSize( 0, 0 );
	setMaximumSize( 32767, 32767 );
}


KNoteEditor::~KNoteEditor()
{
}

void KNoteEditor::setFile( const char *appName, const char *fileName )
{
	QString d = KApplication::localkdedir() + "/share/apps/kassistant/" + appName + "/";
	if ( access( d, F_OK ) )
		mkdir( d, 0740 );
	
	d += fileName;
	QFileInfo f( (const char *)d );

	if( f.exists() )
	{
		if( f.isReadable() == FALSE ) {	
		  return;
		}
		else if ( f.isWritable() == FALSE ) {
		  if (QMessageBox::warning( this, "Warning - Annotation Editor",
					    i18n("You have no write permissions\nfor this annotation.\nIt will be opened read only."),
					    i18n("&Open"), i18n("&Cancel")))
		    return;
		}
		edit->loadFile( d, KEdit::OPEN_INSERT );		
	}

	edit->setName( f.filePath() );
	edit->toggleModified( FALSE );
}

void KNoteEditor::slotOK()
{
	if( edit->isModified() == TRUE )
	{
		
		edit->doSave();
		accept();
	}
	else
		reject();
}

void KNoteEditor::slotDelete()
{
	printf("KNoteEditor: deleting %s\n", (const char *) edit->getName() );
	if (!QMessageBox::warning( this, i18n("Confirm Delete - Annotation Editor"),
				   i18n("Do you really want to\ndelete your annotation?"),
				   i18n("&Yes"), i18n("&No"), 0, 1))
	  {
	    QFile::remove( edit->getName() );
	    accept();
	  }
}

#include "noteditor.moc"
