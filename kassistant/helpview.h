#ifndef __KHELPVIEW_H
#define __KHELPVIEW_H

#include <kquickhelp.h>
#include <html.h>
#include "docserver.h"

class KHelpView : public KHTMLView {
	Q_OBJECT

public:
	KHelpView( QWidget *parent=0, const char *name=0, int flags = 0, KHTMLView *_parent_view = 0L,  int _defaultFontBase = 3 );
	virtual ~KHelpView();
	void setBaseDir( QString s );
	void connectSignals( QWidget *w );

	/**
	 * Overrides @ref KHTMLView::newView. It just creates a new instance
	 * of KHelpView. These instances are used as frames.
	 */
	virtual KHTMLView* newView( QWidget *_parent = 0L, const char *_name = 0L, int _flags = 0L );
	virtual bool mousePressedHook( const char* _url, const char *_target, QMouseEvent *_ev, bool _isselected );
	virtual bool dndHook( const char *_url, QPoint &_p );
	void begin( const char *_url, int _x_offset, int _y_offset );
	
	/**
	 * Sets the base font size ( range: 2-5, default: 3 ).
	 * Note that font sizes are not defined in points. Font sizes range from 1 (smallest) to 7 (biggest).
	 */
	void setDefaultFontBase( int size );

private:
	/**
	 * A list containing all direct child views. Usually every frame in a frameset becomes
	 * a child view. This list is used to update the contents of all children ( see @ref #slotUpdateView ).
	 *
	 * @see newView
	 */
	QList<KHelpView> childViewList;

	/*
	 *  default font base size ( range: 2-5, default 3)
	 *  Note that font sizes are not defined in points. Font sizes range from 1 (smallest) to 7 (biggest).
	 */
	int defaultFontBase;

	/*
	 *  Server for Document Requests
	 */
	KDocumentServer *docServer;

	/*
	 *
	 */
	KQuickHelpWindow *quickHelp;

	QString baseDir;

signals:
	void URLSelected( const char *url );
	void documentDone( KDocumentServer::requestType type );
	void setTitle( const char *title );

private slots:
	void slotDocumentRequest( KHTMLView *_view, const char *_url );
	void slotURLSelected    ( KHTMLView *_view, const char *_url, int _button, const char *_target );
	void slotFormSubmitted  ( const char *_method, const char *_url, const char *_data );  
	void slotDocumentRequestDone( KDocumentServer::requestType type );
};

#endif __KHELPVIEW_H
