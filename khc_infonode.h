/*
 *  khc_infonode.h - part of the KDE Help Center
 *
 *  Copyright (C) 2001 Wojciech Smigaj (achu@klub.chip.pl)
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

#ifndef __KHC_INFONODE_H__
#define __KHC_INFONODE_H__

#include <qstring.h>
#include <list>
#include <regex.h>

/** 
 * Stores information about an info node.
 *
 * @author Wojciech Smigaj <achu@klub.chip.pl>
 */
class khcInfoNode
{
public:
  khcInfoNode();
  ~khcInfoNode();
  /** 
   * Fills the node basing on a fragment of info file (stored in a string).
   * @param topic topic the node belongs to
   * @param str string - fragment of the file, containing exactly one node
   *        from the beginning ^_ character to - but not including - the
   *        ending one
   * @param flags flags specifying which information should be retrieved and
   *        stored in the internal variables. It can be any combination of
   *        the following:
   *        @li RETRIEVE_NAME: retrieve node's name
   #        @li RETRIEVE_TITLE: retrieve node's title (if it has one)
   *        @li RETRIEVE_NEIGHBOURS: retrieve names of the previous, next
   *            and upper nodes (in the info file hierarchy) 
   *        @li RETRIEVE_CONTENTS: retrieve node's text
   * @returns true if node was succesfully filled or false otherwise.
   */
  bool fromString(QString topic, QString str, uint flags);
  bool isTop() const
    { return m_sUp.lower() == "(dir)"; }
  bool isMyParent(const khcInfoNode& otherNode) const
    { return otherNode.m_sName == m_sUp; }
  void dumpChildren(unsigned int nLevel) const;
  void deleteChildren();
  void clear();

  QString m_sTopic, m_sName, m_sTitle, m_sContents;
  QString m_sUp, m_sPrev, m_sNext;
  list<khcInfoNode*> m_lChildren;

protected:
  bool fromHdrLine(QString sLine, uint flags);

  static void compileRegEx();
  static bool ms_bRegExCompiled;
  static regex_t ms_compRegEx; // compiled regular expression, used to retrieve
                               // information from the node string
  static uint ms_nExistingNodes; // stores number of khcInfoNode objects existing at the moment 
};

#endif
