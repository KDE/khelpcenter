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

#include <qdir.h>
#include <qregexp.h>

#include <kdebug.h>

#include "infoconsts.h"

#include "inforeader.h"

khcInfoReader::khcInfoReader(QString sTopic) :
  m_sTopic(sTopic), m_bInitialized(false)
{
  // kdDebug() << "--- khcInfoReader::khcInfoReader ---" << endl;

  m_lFiles.setAutoDelete(true); 
}

uint khcInfoReader::getNextNode(khcInfoNode* pNode, uint flags)
{
  // kdDebug() << "--- khcInfoReader::getNextNode ---" << endl;

  uint nResult;

  Q_ASSERT(!m_sTopic.isEmpty());
  Q_ASSERT(pNode);
  
  if (!m_bInitialized)
    if ((nResult = init()))
      return nResult;

  khcInfoFile* pFile = 0;
  while (true) 
  {
    pFile = m_lFiles.current();
    if (!pFile)
      return ERR_NO_MORE_NODES;
    uint nResult = pFile->getNextNode(pNode, flags);
    switch (nResult)
    {
    case ERR_NONE:
      return ERR_NONE;
    case ERR_NO_MORE_NODES:
      m_lFiles.next();
      break;
    default:
      return nResult;
    }
  }
}

uint khcInfoReader::init()
{
  // kdDebug() << "--- khcInfoReader::init ---" << endl;

  uint nResult; // it will store results from various function calls  

  QString mainFileName;
  if (!getRealFileName(m_sTopic, mainFileName))
    return ERR_FILE_UNAVAILABLE;

  khcInfoFile* pMainFile = new khcInfoFile(mainFileName, m_sTopic);

  QStringList lFileNames;
  nResult = pMainFile->getIndirectTable(lFileNames);
  switch (nResult)
  {
  case ERR_NONE: // the indirect table exists
    {
//        // get the path of the files
//        QFileInfo mainFileInfo(mainFileName);
//        QString path = mainFileInfo.dirPath(true);

      // kdDebug() << "Indirect Table: appending to filelist:" << endl;
      for (QStringList::Iterator it = lFileNames.begin();
	   it != lFileNames.end(); ++it)
      {
	// kdDebug() << (const char*) (path + "/" + *it) << endl;
	QString realName;
	if (!getRealFileName(*it, realName))
	{
	  delete pMainFile;
	  return ERR_FILE_UNAVAILABLE;
	}
	// kdDebug() << realName << endl;
	m_lFiles.append(new khcInfoFile(realName, m_sTopic));
      }
      delete pMainFile;
      break;
    }
  case ERR_NO_IND_TABLE: // it doesn't
    m_lFiles.append(pMainFile);
    break;
  default:
    delete pMainFile;
    return nResult;
  }

  m_lFiles.first(); // make the first item the current item
  m_bInitialized = true;

  return ERR_NONE;
}

/* This function could be implemented far better and probably more efficient, 
   but it is written so that it searched for info files in exactly the same
   order the kde-info2html's FindFile function does. */
bool khcInfoReader::getRealFileName(QString baseName, QString& realName)
{
  // kdDebug() << "--- khcInfoReader::getRealFileName: " << baseName << " ---" << endl;

  Q_ASSERT(!baseName.isEmpty());

  QString sPossibleNames[6] = { baseName };
  QRegExp reEndsWithInfo("^(.+)\\.info$");
  if (reEndsWithInfo.exactMatch(baseName))
    sPossibleNames[3] = baseName.left(baseName.length() - 5);
  else
    sPossibleNames[3] = baseName + ".info";

  sPossibleNames[1] = sPossibleNames[0] + ".gz";
  sPossibleNames[2] = sPossibleNames[0] + ".bz2";

  sPossibleNames[4] = sPossibleNames[3] + ".gz";
  sPossibleNames[5] = sPossibleNames[3] + ".bz2";
  
  // kdDebug() << "possible names: " <<
  // sPossibleNames[0] << ", " << sPossibleNames[3] << endl;

  for (unsigned int i = 0; i <= 3; i+= 3)
    for (unsigned int j = 0; j < INFODIRS; j++)
      for (unsigned int k = 0; k <= 2; k++)
	if (QFile::exists(INFODIR[j] + sPossibleNames[i + k]))
	{
	  realName = INFODIR[j] + sPossibleNames[i + k];
	  // kdDebug() << "realName: " << realName << endl;
	  return true;
	}

  return false;
}
