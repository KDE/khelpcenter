 /*
    This file is part of the KDE Help Center

    Copyright (C) 1998 René Beutler (rbeutler@g26.ethz.ch)
*/


#ifndef __CONTEXTMANAGER_H
#define __CONTEXTMANAGER_H

#include <qrect.h>
#include <qintdict.h>
#include <qstrlist.h>
#include "toplevel.h"


class Context {
public:
	Context();
	QStrList *history;
	QString *base, *current;
	QRect *rect;
	Window wid;	
};


class ContextManager {

public:
	ContextManager( TopLevel top );

	unsigned long registerApp( QString name );
	void removeApp( unsigned long id );
	void activate( unsigned long id );

private:
	TopLevel *toplevel;
	unsigned long currentID, maxID;
	QIntDict<Context> dict;
};


#endif
