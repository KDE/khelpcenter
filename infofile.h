/*
 *  khc_infofile.h - part of the KDE Help Center
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

#ifndef __KHC_INFO_FILE__
#define __KHC_INFO_FILE__

#include <qstring.h>
#include <qstringlist.h>

#include "infonode.h"
#include "infoconsts.h"

class khcInfoFile
{
public:
  /** 
   * The constructor. The parameters have not to be specified here, but they must be specified before any reading is done.
   * @param sName actual name of the info file to be read (e.g. "/usr/share/info/as.info.bz2")
   * @param sTopic topic the file belongs to (e.g. "as")
   */ 
  khcInfoFile(QString sName = "", QString sTopic = "") : m_sName(sName), m_sTopic(sTopic), m_bInitialized(false) { }
  
  QString name() const { return m_sName; }
  void setName(QString sName) { m_sName = sName; }
  QString topic() const { return m_sTopic; }
  void setTopic(QString sTopic) { m_sTopic = sTopic; }

  /** 
   * Initializes the object. The function has not to be called explicitly from
   * outside; it is automatically called when necessary.
   * @returns @li ERR_NONE on success
   *          @li ERR_FILE_UNAVAILABLE if file m_sName doesn't exist or cannot
   *              be read.
   */
  uint init();
  
  /**
   * Reads the indirect table (if it exists) and stores the names of the
   * files contained in it to the supplied list lFileNames.
   * @returns @li ERR_NONE on success
   *          @li ERR_NO_IND_TABLE if the indirect table doesn't exist
   *          @li ERR_FILE_UNAVAILABLE if file m_sName doesn't exist or cannot
   *              be read.
   */
  uint getIndirectTable(QStringList& lFileNames);
  /** Retrieves the next node contained in the file.
   * @returns @li ERR_NONE on success
   *          @li ERR_NO_MORE_NODES if all nodes have been already read
   *          @li ERR_FILE_UNAVAILABLE if file m_sName doesn't exist or cannot
   *              be read.
   */
  uint getNextNode(khcInfoNode* pNode, uint flags);

protected:
  /** 
   * Reads file m_sName into m_sFileContents.
   * @returns @li ERR_NONE on success
   *          @li ERR_FILE_UNAVAILABLE if file m_sName doesn't exist or cannot
   *              be read.
   */
  uint read();

  QString m_sName;
  QString m_sTopic;
  QString m_sFileContents;
  int m_nSearchStartIdx; // index (in m_sFileContents) from which we should
                         // begin looking for the next node
  bool m_bInitialized;
};

#endif // __KHC_INFO_FILE__
