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

#ifndef __KHC_INFOREADER__
#define __KHC_INFOREADER__

#include <qstring.h>
#include <qptrlist.h>

#include "infonode.h"
#include "infoconsts.h"
#include "infofile.h"

namespace KHC {

/**
 * Read nodes in an info file sequentially.
 *
 * @author Wojciech Smigaj <achu@klub.chip.pl>
 */
class InfoReader
{
public:
  /**
   * Construct a khcInfoReader object.
   *
   * @param topic Specifies which info topic should be processed. It can
   *        be omitted, but must be set before any reading is done.
   *        Example: "as", "tar"
   */
  InfoReader(QString sTopic = "");

  QString topic() const { return m_sTopic; }
  void setTopic(QString sTopic) { m_sTopic = sTopic; }

  /**
   * The main routine of the class. It reads next node from the info file
   * and stores it in a InfoNode object.
   * @param pNode Pointer to a InfoNode object in which the read node
   *        should be stored.
   * @param flags Any combination of the following constants:
   *        @li RETRIEVE_NAME: retrieve node's name
   #        @li RETRIEVE_TITLE: retrieve node's title (if it has one)
   *        @li RETRIEVE_NEIGHBOURS: retrieve names of the previous, next
   *            and upper nodes (in the info file hierarchy) 
   *        @li RETRIEVE_CONTENTS: retrieve node's text
   * @returns @li ERR_NONE on success
   *          @li ERR_NO_MORE_NODES if all nodes from all files on the topic
   *              have been read
   *          @li ERR_FILE_UNAVAILABLE if some or all of files on the
   *              specified topic don't exist or cannot be read.
   */
  uint getNextNode(InfoNode* pNode, uint flags);

protected:
  /**
   * Performs initialization.
   * @returns @li ERR_NONE on success
   *          @li ERR_FILE_UNAVAILABLE if some or all of files on the
   *              specified topic don't exist or cannot be read.
   */
  uint init();
  bool getRealFileName(QString baseName, QString& realName);

// variables
  QString m_sTopic;

  QPtrList<InfoFile> m_lFiles;

  bool m_bInitialized;
};

}

#endif // __KHC_INFOREADER__
// vim:ts=2:sw=2:et
