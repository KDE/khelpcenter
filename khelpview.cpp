/*
 *  khelpview.cpp - part of the KDE Help Center
 *
 *  Copyright (c) 1999 Matthias Elter (me@kde.org)
 *
 *  $Id$
 *
 *  based on kdehelp code (c) Martin R. Jones
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

#include <stdlib.h>

#include <qmessagebox.h>
#include <qclipboard.h>

#include <kapp.h>
#include <kurl.h>
#include <krun.h>
#include <kfiledialog.h>
#include <kcursor.h>

#include "khelpview.h"
#include "man.h"
#include "cgiserver.h"

#ifdef HAVE_PATHS_H
#include <paths.h>
#endif

#ifndef _PATH_TMP
#define _PATH_TMP "/tmp"
#endif

#include <unistd.h>

// for selection
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <klocale.h>
#include <qfile.h>
#include <kstddirs.h>

KHelpView::KHelpView(QWidget *parent, const char *name)
    : QWidget(parent, name)
{
    CGIServer = 0;
    busy = false;
    scrollTo = 0;
    rmbPopup = 0;
    findDialog = 0;
  
    // keep the history in a QList<KPageInfo>
    history.setAutoDelete(true);
    // points to the current item in history
    histCurrent = 0L;
  
    man = new ManParser;

    view = new KHTMLWidget( this );
    CHECK_PTR( view );

    view->setURLCursor(KCursor::handCursor());
    view->setUnderlineLinks(true);
    view->setFocusPolicy(QWidget::StrongFocus);
    view->setFocus();
    view->installEventFilter(this);
    view->setUpdatesEnabled(true);
    view->setDefaultBGColor(white);
    view->setDefaultTextColors(black, blue, blue);
    view->setGranularity(600);

    vert = new QScrollBar(0, 0, 12, view->height(), 0, QScrollBar::Vertical, this, "vert");
    horz = new QScrollBar(0, 0, 24, view->width(), 0, QScrollBar::Horizontal, this, "horz");
  
    connect(&bookmarkManager, SIGNAL(changed()), SLOT(slotBookmarkChanged()));
  
    connect(view, SIGNAL(scrollVert(int)), SLOT(slotScrollVert(int)));
    connect(view, SIGNAL(scrollHorz(int)), SLOT(slotScrollHorz(int)));
    connect(view, SIGNAL(setTitle(const char *)), SLOT(slotSetTitle(const char *)));
    connect(view, SIGNAL(URLSelected(const char *, int)),
	    SLOT(slotURLSelected(const char *, int)));
    connect(view, SIGNAL(onURL(const char *)), SLOT(slotOnURL(const char *)));
    connect(view, SIGNAL(popupMenu(const char *, const QPoint &)),
	    SLOT(slotPopupMenu(const char *, const QPoint &)));
    connect(view, SIGNAL(formSubmitted(const char *, const char *, const char *, const char *)),
	    SLOT(slotFormSubmitted(const char *, const char *, const char *, const char *)));
    connect(view, SIGNAL(resized(const QSize &)), SLOT(slotViewResized(const QSize &)));
    connect(view, SIGNAL(textSelected(bool)), SLOT(slotTextSelected(bool)));
  
    connect(vert, SIGNAL(valueChanged(int)), view, SLOT(slotScrollVert(int)));
    connect(horz, SIGNAL(valueChanged(int)), view, SLOT(slotScrollHorz(int)));
  
    connect(view, SIGNAL(documentChanged()), SLOT(slotDocumentChanged()));
    connect(view, SIGNAL(documentDone()), SLOT(slotDocumentDone()));
    connect(view, SIGNAL(documentStarted()), SLOT(slotDocumentStarted()));
  
    // load bookmarks
    QString p = KApplication::localkdedir();
    QString bmFile = p + "/share/apps/khelpcenter/bookmarks.html";
    bookmarkManager.read( bmFile );
  
    // setup geometry
    layout();
}

KHelpView::~KHelpView()
{
    if (findDialog)
	delete findDialog;
  
    delete man;
}

// Open a URL.  Initiate remote transfer if necessary
int KHelpView::openURL( const char *URL, bool withHistory )
{
    char location[256];
    int pos, rv = 1, viewPos = view->yOffset();
    const char *colon;
    bool isRemote = false;
  
    scrollTo = 0;
    ref = "";
  
    if (busy)
	view->setCursor(oldCursor);

    if (CGIServer)
    {
	delete CGIServer;
	CGIServer = 0;
    }
  
    if (!URL)
	return 1;

    if (URL[0] == '\"')	// skip leading "
	URL++;

    fullURL = URL;

    if ((pos = fullURL.findRev('\"')) > 0)
	fullURL.truncate(pos);

    // is this actually a file:
    if ((fullURL[0] == '/' && fullURL[1] != '/') || fullURL[0] == '~')
	fullURL = "file:" + fullURL;

    // extract reference
    int prevPos = currentURL.findRev('#');
    prevPos = prevPos >= 0 ? prevPos : currentURL.length();

    int refPos = fullURL.findRev('#');
    if (refPos < 0)
    {
	ref = QString::null;
	refPos = fullURL.length();
    }
    else
	ref = fullURL.right(fullURL.length() - refPos - 1);
    fullURL.truncate(refPos);

    if (fullURL == currentURL.left(prevPos))
    {
	if (!ref.isEmpty())
	{
	    if (!view->gotoAnchor(ref))
		return 1;
	    currentURL.truncate(prevPos);
	    currentURL += "#";
	    currentURL += ref;
	}
	else
	    currentURL.truncate(prevPos);
	  
	if (withHistory)
	{
	    if (histCurrent)
		histCurrent->setOffset(viewPos);
	    KPageInfo *entry = new KPageInfo(currentURL, 0);
	    entry->setOffset(0);
	    history.append(entry);
	    histCurrent = entry;
	    if (history.count() > MAX_HISTORY_LENGHT)
		history.removeFirst();
	}
	emit enableMenuItems();
	emit setURL(currentURL);
	emit setLocation(currentURL);
	return 0;
    }
  
    // if this is a relative location, then use path of current URL
    colon = strchr(fullURL, ':');
    if (!colon)
    {
	int i = currentURL.findRev('/');
	if (i >= 0)
	{
	    fullURL = currentURL.left(i + 1) + fullURL;
	    colon = strchr(fullURL, ':');
	}
	else
	    return 1;
    }

    strcpy(location, colon+1);
  
    oldCursor = view->cursor();
    view->setCursor(waitCursor);

    if (fullURL.find("file:/") >= 0)
    {
	// use internal CGI sever for local CGI stuff
	if (fullURL.find("cgi-bin") > 0)
	{
	    rv = runCGI(fullURL);
	    isRemote = true;
	}
	else
	    rv = openFile(location);
    }
    else if (fullURL.find("man:/") >= 0)
    {
	strcpy(location, colon+2);
	if ((rv = man->readLocation(location)) == 0)
	    formatMan();
    }
    else
    {
        (void) new KRun( fullURL ); // will autodelete itself
    }
  
    if (!rv)
    {
	busy = true;
		
	currentURL = fullURL.copy();
	if (ref.length() > 0)
	{
	    currentURL += '#';
	    currentURL += ref;
	}
	if (withHistory)
	{
	    if (histCurrent)
		histCurrent->setOffset(viewPos);
	    KPageInfo *entry = new KPageInfo(currentURL, 0);
	    entry->setOffset(0);
	    history.append(entry);
	    if (history.count() > MAX_HISTORY_LENGHT)
		history.removeFirst();
	    histCurrent = entry;
	}
	emit enableMenuItems();
	  
	if (!isRemote)
	{
	    view->parse();
	    horz->setValue(0);
	    emit setURL(currentURL);
	    emit setLocation(currentURL); 
	    view->setCursor(oldCursor);
	}
    }
    else
	view->setCursor(oldCursor);
  
    return rv;
}


// actually read the file (must be on local disk)
int KHelpView::openFile(const QString &location)
{
    int rv = 1;
    QString fileName;
  
    if (location[0] == '~')
    {
	fileName = getenv("HOME");
	fileName += '/';
	fileName += location.data() + 1;
    }
    else
	fileName = location.copy();
  
    switch (detectFileType(fileName))
    {
    case HTMLFile:
	rv = openHTML(fileName);
	break;  
    case ManFile:
	if ((rv = man->readLocation(fileName)) == 0)
	    formatMan();
	break;
    case CannotOpenFile:
    {
	//view->setGranularity(600);
	view->begin(fullURL);
		
	view->write("<html><head><title>Error: File not found!</title></head><body>");
	view->write("<H2>Error: File not found</H2>");
	view->write("<br>Could not find or open file:<br>" + fileName);
	view->write("</body></html>");

	view->end();
	rv = 0;
    }
    break;
	  
    default:
    {
	//view->setGranularity(600);
	view->begin(fullURL);
		
	view->write("<html><head><title>Error: Unknown file format!</title></head><body>");
	view->write("<H2>Error: Unknown file format</H2>");
	view->write("<br>Could not open unknown file:<br>" + fileName);
	view->write("</body></html>");

	view->end();
	rv = 0;
    }
    }
    return rv;
}

int KHelpView::formatMan(int bodyOnly)
{
    if (!bodyOnly)
    {
	view->begin(fullURL);
	view->write("<html><head><title>");
	view->write(man->location());
	view->write("</title></head><body>");
    }
    view->write("<big>");
    view->write(man->location());
    view->write("</big><br><HR><p>");
    view->write(*man->page());
    view->write("</p>");
  
    if (!bodyOnly)
    {
	view->write("</body></html>");
	view->end();
    }
    return 0;
}

int KHelpView::openHTML(const char *location)
{
    if (access(location, R_OK) !=0)
	return 1;

    char buffer[256];
    int val;
    QFile file(location);

    if (!file.open(IO_ReadOnly))
	return 1;
  
    //view->setGranularity(600);
    view->begin(fullURL);
  
    do
    {
	buffer[0] = '\0';
	val = file.readLine(buffer, 256);
	if (strncmp(buffer, "Content-type", 12))
	    view->write(buffer);
    }
    while (!file.atEnd());
  
    view->end();
    return 0;
}

int KHelpView::runCGI(const char *_url)
{
    if (CGIServer)
    {
	delete CGIServer;
	KURL u(localFile);
	unlink(u.path());
    }
  
    CGIServer = new KCGI;
    connect(CGIServer, SIGNAL(finished()), this, SLOT(slotCGIDone()));
  
    char filename[256];
    sprintf(filename, "%s/khelpcenterXXXXXX", _PATH_TMP);
    mktemp(filename);
    localFile.sprintf("file:%s", filename);

    CGIServer->get(_url, localFile, "Get");
    return 0;
}

KHelpView::FileType KHelpView::detectFileType(const QString &fileName)
{
    FileType type = UnknownFile;

    // attempt to identify file type
    // This is all pretty dodgey at the moment...
    if (fileName.find(".htm") > 0 && !access(fileName, 0))
    {
	return HTMLFile;
    }
    else
    {
	// open file and attempt to identify
	char fname[256];
	  
	strcpy(fname, fileName);
	if (strstr(fileName, ".gz"))
	{
	    char sysCmd[256];
	    sprintf(fname, "%s/khelpcenterXXXXXX", _PATH_TMP);
	    mktemp(fname);
	    sprintf(sysCmd, "gzip -cd %s > %s", (const char *)fileName, fname);
	    if (safeCommand(fileName))
		system(sysCmd);
	}
	QFile file(fname);
	char buf[256];
	if (file.open(IO_ReadOnly))
	{
	    for (int i = 0; !file.atEnd(), i < 80; i++)
	    {
		file.readLine(buf, 256);
		QString buffer = buf;
		if (buffer.find("<HTML>", 0, FALSE) >= 0 ||
		    buffer.find("<BODY", 0, FALSE) >= 0)
		{
		    type = HTMLFile;
		    break;
		}
		else if (buffer.find(".TH") >= 0 ||
			 buffer.find(".so") >= 0 )
		{
		    type = ManFile;
		    break;
		}
	    }
	}
	else
	    type = CannotOpenFile;
	if (strstr(fileName, ".gz"))
	    remove(fname);
    }
    return type;
}

bool KHelpView::canCurrentlyDo(AllowedActions action)
{
    switch (action)
    {
    case Copy:        return view->isTextSelected();
    case GoBack:      return ((histCurrent != 0) && (histCurrent != history.first()));
    case GoForward:   return ((histCurrent != 0) && (histCurrent != history.last()));
    case GoUp:        return false;
    case Stop:        return busy;
    default: 
	warning("KHelpView::canCurrentlyDo: missing case in \"switch\" statement\n");
	return false;
    }
    return false; // just to make the compiler happy...
}

const char *KHelpView::getCurrentURL()
{
    return (const char *) currentURL;
}

void KHelpView::addBookmark(const char *_title, const char *url)
{
    QString bmFile = KApplication::localkdedir() + "/share/apps/khelpcenter/bookmarks.html";
    bookmarkManager.add(_title, url);
    bookmarkManager.write(bmFile);
}

void KHelpView::layout()
{
    int top = 0;
    int bottom = height();
  
    bottom -= SCROLLBAR_WIDTH;

    if (view->docWidth() > view->width())
	horz->show();
    else
	horz->hide();

    if (view->docHeight() > view->height())
	vert->show();
    else
	vert->hide();
  
    if(vert->isVisible() && horz->isVisible())
    {
	view->setGeometry(0, top, width() - SCROLLBAR_WIDTH, bottom-top);
	vert->setGeometry(width()-SCROLLBAR_WIDTH, top, SCROLLBAR_WIDTH, bottom-top);
	horz->setGeometry(0, bottom, width() - SCROLLBAR_WIDTH, SCROLLBAR_WIDTH);
    }
    else if (vert->isVisible())
    {
	view->setGeometry(0, top, width() - SCROLLBAR_WIDTH, height()-top);
	vert->setGeometry(width()-SCROLLBAR_WIDTH, top, SCROLLBAR_WIDTH, height()-top);
    }
    else if (horz->isVisible())
    {
	view->setGeometry(0, top, width(), bottom-top);
	horz->setGeometry(0, bottom, width(), SCROLLBAR_WIDTH);
    }
    else
	view->setGeometry(0, top, width(), height()-top);
}

void KHelpView::slotOpenFile()
{
    QString fileName = KFileDialog::getOpenFileName();
  
    if (!fileName.isNull())
    {
	QString url = "file:";
	url += fileName;
	openURL(url);
    }
}

void KHelpView::slotReload()
{
    QString url = currentURL;
    currentURL = "";
    openURL(url, false);
}

void KHelpView::slotPrint()
{
    view->print();
}

void KHelpView::slotCopy()
{
    QString text;
  
    view->getSelectedText(text);
    QClipboard *cb = kapp->clipboard();
    cb->setText(text);
}

void KHelpView::slotFind()
{
    if (!findDialog)
    {
	findDialog = new KFindTextDialog();
	connect(findDialog, SIGNAL(find(const QRegExp &)), SLOT(slotFindNext(const QRegExp &)));
    }
  
    // reset the find iterator
    view->findTextBegin();
    findDialog->show();
}

void KHelpView::slotFindNext()
{
    if (findDialog && !findDialog->regExp().isEmpty())
    {
	slotFindNext(findDialog->regExp());
    }
    else
    {
	// no find has been attempted yet - open the find dialog.
	slotFind();
    }
}

void KHelpView::slotFindNext( const QRegExp &regExp )
{
    if (!view->findTextNext(regExp))
    {
      if (!QMessageBox::information(this, i18n("Find Complete - Help Viewer"),
				    i18n("Continue search from the top of the page?"),
				    i18n("&Yes"), i18n("&No"), 0, 1))
	{
	  view->findTextBegin();
	  slotFindNext(regExp);
	}
      else
	{
	  view->findTextEnd();
	  findDialog->hide();
	}
    }
}

void KHelpView::slotBack()
{
    history.find(histCurrent);
    KPageInfo *p = history.prev();
  
    if (p)
    {
	histCurrent = p;
	if (!openURL(p->getUrl(), false))
	{
	    scrollTo = p->getOffset();
	    if (!busy)
	    {
		view->slotScrollVert(scrollTo);
		vert->setValue(scrollTo);
	    }
	}
    }
}

void KHelpView::slotForward()
{
    history.find(histCurrent);
    KPageInfo *p = history.next();
  
    if (p)
    {
	histCurrent = p;
	if (!openURL(p->getUrl(), false))
	{
	    scrollTo = p->getOffset();
	    if (!busy)
	    {
		view->slotScrollVert(scrollTo);
		vert->setValue(scrollTo);
	    }
	}
    }
}

void KHelpView::slotDir()
{
    QString initDoc = "file:" + locate("html", "default/khelpcenter/main.html");
    openURL(initDoc);
}

void KHelpView::slotUp()
{
  
}

void KHelpView::slotTextSelected(bool)
{
    emit enableMenuItems();
    XSetSelectionOwner(qt_xdisplay(), XA_PRIMARY, handle(), CurrentTime);
}

void KHelpView::slotAddBookmark()
{
    addBookmark(title, currentURL);
}

void KHelpView::slotBookmarkSelected(int id)
{
    id -= BOOKMARK_ID_BASE;
    KFileBookmark *bm = bookmarkManager.getBookmark(id);
  
    if (bm)
	openURL(bm->getURL());
}

void KHelpView::slotBookmarkHighlighted(int id)
{
    id -= BOOKMARK_ID_BASE;
    KFileBookmark *bm = bookmarkManager.getBookmark(id);
  
    if (bm)
    {
	emit setURL(bm->getURL());
    }
    else
    {
	emit setURL(currentURL);
    }
}

void KHelpView::slotBookmarkChanged()
{
    emit bookmarkChanged(bookmarkManager.getRoot());
}

void KHelpView::slotStopProcessing()
{
    if (CGIServer)
    {
	delete CGIServer;
	KURL u(localFile);
	unlink(u.path());
	CGIServer = NULL;
	view->setCursor(oldCursor);
    }
  
    view->stopParser();
  
    busy = false;
    emit enableMenuItems();
}

void KHelpView::slotSetTitle(const char *_title)
{
    title = _title;
    emit setTitle(_title);
}

void KHelpView::slotURLSelected(const char *URL, int button)
{
    if (URL)
    {
	if (button == LeftButton)
	{
	    openURL(URL);
	    view->setFocus();
	}
	else if (button == MidButton)
	{
	    emit openNewWindow(URL);
	}
    }
}

void KHelpView::slotOnURL(const char *URL)
{
    if (URL)
    {
	emit setURL(URL);
    }
    else
	emit setURL(currentURL);	
}

void KHelpView::slotFormSubmitted(const char *_method, const char *_url, const char* _data, const char * )
{
    if (strcasecmp(_method, "GET")==0)
    {
	QString url(_url);
	url += "?";
	url += _data; 
	openURL(url);
    }
    else
	openURL(_url);
}

void KHelpView::slotPopupMenu(const char *url, const QPoint &p)
{
    int id = 0;
  
    if (rmbPopup)
	delete rmbPopup;
  
    rmbPopup = new QPopupMenu;
    rmbPopup->installEventFilter(this);
  
    id = rmbPopup->insertItem(i18n("Back"), this, SLOT(slotBack()));
    rmbPopup->setItemEnabled(id, canCurrentlyDo(GoBack));
    id = rmbPopup->insertItem(i18n("Forward"), this, SLOT(slotForward()));
    rmbPopup->setItemEnabled(id, canCurrentlyDo(GoForward));
  
    if (url)
    {
	rmbPopup->insertSeparator();
	  
	int pos;
	if (url[0] == '\"')
	    url++;
	newURL = url;
	if ((pos  = newURL.findRev('\"')) > 0)
	    newURL.truncate(pos);
	  
	rmbPopup->insertItem(i18n("Open this Link"),this,SLOT(slotPopupOpenURL()));
	rmbPopup->insertItem(i18n("Add Bookmark"),this,SLOT(slotPopupAddBookmark()));
	rmbPopup->insertItem(i18n("Open in new Window"),this,SLOT(slotPopupOpenNew()));
    }
    rmbPopup->popup(p);
}

void KHelpView::slotCGIDone()
{
    view->setCursor(oldCursor);
    KURL u(localFile);
    if (!openFile(u.path()))
	view->parse();
    emit setURL(currentURL);
    emit setLocation(currentURL); 
  
    delete CGIServer;
    CGIServer = 0;
    unlink(u.path());
}

void KHelpView::slotScrollVert(int _y)
{
    vert->setValue(_y);
}

void KHelpView::slotScrollHorz(int _x)
{
    horz->setValue(_x);
}

void KHelpView::slotBackgroundColor(const QColor &col)
{
    view->setBackgroundColor(col);
}

void KHelpView::slotFontSize(int size)
{
    fontBase = size;
    slotReload();
    busy = true;
    emit enableMenuItems();
}

void KHelpView::slotStandardFont(const QString& n)
{
    standardFont = n;
    view->setStandardFont(n);
    slotReload();
    busy = true;
    emit enableMenuItems();
}

void KHelpView::slotFixedFont(const QString& n)
{
    fixedFont = n;
    view->setFixedFont(n);
    slotReload();
    busy = true;
    emit enableMenuItems();
}

void KHelpView::slotColorsChanged(const QColor &bg, const QColor &text,
				  const QColor &link, const QColor &vlink, bool uline, bool)
{
    view->setDefaultBGColor(bg);
    view->setDefaultTextColors(text, link, vlink);
    view->setUnderlineLinks(uline);
    slotReload();
    busy = true;
    emit enableMenuItems();
}

void KHelpView::slotPopupOpenURL()
{
    openURL(newURL);
}

void KHelpView::slotPopupAddBookmark()
{
    addBookmark(newURL, newURL);
}

void KHelpView::slotPopupOpenNew()
{
    emit openNewWindow(newURL);
}

void KHelpView::slotViewResized(const QSize &)
{
    QApplication::setOverrideCursor(waitCursor);
  
    vert->setSteps(12, view->height() - 20); 
    horz->setSteps(24, view->width());

    if (view->docHeight() > view->height())
	vert->setRange(0, view->docHeight() - view->height());
    else
	vert->setRange(0, 0);
  
    QApplication::restoreOverrideCursor();
}

void KHelpView::resizeEvent(QResizeEvent *)
{
    layout();
}

// all this just so that the rmb popup menu doesn't disappear
bool KHelpView::eventFilter(QObject *obj, QEvent *ev)
{
    static QPoint tmpPoint;

    if (obj == rmbPopup)
    {
	if (ev->type() == QEvent::MouseButtonRelease)
	{
	    if (QCursor::pos() == tmpPoint)
	    {
		tmpPoint = QPoint(-10, -10);
		return true;
	    }
	}
    }
  
    if (obj == view)
    {
	switch (ev->type())
	{
	case QEvent::MouseButtonPress:
	case QEvent::MouseButtonDblClick:
	    tmpPoint = QCursor::pos();
	    break;
		  
	case QEvent::MouseButtonRelease:
	    tmpPoint = QPoint(-10,-10);
	    break;
	default:
	    break;
	}
    }
    return false;
}

bool KHelpView::x11Event(XEvent *xevent)
{
    switch (xevent->type)
    {
    case SelectionRequest:
    {
	if (view->isTextSelected())
	{
	    QString text;
	    view->getSelectedText(text);
	    XSelectionRequestEvent *req = &xevent->xselectionrequest;
	    XEvent evt;
	    evt.xselection.type = SelectionNotify;
	    evt.xselection.display  = req->display;
	    evt.xselection.requestor = req->requestor;
	    evt.xselection.selection = req->selection;
	    evt.xselection.target = req->target;
	    evt.xselection.property = None;
	    evt.xselection.time = req->time;
	    if (req->target == XA_STRING)
	    {
		XChangeProperty (qt_xdisplay(), req->requestor,
				 req->property, XA_STRING, 8, PropModeReplace,
				 (uchar *)text.data(), text.length());
		evt.xselection.property = req->property;
	    }
	    XSendEvent(qt_xdisplay(), req->requestor, False, 0, &evt);
	}
		
	return true;
    }
    break;
	  
    case SelectionClear:
	// Do we want to clear the selection???
	view->selectText(0, 0, 0, 0);
	break;
    }
    return false;
}

// called as html is parsed
void KHelpView::slotDocumentChanged()
{
    if (view->docHeight() > view->height())
	vert->setRange(0, view->docHeight() - view->height());
    else
	vert->setRange(0, 0);
  
    if (view->docWidth() > view->width())
	horz->setRange(0, view->docWidth() - view->width());
    else
	horz->setRange(0, 0);
}

void KHelpView::slotDocumentStarted()
{
    emit setBusy(true);
}
// called when all html has been parsed
void KHelpView::slotDocumentDone()
{
    if (scrollTo)
    {
	if (scrollTo > view->docHeight() - view->height())
	    scrollTo = view->docHeight() - view->height();
	view->slotScrollVert(scrollTo);
	vert->setValue(scrollTo);
    }
    else if (!ref.isEmpty())
    {
	if (!view->gotoAnchor(ref))
	    vert->setValue(0);
    }
  
    layout();
    busy = false;
    emit enableMenuItems();
    emit setBusy(false);
}

// Is this a safe command to issue via system, i.e. has a naughty user inserted
// special shell characters in a URL?
bool KHelpView::safeCommand(const char *cmd)
{
    if (strpbrk(cmd, "&;`'\\\"|*?~<>^()[]{}$\n\r"))
	return false;
  
    return true;
}

void KHelpView::setDefaultFontBase(int fSize)
{
    view->resetFontSizes();
    if (fSize != 3)
    {
	int fontSizes[7];
	view->getFontSizes(fontSizes);

	if (fSize > 3)
	{
	    for(int i = 0; i < 7; i++)
	    {
		int j = i + fSize - 3;
		if ( j > 6)
		    j = 6;
		fontSizes[i] = fontSizes[j]; 
	    }
	}
	else
	{
	    for(int i = 7; i--;)
	    {
		int j = i + fSize - 3;
		if ( j < 0)
		    j = 0;
		fontSizes[i] = fontSizes[j]; 
	    }
	}
	view->setFontSizes(fontSizes);
    }
}
