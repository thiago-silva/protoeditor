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
#include "schemasettings.h"

SchemaSettings::SchemaSettings()
  : KConfigSkeleton(QString::fromLatin1("katephpbrowserrc"))
{
  KConfigSkeleton::ItemString  *itemCurrent;
  itemCurrent = new KConfigSkeleton::ItemString( currentGroup(), QString::fromLatin1( "CurrentSchemaName" ), m_current);
  addItem( itemCurrent, QString::fromLatin1( "CurrentSchemaName" ) );

  readConfig();
}


SchemaSettings::~SchemaSettings()
{
}

void SchemaSettings::setCurrentSchemaName(const QString& current)
{
  m_current = current;
}

QString SchemaSettings::currentSchemaName()
{
  return m_current;
}

Schema SchemaSettings::schema(const QString& name)
{
  if(m_map.contains(name))
  {
    return m_map[name];
  }
  else
  {
    return Schema();
  }
}
void SchemaSettings::setSchema(const Schema& s)
{
  m_map[s.name()] = s;
}

void SchemaSettings::setSchemas(const QMap<QString, Schema>& map)
{
  m_map = map;
}

void SchemaSettings::setSchemas(const QValueList<Schema>& list)
{
  m_map.clear();
  for(QValueList<Schema>::const_iterator it = list.begin(); it != list.end(); ++it)
  {
    m_map[(*it).name()] = (*it);
  }
}

QMap<QString, Schema> SchemaSettings::schemas()
{
  return m_map;
}

void SchemaSettings::usrWriteConfig()
{
  KConfig* c = config();
  int i = 0;

  DirectoryList_t list;
  QMap<QString, Schema>::iterator it;
  QStringList slist;

  QMap<QString, QStringList> folderstructure;
  QStringList flist;
  for(it = m_map.begin(); it != m_map.end(); ++it, i++) 
  {
    c->deleteGroup(QString("Schema_%1").arg(i));
    c->setGroup(QString("Schema_%1").arg(i));
     
    c->writeEntry("SchemaName", it.key());
    list = it.data().directoryList();
    for(QValueList<KURL>::iterator dit = list.begin(); dit != list.end(); ++dit)
    {
      slist.append((*dit).prettyURL());
    }     
    c->writeEntry("Directories", slist);
    slist.clear();

    //folder structures
    folderstructure = it.data().folderStructure();    
    int j = 0;
    for(QMap<QString, QStringList>::iterator fit = folderstructure.begin(); fit !=
        folderstructure.end(); ++fit, j++)
    {
      flist << fit.key();
      flist += fit.data();
      c->writeEntry(QString("FolderStructure_%1").arg(j), flist);
      flist.clear();
    }
    folderstructure.clear();    
  }

  //remove pending
  QString group = QString("Schema_%1").arg(i);
  while(c->hasGroup(group)) 
  {
    c->deleteGroup(group);
    i++;
    group = QString("Schema_%1").arg(i);
  }
}

void SchemaSettings::usrReadConfig()
{
  KConfig* c = config();
  int i = 0;
  int j = 0;
  QString name;
  QStringList dirs;
  QMap<QString, QStringList> folders;

  QString group = QString("Schema_%1").arg(i);
  while(c->hasGroup(group)) 
  {
    c->setGroup(group);

    name = c->readEntry("SchemaName");
    dirs = c->readListEntry("Directories");
    
    Schema s(name, dirs);
    
    j = 0;
    while(c->hasKey(QString("FolderStructure_%1").arg(j)))
    {
      QStringList s = c->readListEntry(QString("FolderStructure_%1").arg(j));
      QString fname = s.first();
      s.pop_front();
      folders[fname] = s;
      j++;
    }
    
    s.setFolderStructure(folders);
    folders.clear();
    m_map[name] = s;
    
    i++;
    group = QString("Schema_%1").arg(i);
  }
}
