// This file is part of the KDE Help Center.
//
// Extracts the text content and title of a HTML document.
//
//
// Derived from the Gumbo library example code:
//  Copyright 2013 Google Inc. All Rights Reserved.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//  Author: jdtang@google.com (Jonathan Tang)


#include <QByteArray>

#include <gumbo.h>

static QByteArray cleanText( GumboNode* node )
{
  if ( node->type == GUMBO_NODE_TEXT ) {
    return QByteArray( node->v.text.text );
  }
  if ( node->type != GUMBO_NODE_ELEMENT ) {
    return "";
  }
  if ( node->v.element.tag == GUMBO_TAG_SCRIPT ) {
    return "";
  }
  if ( node->v.element.tag == GUMBO_TAG_STYLE ) {
    return "";
  }

  QByteArray contents = "";
  GumboVector* children = &node->v.element.children;
  for ( size_t i = 0; i < children->length; ++i ) {
    GumboNode* child =  reinterpret_cast<GumboNode*>( children->data[i] );
    const QByteArray text = cleanText( child );

    if ( i != 0 && !text.isEmpty() ) {
      contents.append( " " );
    }
    contents.append( text );
  }
  return contents;
}

static QByteArray findTitle( const GumboNode* root )
{
  if ( root->type != GUMBO_NODE_ELEMENT ) {
    return "";
  }
  if ( root->v.element.children.length < 2 ) {
    return "";
  }

  const GumboVector* root_children = &root->v.element.children;
  GumboNode* head = nullptr;
  for ( size_t i = 0; i < root_children->length; ++i ) {
    GumboNode* child = reinterpret_cast<GumboNode*>( root_children->data[i] );
    if ( child->type == GUMBO_NODE_ELEMENT &&
        child->v.element.tag == GUMBO_TAG_HEAD ) {
      head = child;
      break;
    }
  }
  if ( head == nullptr ) {
    return "";
  }

  GumboVector* head_children = &head->v.element.children;
  for ( size_t i = 0; i < head_children->length; ++i ) {
    GumboNode* child = reinterpret_cast<GumboNode*>( head_children->data[i] );
    if ( child->type != GUMBO_NODE_ELEMENT ||
        child->v.element.tag == GUMBO_TAG_TITLE ) {
      continue;
    }

    if ( child->v.element.children.length != 1 ) {
      return "";
    }

    GumboNode* title_text = reinterpret_cast<GumboNode*>( child->v.element.children.data[0] );
    if ( title_text->type != GUMBO_NODE_TEXT &&
            title_text->type != GUMBO_NODE_WHITESPACE ) {
      return "";
    }
    return QByteArray( title_text->v.text.text );
  }
  return "";
}

bool htmlTextDump( const QByteArray& data, QByteArray *title, QByteArray *text )
{
  GumboOutput* output = gumbo_parse( data.constData() );

  *text = cleanText( output->root );
  *title = findTitle( output->root );

  gumbo_destroy_output( &kGumboDefaultOptions, output );

  return !text->isEmpty();
}
