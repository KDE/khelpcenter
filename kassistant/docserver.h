#ifndef __DOCSERVER_H
#define __DOCSERVER_H

#include <khtml.h>
#include <khtmlview.h>
#include "cgi.h"

class KDocumentServer : public QObject {
	 Q_OBJECT

public:
	enum requestType { FILE, WIDGET, CGIBIN, COMMAND, POPUP, UNKNOWN, ERROR };

	KDocumentServer();	

	/*
	 *  processes document requests. if you want to add another type,
	 *  just add another entry here and a method to process it.
	 */
	void request( KHTMLView *_view, const char *_url );

protected:
	/*
	 *  highlight the widget given by the identifier
	 */
	void widget( const char *identifier );

	/*
	 *  processes url 'file:/...'
	 */
	void file( KHTMLView *_view, const char *_url );

	/*
	 *  process a CGI query
	 *
	 */
	void CGI( KHTMLView *_view, const char *_url );

	/*
	 *  execute a command
	 */
	void command( const char *_url );

	KCGI *CGIServer;
	char filename[256];
	QString localFile;
	KHTMLView *view;

signals:
	void requestDone( KDocumentServer::requestType type );

private slots:
	void slotCGIDone();

};

#endif
