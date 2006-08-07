/***************************************************************************
 *   Copyright (C) 2004-2006 by Thiago Silva                               *
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

#include "perllocalcom.h"

#include "protoeditor.h"
#include "protoeditorsettings.h"
#include "perlsettings.h"

#include <kprocess.h>
#include <klocale.h>
#include <qstring.h>
#include <kurl.h>
#include <qregexp.h>

PerlLocalCom::PerlLocalCom()
  : PerlCom(), m_process(0), m_processRunning(false)  
{
}


PerlLocalCom::~PerlLocalCom()
{
  if(m_process)
  {
    m_process->kill();
  }

  delete m_process;
}

void PerlLocalCom::slotProcessExited(KProcess*)
{
  m_processRunning = false;
}

void PerlLocalCom::slotReceivedOutput(KProcess *, char *buffer, int buflen)
{
  QCString msg(buffer, buflen+1);

  QRegExp rx;
  //rx.setPattern(".*\027.*DB<\\d*>.*");
  rx.setPattern("\033.*DB.*\033[^\n]*");

  if(rx.search(buffer, 0) != -1) 
  {
    msg.replace( rx, "DB:" );
  }
 
  
  emit sigOutput(msg);
}

void PerlLocalCom::startDebugging(const QString& filePath, const QString& )
{
  if(!m_processRunning)
  {
    delete m_process;

    m_process = new KProcess;
    connect(m_process, SIGNAL(processExited(KProcess*)),
            this, SLOT(slotProcessExited(KProcess*)));

    connect(m_process, SIGNAL(receivedStdout(KProcess*, char *, int)),
      this, SLOT(slotReceivedOutput(KProcess *, char*, int)));

    connect(m_process, SIGNAL(receivedStderr(KProcess*, char*, int)),
      this, SLOT(slotReceivedOutput(KProcess *, char*, int)));


  }

  if(m_processRunning)
  {
    m_process->detach();
  }

  m_process->clearArguments();
 
  KURL url = KURL::fromPathOrURL(filePath);

  QString cmd = 
    Protoeditor::self()->settings()->languageSettings(PerlSettings::lang)->interpreterCommand();
  //*m_process << (QString("cd ") +  url.directory() + ";" + cmd + " " + url.path() + " " + args);
  *m_process << cmd << "-d" << url.path();


  if(!m_process->start(KProcess::NotifyOnExit, KProcess::Communication(KProcess::All)))
  {
    emit sigError(i18n("Error executing console."));
  }
  else
  {
    m_processRunning = true;    
  }
}

void PerlLocalCom::requestContinue()
{
}

void PerlLocalCom::requestStop()
{
}

void PerlLocalCom::requestRunToCursor(const QString& , int )
{
}

void PerlLocalCom::requestStepInto()
{
}

void PerlLocalCom::requestBreakpoint(DebuggerBreakpoint*)
{
}

void PerlLocalCom::requestBreakpointRemoval(int)
{
}

void PerlLocalCom::requestWatch(const QString&)
{
}

void PerlLocalCom::executeCmd(const QString& str)
{
  QString cmd = str + "\n";
  m_process->writeStdin(cmd, cmd.length());
}

#include "perllocalcom.moc"
