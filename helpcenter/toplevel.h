/*
 *  toplevel.h - part of the KDE Help Center
 *
 *  Copyright (c) 199 Matthias Elter (me@main-echo.net)
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

#ifndef TOPLEVEL_H
#define TOPLEVEL_H

#define HELPCENTER_VERSION		"0.4"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

#include <kapp.h>
#include <knewpanner.h>
#include <ktmainwindow.h>

#include <qpopupmenu.h>
#include <qsplitter.h>

class HTreeView;
class KHelpView;
class KFileBookmark;

class HelpCenter : public KTMainWindow
{
	Q_OBJECT
public:
	HelpCenter();
	virtual ~HelpCenter();

	int openURL( const char *URL, bool withHistory = true );
	void openNewWindow( const char *url );

    KHelpView *htmlView() { return htmlview; }

public slots:
	void slotBookmarkChanged(KFileBookmark *parent);
	void slotToolbarClicked(int);
	void slotURLSelected(const QString& _url, int _button);

	void slotSetTitle(const QString& _title );
	void slotSetLocation(const QString& _url);
	void slotSetURL(QString url);
	void slotSetStatusText(const QString& text);

	void slotNewWindow(const QString& url);
    void slotCloneWindow();

	void slotQuit();

	void slotEnableMenuItems();

	void slotOptionsTree();
	void slotOptionsToolbar();
	void slotOptionsLocationbar();
	void slotOptionsStatusbar();
	void slotOptionsGeneral();
	void slotOptionsSave();

protected:
    virtual void resizeEvent(QResizeEvent *);

    virtual void saveProperties(KConfig *);
    virtual void readProperties(KConfig *);

private slots:
	  void slotLocationEntered();

private:
	void enableMenuItems();
	void enableTree(bool enable);
	void fillBookmarkMenu(KFileBookmark *parent, QPopupMenu *menu, int &id);
	void readConfig();
	void setupMenubar();
	void setupToolbar();
	void setupStatusbar();
	void setupLocationbar();
	void setupView();

private:
	QSplitter *splitter;
	KHelpView *htmlview;
	HTreeView *treeview;

	QPopupMenu *fileMenu, *editMenu, *viewMenu, *gotoMenu, *optionsMenu, *helpMenu,*bookmarkMenu;

	// toolbar id's:
	enum {TB_BACK, TB_FORWARD, TB_RELOAD, TB_STOP, TB_PRINT, TB_BOOKMARK, TB_ZOOMIN,
		  TB_ZOOMOUT, TB_FIND};
	// menu id's:
	int idCopy, idBack, idForward, idTop, idUp, idPrevious, idNext, idTree, idToolbar
	  ,idLocationbar, idStatusbar;

	// GUI options:
	bool showStatusbar, showToolbar, showLocationbar, showTree;
};

#endif // TOPLEVEL_H
