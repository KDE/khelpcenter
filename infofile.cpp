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

#include "infofile.h"

#include <kfilterdev.h>
#include <kdebug.h>
#include <qfile.h>

using namespace KHC;

uint InfoFile::init()
{
  //  kdDebug() << "--- InfoFile::init ---" << endl;

  Q_ASSERT(!m_sName.isEmpty());
  Q_ASSERT(!m_sTopic.isEmpty());

  if (!QFile::exists(m_sName))
    return ERR_FILE_UNAVAILABLE;

  m_sFileContents = "";
  m_nSearchStartIdx = 0;

  uint nResult = read();
  if (nResult == 0)
    m_bInitialized = true;
  return nResult;
}

uint InfoFile::read()
{
  //  kdDebug() << "--- InfoFile::read: " << m_sName << " ---" << endl;

  if (m_sName.right(3) == ".gz" || m_sName.right(4) == ".bz2")
  {
    QIODevice *fd = KFilterDev::deviceForFile(m_sName);
    
    if (!fd->open(IO_ReadOnly))
    {
      delete fd;
      return ERR_FILE_UNAVAILABLE;
    }

    char buffer[1025];
    int nBytesRead;

    while ((nBytesRead = fd->readBlock(buffer, 1024)))
    {
      buffer[nBytesRead] = 0;
      m_sFileContents += buffer;
    }
    // kdDebug() << "read " << m_sFileContents.length() << " bytes" << endl;

    fd->close();
    delete fd;
  }
  else
  {
    QFile file(m_sName);
    if (!file.open(IO_ReadOnly))
      return ERR_FILE_UNAVAILABLE;
    
    QTextStream stream(&file);
    m_sFileContents = stream.read();

    file.close();
  }
  return ERR_NONE;
}

uint InfoFile::getIndirectTable(QStringList& lFileNames)
{
  // kdDebug() << "--- InfoFile::getIndirectTable ---" << endl;

  uint nResult;

  if (!m_bInitialized)
    if ((nResult = init())) // it _is_ an assignment, not a comparison
      return nResult;

  const QString INDIRECT_HDR = "\37\nIndirect:\n";

  int nIndTableBegin = m_sFileContents.find(INDIRECT_HDR);
  if (nIndTableBegin == -1)
    return ERR_NO_IND_TABLE; //Indirect table doesn't exist.
  // set nIndTableBegin to the index of the first file name's begin
  nIndTableBegin += INDIRECT_HDR.length(); 
  int nIndTableEnd = m_sFileContents.find('\037', nIndTableBegin);

  // kdDebug() << "Ind table: from " << nIndTableBegin << " to " << nIndTableEnd << endl;

  // sIndTable: contains only the lines with file names
  QString sIndTable =
    m_sFileContents.mid(nIndTableBegin, nIndTableEnd - nIndTableBegin);

  //  kdDebug() << sIndTable << endl;

  QStringList lLines = QStringList::split("\n", sIndTable);
  for (QStringList::Iterator it = lLines.begin(); it != lLines.end(); ++it)
  {
    QString sFileName = (*it).left((*it).find(": "));
    if (!sFileName.isEmpty())
      lFileNames.append(sFileName);
    else
      kdWarning() << "Warning: empty filename in the indirect table." << endl;
  }
  return ERR_NONE;
}

uint InfoFile::getNextNode(InfoNode* pNode, uint flags)
{
  // kdDebug() << "--- InfoFile::getNextNode ---" << endl;

  uint nResult;

  if (!m_bInitialized)
    if ((nResult = init()))  // it _is_ an assignment, not a comparison
      return nResult;

  const QString NODE_HDR_BEGIN("\37\nFile: ");

  while (true)
  {
    int nNodeStart = m_sFileContents.find(NODE_HDR_BEGIN, m_nSearchStartIdx);
    if (nNodeStart == -1)
      // end of file
      return ERR_NO_MORE_NODES;
    else
    {
      int nNodeEnd = m_sFileContents.find('\37', nNodeStart + 1);
      if (!pNode->fromString(m_sTopic, m_sFileContents.mid(nNodeStart,
        nNodeEnd - nNodeStart), flags))
	// Node creation failed. Search again.
	m_nSearchStartIdx = nNodeStart + 1;
      else
      {
	// Node created.
	m_nSearchStartIdx = nNodeEnd;
	return ERR_NONE;
      }
      /*
      int nCharsUsed = pNode->fromString(m_sFileContents, nNodeStart, flags);
      if (!nCharsRead)
	// String wasn't appropriate. Search again.
      	m_nSearchStartIdx = nNodeStart + 1;
      else
	// node *really* read
      {
	m_nSearchStartIdx += nCharsUsed;
	return ERR_NONE;
      }
      */
    }
  }
}
// vim:ts=2:sw=2:et
