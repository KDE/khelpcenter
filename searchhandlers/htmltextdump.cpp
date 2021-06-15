/*
    SPDX-FileCopyrightText: 2016 Pino Toscano <pino@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "htmltextdump.h"

#include <QLoggingCategory>

#include <libxml/HTMLparser.h>

namespace {

Q_LOGGING_CATEGORY( LOG, "org.kde.khelpcenter.xapian.htmltextdump", QtWarningMsg )

class HtmlDocPtr {
  public:
    HtmlDocPtr( htmlDocPtr doc ) : _doc( doc ) {}
    ~HtmlDocPtr() { xmlFreeDoc( _doc ); }
    operator bool() const { return _doc; }
    operator htmlDocPtr() const { return _doc; }

  private:
    htmlDocPtr _doc;
};

}

static xmlNode* findChildElement( xmlNode *node, const char *name )
{
  for ( xmlNode *n = node; n; n = n->next ) {
    if ( n->type == XML_ELEMENT_NODE && xmlStrcmp( n->name, BAD_CAST name ) == 0 ) {
      return n->children;
    }
  }
  return nullptr;
}

static void collectText( xmlNode *node, QByteArray *text )
{
  for ( xmlNode *n = node; n; n = n->next ) {
    if ( n->type == XML_TEXT_NODE ) {
      xmlChar *content = xmlNodeGetContent( n );
      *text += QByteArray( " " ) + QByteArray( reinterpret_cast<char *>( content ) );
      xmlFree( content );
    }
    collectText( n->children, text );
  }
}

bool htmlTextDump( const QByteArray& data, QByteArray *title, QByteArray *text )
{
  HtmlDocPtr doc( htmlReadMemory( data.constData(), data.length(), nullptr, "UTF-8", HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING | HTML_PARSE_NONET ) );
  if ( !doc ) {
    qCWarning(LOG) << "cannot parse html";
    return false;
  }

  xmlNode *root = xmlDocGetRootElement( doc );
  if ( !root ) {
    qCWarning(LOG) << "missing root";
    return false;
  }
  xmlNode *html = findChildElement( root, "html" );
  if ( !html ) {
    qCWarning(LOG) << "missing <html>";
    return false;
  }
  xmlNode *head = findChildElement( html, "head" );
  xmlNode *body = findChildElement( html, "body" );
  if ( !body ) {
    qCWarning(LOG) << "missing <body>";
    return false;
  }

  QByteArray newText;
  collectText( body, &newText );
  *text = newText;

  if ( head ) {
    xmlNode *title_node = findChildElement( head, "title" );
    if ( title_node ) {
      QByteArray newTitle;
      collectText( title_node, &newTitle );
      *title = newTitle;
    }
  }

  return true;
}
