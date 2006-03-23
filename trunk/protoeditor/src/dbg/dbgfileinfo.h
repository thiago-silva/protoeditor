/***************************************************************************
 *   Copyright (C) 2004-2005 by Jesterman                                       *
 *   jesterman@brturbo.com                                                 *
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
#ifndef DBGFILEINFO_H
#define DBGFILEINFO_H
#include <qmap.h>

class SiteSettings;

class DBGFileInfo {
public:
  DBGFileInfo();
  ~DBGFileInfo();

  void setSite(SiteSettings*);

  QString mapLocalToRemote(const QString&);
  QString mapRemoteToLocal(const QString&);

  const QString& moduleName(int modno);
  int moduleNumber(const QString&);

  void addModuleInfo(int, const QString&);

  void addContextInfo(int ctxid, int modno, int lineno);
  void setContextname(int ctxid, const QString&);
  
  int contextId(int modno, int lineno);
  QString contextName(int ctxid);
  
  bool contextUpdated();
  bool moduleUpdated();

//   void clearContextData();
//   void clearModuleData();
  
  void clear();
  
  void clearModuleStatus();
  void clearContextStatus();

  int lastLineFromModule(int modno);

  int totalModules();
private:

  SiteSettings*      m_site;
  bool               m_moduleUpdated;
  bool               m_contextUpdated;
  QMap<int, QString> m_fileMap;

  class ContextData {
    public:
    ContextData(int c, int m, int l)
      : ctxid(c), modno(m), lineno(l) {}
    ContextData() : ctxid(0), modno(0), lineno(0) {}
    ~ContextData() {}
    
    int     ctxid;
    int     modno;
    int     lineno;
    QString ctxname;
  };
    
  QValueList<ContextData> m_contextList;
};

#endif
