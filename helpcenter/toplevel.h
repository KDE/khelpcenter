/*
 *  toplevel.h - part of the KDE Help Center
 *
 *  Copyright (c) 199 Matthias Elter (me@kde.org)
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

#define HELPCENTER_VERSION		"0.6"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

#include <kapp.h>
#include <knewpanner.h>
#include <ktmainwindow.h>

#include <qpopupmenu.h>
#include <qsplitter.h>
#include <qlist.h>

class HTabView;
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
	unsigned long getListIndex() { return listIndex; }
	static HelpCenter *getHelpWindowAt(unsigned long id) 
	  { return helpWindowList.at(id); }

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
	
	void slotMagMinus();
	void slotMagPlus();

	void slotQuit();

	void slotEnableMenuItems();

	void slotOptionsTree();
	void slotOptionsToolbar();
	void slotOptionsLocationbar();
	void slotOptionsStatusbar();
	void slotOptionsGeneral();
	void slotOptionsSave();
	void slotReadConfig();
	void slotSetBusy(bool bussy);

signals:
	void setBusy(bool bussy);

protected:
    virtual void resizeEvent(QResizeEvent *);

    virtual void saveProperties(KConfig *);
    virtual void readProperties(KConfig *);

private slots:
	void slotLocationEntered();
    void slotHistoryFillBack();
    void slotHistoryFillForward();
    void slotHistoryBackActivated(int id);
    void slotHistoryForwardActivated(int id);
	void slotAnimatedWheelTimeout();

private:
	void enableMenuItems();
	void enableTree(bool enable);
	void fillBookmarkMenu(KFileBookmark *parent, QPopupMenu *menu, int &id);
	void setupMenubar();
	void setupToolbar();
	void setupStatusbar();
	void setupLocationbar();
	void setupView();

private:
	QSplitter *splitter;
	KHelpView *htmlview;
	HTabView *tabview;

	QPopupMenu *fileMenu, *editMenu, *viewMenu, *gotoMenu, *optionsMenu, *helpMenu,*bookmarkMenu;
	QPopupMenu *historyBackMenu, *historyForwardMenu;

	// toolbar id's:
	enum {TB_TREE, TB_BACK, TB_FORWARD, TB_RELOAD, TB_STOP, TB_PRINT, TB_BOOKMARK, TB_ZOOMIN,
		  TB_ZOOMOUT, TB_FIND, TB_WHEEL};
	// menu id's:
	int idCopy, idBack, idForward, idTop, idUp, idPrevious, idNext, idTree, idToolbar
	  ,idLocationbar, idStatusbar, idMagPlus, idMagMinus;

	// GUI options:
	bool showStatusbar, showToolbar, showLocationbar, showTree;
	int fontBase;

	// static list of HelpCenter windows
	static QList<HelpCenter> helpWindowList;

	// static list of wheel frames
	static QList<QPixmap> animatedWheel;

	// index of HelpCenter instance in helpWindowList
	unsigned long listIndex;

	// The image from 0...animatedWheel.count()-1 we are currently displaying.
    uint animatedWheelCounter;
	
	// Timer used to display the animated logo.
	QTimer *animatedWheelTimer;
};

#endif // TOPLEVEL_H
