/***************************************************************************
 *   Copyright (C) 2006 by Thiago Silva                                    *
 *   thiago.silva@kdemail.net                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef BROWSERLOADER_H
#define BROWSERLOADER_H

#include <qobject.h>
#include <qvaluelist.h>
#include <qmap.h>
#include <kurl.h>

#include "schema.h"

#include <kio/global.h>

namespace KIO { class Job; }

class BrowserTab;
class BrowserNode;

class BrowserLoader : public QObject
{
  Q_OBJECT
public:
  BrowserLoader(BrowserTab*);
  ~BrowserLoader();

  void update(const Schema&);

private slots:
  void slotResult(KIO::Job*);
  void slotRedirection(KIO::Job*, const KURL &);
  void slotEntries(KIO::Job*, const KIO::UDSEntryList &);

private:
  void processDirectories();

  BrowserTab* m_browserTab;
  
  KURL m_currentURL;
  QMap<KURL, QValueList<BrowserNode*> > m_nodeMap; //<fileurl, nodeList>
  DirectoryList_t m_dirlist;
  
};

#endif
