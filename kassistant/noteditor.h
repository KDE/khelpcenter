#ifndef __KNOTEDITOR_H
#define __KNOTEDITOR_H

#include <qdialog.h>
#include <keditcl.h>

class KNoteEditor : public QDialog
{
	Q_OBJECT

public:
	KNoteEditor( QWidget *parenti = 0L, const char *name = 0L );
	virtual ~KNoteEditor();
	void setFile( const char *appName, const char *fileName );

protected slots:
	void slotDelete();
	void slotOK();

protected:
	KEdit *edit;

};

#endif 
