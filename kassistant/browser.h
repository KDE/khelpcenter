/*
    This file is part of the KDE Help Center

    Copyright (C) 1998 René Beutler (rbeutler@g26.ethz.ch)
*/


#ifndef _BROWSER_H_
#define _BROWSER_H_

#include <kapp.h>
#include <qpopmenu.h>
#include <qframe.h>
#include <qevent.h>

#include <ktmainwindow.h>
#include <kmenubar.h>
#include <kstatusbar.h>
#include <ktoolbar.h>
#include <kaccel.h>
#include <html.h>

#include "noteditor.h"
#include "docserver.h"
#include "helpview.h"

class Browser : public KTMainWindow
{
	Q_OBJECT
  
public:

	Browser();
	enum IDS { ID_EXIT, ID_CLOSE, ID_MAGPLUS, ID_MAGMINUS, ID_BACK, ID_FORWARD, 
	           ID_RELOAD, ID_ATTACH, ID_PRINT, ID_QUESTIONLINE, ID_QUESTION, ID_INDEX,
		   ID_CLEAR, ID_BOOKMARK, ID_CMDPOPUP, ID_COPY, ID_STOP };
	void setURL( QString url, bool reload = FALSE );
	void setBaseDir( QString dir );

protected:

	void setupMenuBar();
	void setupAccelerators();
	void setupStatusBar();
	void setupToolBar();
	virtual void closeEvent( QCloseEvent * );
	virtual void resizeEvent( QResizeEvent * );
	void adjustBrowserSize();

public slots:

	void slotConfigure();

private slots:

	void slotAbout();
	void slotAttach();
	void slotBack();
	void slotClose();
	void slotCopy();
	void slotDocumentDone( KDocumentServer::requestType );
	void slotExit();
	void slotEmptyQuestion();
	void slotFreeze();
	void slotHelp();
	void slotHistoryActivated( int id );
	void slotHTMLWidgetDone();
	void slotMagMinus();
	void slotMagPlus();
	void slotPopupMenu( KHTMLView *_view, const char *_url, const QPoint &_point );
	void slotPrint();
	void slotQuestion();
	void slotQuestionAsked();
	void slotReload();
	void slotSettings();
	void slotSetTitle( const char *_title );
 	void slotTextSelected( KHTMLView *_view, bool _selected );
	void slotURLSelected( const char *_url );

private:
	bool autoSizeEnabled, adjustBrowserAllowed;
	QAccel *accel;
	KAccel *kAccel;	
	QPopupMenu *historyPopup, *cmdPopup;
	KHelpView *view;
	static int fontBase;
	QString currentURL, selectedURL, homeURL, documentTitle;
	QStrList URLHistory;
};

#endif
