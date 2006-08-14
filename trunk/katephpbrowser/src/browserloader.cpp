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
#include "browserloader.h"

#include <kio/job.h>
#include <kio/netaccess.h>
#include "browsertab.h"
#include "browserlistview.h"
#include "browsernode.h"
#include <kfileitem.h>
#include <qfileinfo.h> 

#include "phpbrowserparser.h"
#include <kdirwatch.h>


BrowserLoader::BrowserLoader(BrowserTab* tab)
  : m_browserTab(tab), m_dirWatch(0)
{
  init();

}

BrowserLoader::~BrowserLoader()
{
  delete m_dirWatch;
}

void BrowserLoader::init()
{
  m_dirWatch = new KDirWatch();
  connect(m_dirWatch, SIGNAL(dirty(const QString &)),
      this, SLOT(slotDirty(const QString &)));

  connect(m_dirWatch, SIGNAL(created(const QString &)),
      this, SLOT(slotCreated(const QString &)));

  connect(m_dirWatch, SIGNAL(deleted(const QString &)),
      this, SLOT(slotDeleted(const QString &)));

  m_dirWatch->startScan(true);
}

void BrowserLoader::clear()
{
  delete m_dirWatch;
  init();
}

void BrowserLoader::update(const Schema& schema)
{
  m_dirlist = schema.directoryList();


  if(m_dirlist.count() != 0)
  {
    m_currentURL = m_dirlist.front();
    m_dirlist.pop_front();

    processDirectories();

    if(!m_dirWatch->contains(m_currentURL.path()))
    {
      m_dirWatch->addDir(m_currentURL.path(), true);
    }
  }  
}

void BrowserLoader::slotDirty(const QString& path)
{
  if(QFileInfo(path).isDir())
  {
    m_currentURL = KURL::fromPathOrURL(path);
    processDirectories();
  }
  else
  {
    loadFile(KURL::fromPathOrURL(path));
  } 
}

void BrowserLoader::slotCreated(const QString& path)
{
  if(QFileInfo(path).isDir())
  {
    m_currentURL = KURL::fromPathOrURL(path);
    processDirectories();
  }
  else
  {
    loadFile(KURL::fromPathOrURL(path));
  }
}

void BrowserLoader::slotDeleted(const QString& path)
{  
  if(QFileInfo(path).isDir())
  {
    m_currentURL = KURL::fromPathOrURL(path);
    processDirectories();
  }
  else
  {
    loadFile(KURL::fromPathOrURL(path));
  }
}

void BrowserLoader::processDirectories()
{
  KIO::ListJob *job;
  job = KIO::listDir(m_currentURL, false,false);

  connect(job, SIGNAL(entries(KIO::Job*, const KIO::UDSEntryList&)),
      SLOT(slotEntries(KIO::Job*, const KIO::UDSEntryList&)));

  connect(job, SIGNAL(result(KIO::Job *)),SLOT(slotResult(KIO::Job *)));

	connect(job, SIGNAL(redirection(KIO::Job *, const KURL &)),
      SLOT(slotRedirection(KIO::Job *, const KURL &)));
}


void BrowserLoader::slotResult(KIO::Job *job)
{
	if (job && job->error())
  {
		job->showErrorDialog();
    return;
  }

  if(m_dirlist.count() != 0) 
  {    
    m_currentURL = m_dirlist.front();
    m_dirlist.pop_front();
    if(!m_dirWatch->contains(m_currentURL.path()))
    {
      m_dirWatch->addDir(m_currentURL.path(), true);
    }
    processDirectories();
  }
}

void BrowserLoader::slotRedirection(KIO::Job *, const KURL & url)
{
	m_currentURL = url;
}

void BrowserLoader::slotEntries(KIO::Job * job, const KIO::UDSEntryList &entries)
{
  if (job && job->error())
  {
    job->showErrorDialog();
    return;
  }

	KIO::UDSEntryListConstIterator it = entries.begin();
	KIO::UDSEntryListConstIterator end = entries.end();
  
  for (; it != end; ++it)
  {
    KFileItem file(*it, m_currentURL, false /* no mimetype detection */, true);
    if(!file.isDir() && (file.url().filename().right(4) == ".php"))
    {
      loadFile(file.url());
    }
	}
}

void BrowserLoader::loadFile(const KURL& url)
{  
  if(!m_dirWatch->contains(url.path()))
  {
    m_dirWatch->addFile(url.path());
  }

  PHPBrowserParser parser;

  QPtrList<BrowserNode> list = parser.parseURL(url);
  m_browserTab->listView()->addFileNodes(url, list);
}

#include "browserloader.moc"
