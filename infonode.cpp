/*
 *  This file is part of the KDE Help Center
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

#include "infonode.h"

#include <map>
// #include <qptrlist.h>
#include <qstringlist.h>
#include <stdio.h>
#include <kdebug.h>

#include "infoconsts.h"

// TODO: metoda clear() - ustawiajaca wszystkie zmienne na true;
bool khcInfoNode::ms_bRegExCompiled = false;
regex_t khcInfoNode::ms_compRegEx;
uint khcInfoNode::ms_nExistingNodes = 0;

khcInfoNode::khcInfoNode()
{
  if (!ms_bRegExCompiled)
    compileRegEx();
  ms_nExistingNodes++;
}

khcInfoNode::~khcInfoNode()
{
  deleteChildren();

  if (!(--ms_nExistingNodes))
  {
    regfree(&ms_compRegEx);
    ms_bRegExCompiled = false;
  }
}

void khcInfoNode::deleteChildren()
{
  while (!m_lChildren.empty())
  {
    // kdDebug("Deleting children...");
    delete (*m_lChildren.begin());
    m_lChildren.pop_front();
  }
}

void khcInfoNode::clear()
{
  m_sTopic = m_sName = m_sTitle = m_sContents = m_sUp = m_sPrev = m_sNext = "";
  deleteChildren();
}

void khcInfoNode::compileRegEx()
{
  // kdDebug() << "--- khcInfoNode::compileRegEx ---" << endl;

  int nResult= regcomp(&ms_compRegEx,
    "^\37\n([^\n]+)\n\n(([^\n]+)\n[-=\\*\\. ]+\n\n)?\n*(.*)", REG_EXTENDED);
  Q_ASSERT(!nResult);

  ms_bRegExCompiled = true;
}

bool khcInfoNode::fromString(QString topic, QString str, uint flags)
{
  // kdDebug() << "--- khcInfoNode::fromString ---" << endl;

  Q_ASSERT(!topic.isEmpty() && !str.isEmpty());
  m_sTopic = topic;

  regmatch_t* pRegMatch = new regmatch_t[ms_compRegEx.re_nsub + 1];
  Q_CHECK_PTR(pRegMatch);

  int nResult = regexec(&ms_compRegEx, str.latin1(),
			ms_compRegEx.re_nsub + 1, pRegMatch, 0);
  if (nResult)
  {
    kdWarning() << "Couldn't parse node; regexec() returned " << nResult << endl;
    delete[] pRegMatch;
    return false;
  }

  Q_ASSERT(pRegMatch[0].rm_so == 0 && pRegMatch[0].rm_eo == str.length());

  if (!fromHdrLine(str.mid(pRegMatch[1].rm_so,
			   pRegMatch[1].rm_eo - pRegMatch[1].rm_so), flags))
  {
    delete[] pRegMatch;
    return false;
  }

  if (flags & RETRIEVE_TITLE)
    m_sTitle = str.mid(pRegMatch[3].rm_so,
		       pRegMatch[3].rm_eo - pRegMatch[3].rm_so);

  if (flags & RETRIEVE_CONTENTS)
    m_sContents = str.mid(pRegMatch[4].rm_so,
			  pRegMatch[4].rm_eo - pRegMatch[4].rm_so);

  delete[] pRegMatch;
  return true;
}

bool khcInfoNode::fromHdrLine(QString sLine, uint flags)
{
  //  kdDebug() << "--- khcInfoNode::fromHdrLine ---" << endl;

  Q_ASSERT(!sLine.isEmpty());

  // Lista bedzie zawierac ciagi typu "Node: xxx"
  QStringList itemsList = QStringList::split(",  ", sLine);
  // Mapa bedzie zawierac elementy typu "Node" => "xxx"
  std::map<QString, QString> valuesMap;
  for (QStringList::Iterator listIt = itemsList.begin();
       listIt != itemsList.end(); ++listIt)
  {
    QStringList tempList = QStringList::split(": ", *listIt);
    if (tempList.count() == 2)
      valuesMap.insert(std::pair<QString, QString>(tempList[0], tempList[1]));
  }

  bool bNameFound = false;

  for (std::map<QString, QString>::iterator mapIt = valuesMap.begin();
       mapIt != valuesMap.end(); ++mapIt)
  {
    if (mapIt->first == "Node")
    {
      bNameFound = true;
      if (flags & RETRIEVE_NAME)
	m_sName = mapIt->second;
      continue;
    }
    if (flags & RETRIEVE_NEIGHBOURS && mapIt->first == "Up")
    {
      m_sUp = mapIt->second;
      continue;
    }
    if (flags & RETRIEVE_NEIGHBOURS &&
	(mapIt->first == "Prev" || mapIt->first == "Previous"))
    {
      m_sPrev = mapIt->second;
      continue;
    }
    if (flags & RETRIEVE_NEIGHBOURS && mapIt->first == "Next")
    {
      m_sNext = mapIt->second;
      continue; // unnecessary, but looks better...
    }
  }
  if (!bNameFound)
    return false;
  return true;
}

void khcInfoNode::dumpChildren(unsigned int nLevel) const
{
  for (std::list<khcInfoNode*>::const_iterator it = m_lChildren.begin();
       it != m_lChildren.end(); ++it)
  {
    QString sTabs;
    sTabs.fill('\t', nLevel);
    kdDebug() << sTabs <<
      "Node: " << (*it)->m_sName <<
      " Up: " << (*it)->m_sUp <<
      " Prev: " << (*it)->m_sPrev <<
      " Next: " << (*it)->m_sNext << endl;
    (*it)->dumpChildren(nLevel + 1);
  }
}
