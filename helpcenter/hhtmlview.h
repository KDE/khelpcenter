/*
 *  hhtmlview.cpp - part of the KDE Help Center
 *
 *  Copyright (C) 1999 Matthias Elter (me@kde.org)
 *
 *  based on code from Martin R. Jones's kdehelp
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

#ifndef HHTMLView_H
#define HHTMLView_H

#include <qwidget.h>
#include <qdialog.h>
#include <qpopupmenu.h>
#include <qscrollbar.h>
#include <qlabel.h>
#include <qcursor.h>

#include <khtml.h>
#include <kfilebookmark.h>

#include "cgi.h"
#include "misc.h"
#include "man.h"
#include "options.h"
#include "history.h"
#include "finddlg.h"
#include "fmthtml.h"

#define SCROLLBAR_WIDTH		16
#define BOOKMARK_ID_BASE	200

//-----------------------------------------------------------------------------

class KOpenURLDialog : public QDialog
{
	Q_OBJECT
public:
	KOpenURLDialog( QWidget *parent = NULL, const char *name = NULL );

signals:
	void openURL( const char *URL, int );

public slots:
	void openPressed();

private:
	QLineEdit *lineEdit;
};

//-----------------------------------------------------------------------------

class KPageInfo
{
public:
	KPageInfo( const char *u, int y )
		{	url = u; yOffset = y; }

    KPageInfo( const KPageInfo &i )
        {   url = i.url.copy(); yOffset = i.yOffset; }

	const QString getUrl() const
		{	return url; }
	int getOffset() const
		{	return yOffset; }

	void setOffset( int y )
		{	yOffset = y; }

private:
	QString url;
	int yOffset;
};

//-----------------------------------------------------------------------------

class KHelpView : public KHTMLWidget
{
public:
    KHelpView( QWidget *parent = 0L, const char *name = 0L );
    virtual ~KHelpView();

    static KHistory *urlHistory;

    void setDefaultFontBase (int fSize);
protected:
    virtual bool URLVisited( const char *_url );
};

//-----------------------------------------------------------------------------

class KHelpWindow : public QWidget
{
	Q_OBJECT
public:
	// List of all for the HelpWindow that can currently be 
	// triggered off externally
	enum AllowedActions { Copy, GoBack, GoForward, GoUp, Stop };

	KHelpWindow(QWidget *parent=NULL, const char *name=NULL);
	virtual ~KHelpWindow();

	int openURL( const char *URL, bool withHistory = true );

	bool canCurrentlyDo(AllowedActions action);
	const char *getCurrentURL();

    const cHistory<KPageInfo> &getHistory() const { return history; }
	void setHistory( const cHistory<KPageInfo> &hist )
        {   history = hist; }

signals:
	void enableMenuItems();
	void openNewWindow(const QString& newURL);

	void setURL( const QString& url);
	// the name of the url being displayed has changed to "url"
	
	void setLocation( const QString& url);
	// This signal gets emitted when the URL in the LocationBar should be changed
     
	void bookmarkChanged(KFileBookmark *);

	void setTitle(const QString& _title);

public slots:
	void	slotOpenFile();
	void	slotOpenURL();
	void	slotSearch();
	void	slotReload();
	void	slotPrint();

	void	slotCopy();
	void	slotFind();
	void	slotFindNext();
	void	slotFindNext( const QRegExp & );
	void	slotBack();
	void	slotForward();
	void	slotDir();
	void	slotUp();
	void	slotTextSelected( bool sel );
	void	slotAddBookmark();
	void	slotBookmarkSelected( int id );
	void	slotBookmarkHighlighted( int id );
	void	slotBookmarkChanged();
	void	slotStopProcessing();
	void	slotSetTitle( const char * );
	void	slotURLSelected( const char *, int );
	void	slotOnURL( const char * );
	void	slotFormSubmitted( const char *, const char *, const char *, const char * );
	void	slotPopupMenu( const char *, const QPoint & );
	void	slotCGIDone();
	void	slotScrollVert( int _y );
	void	slotScrollHorz( int _y );
	void	slotBackgroundColor( const QColor &col );
	void	slotFontSize( int );
	void	slotStandardFont( const QString& );
	void	slotFixedFont( const QString& );
	void	slotColorsChanged( const QColor&, const QColor&, const QColor&,
		    const QColor&, const bool, const bool );
	void	slotPopupOpenURL();
	void	slotPopupAddBookmark();
	void	slotPopupOpenNew();
	void	slotViewResized( const QSize & );
	void	slotDocumentChanged();
	void	slotDocumentDone();

protected:
	virtual void resizeEvent( QResizeEvent * );

	virtual bool eventFilter( QObject *, QEvent * );
	virtual bool x11Event( XEvent * );

private:
	enum FileType { UnknownFile, HTMLFile, InfoFile, ManFile, CannotOpenFile };

	void	readOptions();
	int	    openFile( const QString & );
	int	    formatMan( int bodyOnly = FALSE );
	int 	openHTML( const char *location );
	int	    runCGI( const char *_url );
	FileType detectFileType( const QString &filename );
	void	convertSpecial( const char *buffer, QString &converted );
	void	enableToolbarButton( int id, bool enable );
	void	createMenu();
	QString	getPrefix();
	QString	getLocation();
	void	addBookmark( const char *_title, const char *url );
	void	layout();

private:
	QScrollBar *vert;
	QScrollBar *horz;
	QLabel *statusBar;
	QPopupMenu *rmbPopup;
	KHelpView *view;
	KOpenURLDialog *openURLDialog;
	KFindTextDialog *findDialog;

	QString localFile;

	KCGI *CGIServer;

	static KHelpOptionsDialog *optionsDialog;
	static KFileBookmarkManager bookmarkManager;

	// html view preferences
	static int  fontBase;
	static QString standardFont;
	static QString fixedFont;

	static QColor bgColor;
	static QColor textColor;
	static QColor linkColor;
	static QColor vLinkColor;
	static bool   underlineLinks;
	static bool   forceDefaults;

	QString fullURL;
	QString currentURL;
	QString currentInfo;
	QString title;
	QString ref;

	// current width of the html view
	int viewWidth;

	// scroll to here when parsed
	int scrollTo;

	// busy parsing
	bool busy;

	QCursor oldCursor;

	cHistory<KPageInfo> history;
	cHTMLFormat html;
	cMan *man;
	cHelpFormatBase *format;

	static QString newURL;
};

#endif

