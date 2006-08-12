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

#include "phpbrowserparser.h"


BrowserLoader::BrowserLoader(BrowserTab* tab)
  : m_browserTab(tab)
{
}

BrowserLoader::~BrowserLoader()
{
}

void BrowserLoader::update(const Schema& schema)
{
  m_dirlist = schema.directoryList();

  if(m_dirlist.count() == 0) return;

  m_currentURL = m_dirlist.front();
  m_dirlist.pop_front();
  processDirectories();
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
		//job->showErrorDialog();
    //ops!
  }

  if(m_dirlist.count() != 0) 
  {    
    m_currentURL = m_dirlist.front();
    m_dirlist.pop_front();
    processDirectories();
  }
}

void BrowserLoader::slotRedirection(KIO::Job *, const KURL & url)
{
	m_currentURL = url;
}

void BrowserLoader::slotEntries(KIO::Job *, const KIO::UDSEntryList &entries)
{
	KIO::UDSEntryListConstIterator it = entries.begin();
	KIO::UDSEntryListConstIterator end = entries.end();

  PHPBrowserParser parser;
  for (; it != end; ++it)
  {
    KFileItem file(*it, m_currentURL, false /* no mimetype detection */, true);
    if (!file.isDir() && (file.url().filename().right(4) == ".php"))
    {   
      KURL url = file.url();
      QValueList<BrowserNode*> list = parser.parseURL(file.url());
      if(list.count() > 0)
      { 
        m_browserTab->listView()->loadFileNodes(file.url(), list);
      }
    }
	}
}

#include "browserloader.moc"
