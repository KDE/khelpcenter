/*
 *  infohierarchymaker.h - part of the KDE Help Center
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

#ifndef __KHC_INFOHIERARCHYMAKER__
#define __KHC_INFOHIERARCHYMAKER__

#include <qobject.h>
#include <qstring.h>
#include <qtimer.h>

#include "inforeader.h"

namespace KHC {

/**
 * This class creates info nodes hierarchy: it reads (indirectly - using
 * InfoReader etc.) info files, belonging to the given info subject, and
 * creates the nodes tree. 
 *
 * @author Wojciech Smigaj <achu@klub.chip.pl>
 */
class InfoHierarchyMaker : public QObject
{
  Q_OBJECT
public:
  InfoHierarchyMaker();
  virtual ~InfoHierarchyMaker();

  /**
   * The most important function. It begins creating the hierarchy of info nodes,
   * according to the given paramenters. When the hierarchy is ready, the signal @ref hierarchyCreated
   * is emitted.
   * @param key some uint value (possibly even a converted pointer) which will be returned as the "key"
   *        argument of the hierarchyCreated signal. It should uniquely identify the hierarchy creation "job".
   * @param topic topic of the info node which should be the root of the hierarchy created (e.g. "emacs")
   * @param root name of the info node which should be the root of the hierarchy created (e.g. "Overview").
   *        If it is empty, the "highest" (top) node of the topic becomes the root.
   */
  void createHierarchy(uint key, QString topic, QString root = "");

  /**
   * @returns true if the object is creating hierarchy at the moment.
   */
  bool isWorking() const { return m_bIsWorking; }

signals:
  /**
   * This signal is emitted when the hierarchy creation is finished.
   * @param key the value specified as the "key" parameter to the createHierarchy function
   * @param nErrorCode result of the hierarchy creation; can be one of:
   *        @li ERR_NONE on success
   *        @li ERR_FILE_UNAVAILABLE if one or more files required to create the hierarchy couldn't be accessed
   *        @li ERR_NO_HIERARCHY if the nodes seem not to be ordered in a hierarchy (well, in fact
   *            they have not to; the specification allows them to be ordered in whatever order)
   * @param pTopNode root of the hierarchy
   */
  void hierarchyCreated(uint key, uint nErrorCode, const InfoNode* pTopNode);

public slots:
  void getSomeNodes();

protected:
  void clearNodesList();
  bool makeHierarchy(InfoNode** ppTopNode, QString topNodeName = "");
  bool findChildren(InfoNode* pParentNode);
  bool orderSiblings(std::list<InfoNode*>& siblingsList);
  void restoreChildren(InfoNode* pParentNode);

  InfoReader m_infoReader;
  std::list<InfoNode*> m_lNodes;

  QTimer m_timer;
  QString m_root;
  uint m_key;
  bool m_bIsWorking;
};

}

#endif // __KHC_INFOHIERARCHYMAKER__
// vim:ts=2:sw=2:et
