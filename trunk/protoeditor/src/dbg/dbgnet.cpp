/***************************************************************************
 *   Copyright (C) 2004 by Thiago Silva                                    *
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


#include "debuggerdbg.h"
#include "dbgnet.h"
#include "dbg_defs.h"
#include "dbgresponsepack.h"
#include "dbgconnection.h"
#include "dbgreceiver.h"
#include "dbgrequestor.h"
#include "dbgnetdata.h"
#include "dbgstack.h"
#include "debuggerstack.h"
#include "debuggerbreakpoint.h"
#include "dbgfileinfo.h"

#include "sitesettings.h"

#include <kdebug.h>
#include <knotifyclient.h>

DBGNet::DBGNet(DebuggerDBG* debugger, QObject *parent, const char *name)
    : QObject(parent, name), m_opts(0), m_sessionId(0), m_headerFlags(0), m_profFreq(-1),
    m_debugger(debugger), m_con(0), m_receiver(0), m_requestor(0),
    m_dbgStack(0), m_dbgFileInfo(0), m_isProfiling(false), m_setupProfile(false)
{
  m_receiver    = new DBGReceiver(this);
  m_requestor   = new DBGRequestor();
  m_dbgFileInfo = new DBGFileInfo();
  m_dbgStack    = new DBGStack();

  connect(m_receiver, SIGNAL(sigError(const QString&)),
          this, SLOT(slotError(const QString&)));

  connect(m_requestor, SIGNAL(sigError(const QString&)),
          this, SLOT(slotError(const QString&)));

  m_con = new DBGConnection();
  connect(m_con, SIGNAL(sigAccepted(QSocket*)), this, SLOT(slotIncomingConnection(QSocket*)));
  connect(m_con, SIGNAL(sigClientClosed()), this, SLOT(slotDBGClosed()));
  connect(m_con, SIGNAL(sigError(const QString&)), this, SLOT(slotError(const QString&)));

}

DBGNet::~DBGNet()
{
  delete m_con;
  delete m_receiver;
  delete m_requestor;
  delete m_dbgStack;
  delete m_dbgFileInfo;
}

bool DBGNet::startListener(int port)
{
  return m_con->listenOn(port);
}

void DBGNet::stopListener()
{
  m_con->close();
}

void DBGNet::requestPage(const QString& filePath, SiteSettings* site, int listenPort, dbgint sessid)
{
  m_dbgFileInfo->setSite(site);

  m_sessionId = sessid;
  m_requestor->makeHttpRequest(site->host()
                               , site->port()
                               , m_dbgFileInfo->toURI(filePath) /* /foo/bar.php */
                               , listenPort
                               , sessid);
}

void DBGNet::requestOptions(dbgint options)
{
  m_opts = options;
  m_requestor->requestOptions(options);
}

void DBGNet::requestContinue()
{
  m_requestor->requestContinue();
}

void DBGNet::requestStop()
{
  m_requestor->requestStop();
}

void DBGNet::requestStepInto()
{
  m_requestor->requestStepInto();
}

void DBGNet::requestStepOver()
{
  m_requestor->requestStepOver();
}

void DBGNet::requestStepOut()
{
  m_requestor->requestStepOut();
}


void DBGNet::requestVariables(dbgint scopeid, bool isGglobal)
{
  //so we know if we are asking for a vars on global or local scope
  if(isGglobal)
  {
    m_varScopeRequestList.push_back(GLOBAL_SCOPE_ID);
  }
  else
  {
    m_varScopeRequestList.push_back(scopeid);
  }
  m_requestor->requestVariables(scopeid);
}

void DBGNet::requestWatch(const QString& expression, dbgint scopeid)
{
  m_varScopeRequestList.push_back(WATCH_SCOPE_ID);
  m_requestor->requestWatch(expression, scopeid);
}

void DBGNet::requestBreakpoint(DebuggerBreakpoint* bp)
{
  m_requestor->requestBreakpoint( bp->id()
                                  , m_dbgFileInfo->moduleNumber(bp->filePath())
                                  , m_dbgFileInfo->toRemoteFilePath(bp->filePath())
                                  , bp->line()
                                  , bp->condition()
                                  , bp->status()
                                  , bp->skipHits());
}

void DBGNet::requestBreakpointRemoval(int bpid)
{
  m_requestor->requestBreakpointRemoval(bpid);
}

void DBGNet::requestProfileData(const QString& filePath, SiteSettings* site, int port, dbgint sessid)
{
  m_isProfiling = true;
  requestPage(filePath, site, port, sessid);
}

void DBGNet::setOptions(int op)
{
  m_opts = op;
}

void DBGNet::receivePack(DBGResponsePack* pack)
{
  if(!processHeader(pack->header()))
  {
    return;
  }

  DBGResponseTag* tag = NULL;
  pack->rewind();
  while((tag = pack->next()) != NULL)
  {
    tag->process(this, pack);
  }

  if(m_isProfiling)
  {
    setupProfile();
  }
  else
  {
    shipStack();
  }
}

void DBGNet::setupProfile()
{
  if(m_setupProfile) return;
  
  if(m_dbgFileInfo->contextUpdated())
  {
    //set temporary breakpoint on last line of the first module

//     QString filePath = m_dbgFileInfo->moduleName(1);
    m_requestor->requestBreakpoint( 0   //bpno
                                    , 1 //modno
                                    , ""/*filePath*/
                                    , m_dbgFileInfo->lastLineFromModule(1)
                                    , ""
                                    , BPS_ENABLED
                                    , 0
                                    , true);

    m_dbgFileInfo->clearContextStatus();

    //continue to reach it
    m_requestor->requestContinue();
  }
  else if(m_dbgFileInfo->moduleUpdated())
  {
    int totalModules = m_dbgFileInfo->totalModules();
    for(int i = 1; i <= totalModules; i++)
    {
      m_requestor->requestSrcLinesInfo(i);
      m_requestor->requestSrcCtxInfo(i);
      m_requestor->requestProfileData(i);
    }

    //every data we need were requested. We continue to end now
    m_requestor->requestContinue();
    m_setupProfile = true;
  }
}

bool DBGNet::processHeader(DBGHeader* header)
{
  if(header->sync() != DBGSYNC)
  {
    error("Network sync error.");
    return false;
  }

  //TODO: create dbgStarted() and dbgStepDone() to organize this.

  switch(header->cmd())
  {
    case DBGC_REPLY:
      break;
    case DBGC_END:
      break;
    case DBGC_EMBEDDED_BREAK:
      break;
    case DBGC_ERROR:
      if(m_isProfiling)
      {
        error("PHP could not execute the script. Check for errors before running the profiler.");
        return false;
      }
      break;
    case DBGC_LOG:
    case DBGC_SID:
    case DBGC_PAUSE:
      break;
    case DBGC_STARTUP:
      //so lets start too...
      m_requestor->addHeaderFlags(DBGF_STARTED);

      //...sending options...
      m_requestor->requestOptions(m_opts);

      //...tell everyone we are running.
      if(!m_isProfiling)
      {
        emit sigDBGStarted();
      }

      //...fancy notification on taskbar
      KNotifyClient::userEvent(0, "", KNotifyClient::Taskbar);

      if(m_isProfiling)
      {
        m_requestor->requestStepInto();
        
        //get module info
//      m_requestor->requestSrcTree();
        //get frequency info
        
        m_requestor->requestProfileFreqData(TEST_LOOPS);
        //get lines info to set temp breakpoint on the last valid line
        m_requestor->requestSrcLinesInfo(1);
        break;
      }

      if(!(m_opts & SOF_BREAKONLOAD))
      {
        //workaround for my lazyness (I don't know why setting SOF_BREAKONLOAD
        //is not enough to avoid the load break)
        m_requestor->requestContinue();
      }
      else
      {

        //...asking for module information..
        m_requestor->requestSrcTree();

        //workaround for the double step on the begginning
        //is annoying having to step twice in the begginig. Lets do the the first.
        m_requestor->requestStepInto();
      }

      break;
    case DBGC_BREAKPOINT:
      if(!m_isProfiling)
      {
        emit sigBreakpoint();
        processStepData();
      }
      else
      {
        processProfileData();
      }
      break;
    case DBGC_STEPINTO_DONE:
    case DBGC_STEPOVER_DONE:
    case DBGC_STEPOUT_DONE:
      if(!m_isProfiling)
      {
        emit sigStepDone();
        processStepData();
      }
      break;
  }
  return true;
}

void DBGNet::processStepData()
{
  m_dbgStack->clear();
  m_requestor->requestSrcTree();
}

void DBGNet::processProfileData()
{
   m_requestor->requestSrcTree();
}

void DBGNet::processSessionId(const DBGResponseTagSid* sid, DBGResponsePack* pack)
{
  switch(sid->sesstype())
  {
    case DBG_JIT:
      m_sessionId = QString(pack->retrieveRawdata(sid->isid())->data()).toLong();
      break;
    case DBG_REQ:
      break;
    case DBG_COMPAT:
    case DBG_EMB:
      //TODO: don't know about those
      break;
  }
}

void DBGNet::processStack(const DBGResponseTagStack* stack, DBGResponsePack* pack)
{
  if(m_isProfiling) return;

  m_dbgStack->add(stack->modNo()
                  , pack->retrieveRawdata(stack->idescr())->data()
                  , stack->lineNo(), stack->scopeId());
}

void DBGNet::processDBGVersion(const DBGResponseTagVersion* version, DBGResponsePack* pack)
{
  m_debugger->checkDBGVersion(version->majorVersion()
                              , version->minorVersion()
                              , pack->retrieveRawdata(version->idescription())->data());
}

void DBGNet::processSrcTree(const DBGResponseTagSrcTree* src, DBGResponsePack* pack)
{
  m_dbgFileInfo->addModuleInfo(src->modNo(),
                               pack->retrieveRawdata(src->imodName())->data());
}

void DBGNet::processSrcLinesInfo(const DBGResponseTagSrcLinesInfo* src, DBGResponsePack*)
{
  int total = src->startLineNo() + src->linesCount();
  for(int i = src->startLineNo(); i < total; i++)
  {
    m_dbgFileInfo->addContextInfo(src->ctxId(), src->modNo(), i);
  }
}

void DBGNet::processSrcCtxInfo(const DBGResponseTagSrcCtxInfo* ctx, DBGResponsePack* pack)
{
  if(ctx->ifunctionName())
  {
    m_dbgFileInfo->setContextname(ctx->ctxid(), pack->retrieveRawdata(ctx->ifunctionName())->data());
  }
}

void DBGNet::processEval(const DBGResponseTagEval* eval, DBGResponsePack* pack)
{
  if(m_isProfiling) return;

  /* Note about setAscii()
    When the script uses the session feature,
    DBG sends some variables (ie. CRITERIUMLIST) with \0 characters in the middle,
    wich fools QString attribution, then, having a broken
    text with the serialized vars.
    BUG: #1156552
  */

  QString error;
  QString str;
  QString result;

  if(eval->ierror())
  {
    error.setAscii(pack->retrieveRawdata(eval->ierror())->data(),
                   pack->retrieveRawdata(eval->ierror())->size()-1);
  }

  if(eval->istr())
  {
    str.setAscii(pack->retrieveRawdata(eval->istr())->data(),
                 pack->retrieveRawdata(eval->istr())->size()-1);
  }

  if(eval->iresult())
  {
    result.setAscii(pack->retrieveRawdata(eval->iresult())->data(),
                    pack->retrieveRawdata(eval->iresult())->size()-1);
  }

  //note: eval errors are annoying to be displayed to the user

  if(!error.isEmpty())
  {
    kdDebug() << "Eval error: " << error << endl;
  }

  dbgint scopeid = m_varScopeRequestList.first();
  m_varScopeRequestList.pop_front();

  if(scopeid == WATCH_SCOPE_ID)
  {
    m_debugger->updateWatch(result, str/*, error*/);
  }
  else
  {
    m_debugger->updateVar(result, str/*, error*/, (scopeid==GLOBAL_SCOPE_ID)?true:false);
  }
}

void DBGNet::processLog(const DBGResponseTagLog* log, DBGResponsePack* pack)
{
  if(m_isProfiling) return;

  QString logmsg;
  QString module;

  if(log->ilog())
  {
    logmsg = pack->retrieveRawdata(log->ilog())->data();
  }

  if(log->imodName())
  {
    module = pack->retrieveRawdata(log->imodName())->data();
  }

  m_debugger->debugLog(log->type()
                       , logmsg
                       , log->lineNo()
                       , m_dbgFileInfo->toLocalFilePath(module)
                       , log->extInfo());
}

void DBGNet::processError(const DBGResponseTagError* error, DBGResponsePack* pack)
{
  m_debugger->debugError(pack->retrieveRawdata(error->imessage())->data());
}

void DBGNet::processBreakpoint(const DBGTagBreakpoint* bp, DBGResponsePack* pack)
{
  if(m_isProfiling) return;

  QString modname;
  QString condition;

  if(bp->imodname())
  {
    modname = pack->retrieveRawdata(bp->imodname())->data();
  }
  if(bp->icondition())
  {
    condition = pack->retrieveRawdata(bp->icondition())->data();
  }

  m_debugger->updateBreakpoint(bp->bpNo()
                               , m_dbgFileInfo->toLocalFilePath(modname)
                               , bp->lineNo()
                               , bp->state()
                               //, bp->isTemp()
                               , bp->hitCount()
                               , bp->skipHits()
                               , condition
                               //, (bp->isUnderHit()==1)?true:false
                              );
}

void DBGNet::processProf(const DBGResponseTagProf* proftag, DBGResponsePack*)
{
  //NOTE: I'm not very confident about those bit << conversions

  ASSERT(m_profFreq != -1);

  QString moduleName = m_dbgFileInfo->moduleName(proftag->modNo());
  int ctxid = m_dbgFileInfo->contextId(proftag->modNo(), proftag->lineNo());

  QString contextName = m_dbgFileInfo->contextName(ctxid);
  if(contextName.isEmpty())
  {
    contextName = moduleName.section('/', -1).append("::main");
  }

  contextName.append("()");

  m_debugger->addProfileData(proftag->modNo(),
                             moduleName,
                             ctxid,
                             contextName,
                             proftag->lineNo(),
                             proftag->hitCount(),
                             (((double)(proftag->minLo() | (proftag->minHi() << 32)) / m_profFreq) * 1000),
                             (((double)(proftag->maxLo() | (proftag->maxHi() << 32)) / m_profFreq) * 1000),
                             (((double)(proftag->sumLo() | (proftag->sumHi() << 32)) / m_profFreq) * 1000));
}

void DBGNet::processProfC(const DBGResponseTagProfC* prof, DBGResponsePack*)
{
  //NOTE: I'm not very confident about those bit << conversions
  m_profFreq = prof->freqlo() | (prof->freqhi() << 32);
}

void DBGNet::shipStack()
{
  if(m_dbgStack->isEmpty()) return;

  if(m_dbgFileInfo->moduleUpdated())
  {
    m_debugger->updateStack(m_dbgStack->debuggerStack(m_dbgFileInfo));

    m_dbgFileInfo->clearModuleStatus();
    m_dbgStack->clear();
  }
}

void DBGNet::slotIncomingConnection(QSocket* sock)
{
  m_receiver->setSocket(sock);
  m_requestor->setSocket(sock);
}

void DBGNet::slotDBGClosed()
{
  //stoping the debugger (not the session!!)

  emit sigDBGClosed();

  m_receiver->clear();
  m_requestor->clear();

  m_dbgStack->clear();
  m_dbgFileInfo->clear();
  m_varScopeRequestList.clear();

  m_isProfiling = false;
  m_setupProfile = false;
}

void DBGNet::slotError(const QString& msg)
{
  error(msg);
}

void DBGNet::error(const QString& msg)
{
  emit sigError(msg);
  m_con->closeClient();
}


#include "dbgnet.moc"
