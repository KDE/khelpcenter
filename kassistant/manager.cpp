/*
    This file is part of the KDE Help Center

    Copyright (C) 1998 René Beutler (rbeutler@g26.ethz.ch)
*/


#include <komApplication.h>
#include <qintdict.h>
#include "manager.h"

Context::Context()
{
	history = new QStrList;
	base = new QString;
	current = new QString;
}

ContextManager::ContextManager( TopLevel top )
{
	CHECK_PTR( top );
	toplevel = top;
	
	currentID = maxID = 0;

	dict.setAutoDelete( TRUE );
}


unsigned long ContextManager::registerApp( QString name )
{
	unsigned long id = maxID;
	maxID++;

	struct Context *context = new Context;	

        String path = KApplication::kde_htmldir().copy() + "/";
        QString dir = path + klocale->language() + "/" + name;
        if( QFileInfo( file ).isDir && QFileInfo( file ).isReadable() )
                path += klocale->language() + name;
        else
                path += "default" + name;

	context->base = "file:" + path + "/";

        dict.insert( id, context );

	return id;
}

void ContextManager::removeApp( unsigned long id )
{
	dict.remove( id );	
}

void ContextManager::activate( unsigned long id )
{
	if( currentID != id )
	{
		currentID = id;
		Context *context = dict[id];
		if( context != 0L )
		{
			toplevel->setBaseDir( context->base );
			toplevel->setURL( context->current );
		}
	}
}