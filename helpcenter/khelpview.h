/*
 *  khelpview.cpp - part of the KDE Help Center
 *
 *  Copyright (c) 1999 Matthias Elter (me@kde.org)
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

#ifndef __khelpview_h__
#define __khelpview_h__

#include <qwidget.h>
#include <qdialog.h>
#include <qpopupmenu.h>
#include <qscrollbar.h>
#include <qlabel.h>
#include <qcursor.h>
#include <qlist.h>

#include <khtml.h>
#include <kfilebookmark.h>

#include "cgi.h"
#include "misc.h"
#include "man.h"
#include "finddlg.h"
#include "fmthtml.h"

#define SCROLLBAR_WIDTH		16
#define BOOKMARK_ID_BASE	200

// KPageInfo: Class for history entries
class KPageInfo
{
 public:
  KPageInfo(const char *u, int y) {url = u; yOffset = y;}
  KPageInfo(const KPageInfo &i) {url = i.url.copy(); yOffset = i.yOffset;}
  
  const QString getUrl() const {return url;}
  int getOffset() const {return yOffset;}
  
  void setOffset(int y) {yOffset = y;}
  
 private:
  QString url;
  int yOffset;
};

class KHelpView : public QWidget
{
  Q_OBJECT
 public:
  enum AllowedActions { Copy, GoBack, GoForward, GoUp, Stop };
  
  KHelpView(QWidget *parent=NULL, const char *name=NULL);
  virtual ~KHelpView();
  
  int openURL( const char *URL, bool withHistory = true );
  
  bool canCurrentlyDo(AllowedActions action);
  const char *getCurrentURL();
  
  const QList<KPageInfo> &getHistory() const { return history; }
  void setHistory( const QList<KPageInfo> &hist ) {   history = hist; }
  
 signals:
  void enableMenuItems();
  void openNewWindow(const QString& newURL);
  
  void setURL(const QString& url);  
  void setLocation(const QString& url);
  void bookmarkChanged(KFileBookmark *);
  void setTitle(const QString& _title);
  
 public slots:
  void slotOpenFile();
  void slotReload();
  void slotPrint();
  
  void slotCopy();
  void slotFind();
  void slotFindNext();
  void slotFindNext( const QRegExp & );
  void slotBack();
  void slotForward();
  void slotDir();
  void slotUp();
  void slotTextSelected( bool sel );
  void slotAddBookmark();
  void slotBookmarkSelected( int id );
  void slotBookmarkHighlighted( int id );
  void slotBookmarkChanged();
  void slotStopProcessing();
  void slotSetTitle( const char * );
  void slotURLSelected( const char *, int );
  void slotOnURL( const char * );
  void slotFormSubmitted( const char *, const char *, const char *, const char * );
  void slotPopupMenu( const char *, const QPoint & );
  void slotCGIDone();
  void slotScrollVert( int _y );
  void slotScrollHorz( int _y );
  void slotBackgroundColor( const QColor &col );
  void slotFontSize( int );
  void slotStandardFont( const QString& );
  void slotFixedFont( const QString& );
  void slotColorsChanged( const QColor&, const QColor&, const QColor&,
						  const QColor&, const bool, const bool );
  void slotPopupOpenURL();
  void slotPopupAddBookmark();
  void slotPopupOpenNew();
  void slotViewResized( const QSize & );
  void slotDocumentChanged();
  void slotDocumentDone();
  
 protected:
  virtual void resizeEvent( QResizeEvent * );
  
  virtual bool eventFilter( QObject *, QEvent * );
  virtual bool x11Event( XEvent * );
  
 private:
  enum FileType { UnknownFile, HTMLFile, InfoFile, ManFile, CannotOpenFile };
  
  int openFile( const QString & );
  int formatMan( int bodyOnly = FALSE );
  int openHTML( const char *location );
  int runCGI( const char *_url );
  FileType detectFileType( const QString &filename );
  void enableToolbarButton( int id, bool enable );
  void createMenu();
  void addBookmark( const char *_title, const char *url );
  void layout();
 private:
  QScrollBar *vert;
  QScrollBar *horz;
  QLabel *statusBar;
  QPopupMenu *rmbPopup;
  KHTMLWidget *view;
  KFindTextDialog *findDialog;
  
  QString localFile;
  KCGI *CGIServer;
  KFileBookmarkManager bookmarkManager;
  
  // html view preferences
  int  fontBase;
  QString standardFont;
  QString fixedFont;
  
  QColor bgColor;
  QColor textColor;
  QColor linkColor;
  QColor vLinkColor;
  bool   underlineLinks;
  bool   forceDefaults;
  
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
  
  QList<KPageInfo> history;
  KPageInfo *histCurrent;
  cHTMLFormat html;
  cMan *man;
  cHelpFormatBase *format;
  
  QString newURL;
};

#endif

