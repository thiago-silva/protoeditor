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


#include "dbgrequestpackbuilder.h"

#include "dbgrequestpack.h"
#include "dbg_defs.h"
#include "dbgheader.h"
#include "debuggerbreakpoint.h"
#include "dbgtags.h"
#include "dbgframe.h"

/* commands that doesn't need body information */
DBGRequestPack* DBGRequestPackBuilder::buildCommand(int cmd)
{
    DBGRequestPack* p = new DBGRequestPack(cmd);
    p->header()->setFlags(DBGF_STARTED);
    return p;
}

DBGRequestPack* DBGRequestPackBuilder::buildWatch(const QString expression, int scope_id)
{

  DBGRequestPack *p = new DBGRequestPack(DBGA_REQUEST);

  DBGRequestTagEval* eval = new DBGRequestTagEval(1, scope_id);

  DBGFrame* frameval = new DBGFrame(FRAME_EVAL, eval->tagSize());

  DBGTagRawdata* rawdata = new DBGTagRawdata(
                    1
                    , expression.ascii()
                    , expression.length()+1);

  DBGFrame* frameraw = new DBGFrame(FRAME_RAWDATA, rawdata->tagSize());

  p->addInfo(frameraw, rawdata);
  p->addInfo(frameval, eval);

  return p;
}


DBGRequestPack* DBGRequestPackBuilder::buildDeletedBreakpoint(int bpid)
{
  DBGRequestPack* p = new DBGRequestPack(DBGA_REQUEST);
  DBGFrame* framebp;

  DBGTagBreakpoint* bptag = new DBGTagBreakpoint();
  framebp = new DBGFrame(FRAME_BPS, bptag->tagSize());

  bptag->setBpNo(bpid);
  bptag->setState(BPS_DELETED);

  p->addInfo(framebp, bptag);

  return p;
}

DBGRequestPack* DBGRequestPackBuilder::buildBreakpointList(int bpno) {
  DBGRequestPack* p = new DBGRequestPack(DBGA_REQUEST);
  DBGFrame* framebp;

  DBGRequestTagBpl* bptag = new DBGRequestTagBpl(bpno);
  framebp = new DBGFrame(FRAME_BPL, bptag->tagSize());

  p->addInfo(framebp, bptag);
  return p;
}

DBGRequestPack* DBGRequestPackBuilder::buildBreakpoint(int modno, DebuggerBreakpoint* breakpoint)
{
  DBGRequestPack* p = new DBGRequestPack(DBGA_REQUEST);
  DBGFrame* framebp;

  DBGTagBreakpoint* bptag = new DBGTagBreakpoint();
  framebp = new DBGFrame(FRAME_BPS, bptag->tagSize());

  bptag->setLineNo(breakpoint->line());
  bptag->setImodName(1);
  switch(breakpoint->status()) {
    case DebuggerBreakpoint::ENABLED:
      bptag->setState(BPS_ENABLED);
      break;
    case DebuggerBreakpoint::DISABLED:
      bptag->setState(BPS_DISABLED);
      break;
    case DebuggerBreakpoint::UNRESOLVED:
      //doesn't make sense we ask for an unresolved.
      //we have to force it to be enabled
      bptag->setState(BPS_ENABLED);
      break;
    default:
      bptag->setState(BPS_DELETED);
      break;
  }

  bptag->setBpNo(breakpoint->id());
  bptag->setModNo(modno);

  DBGTagRawdata* rawdata = new DBGTagRawdata(
                    1
                    , breakpoint->filePath().ascii()
                    , breakpoint->filePath().length()+1);

  DBGFrame* frameraw = new DBGFrame(FRAME_RAWDATA, rawdata->tagSize());

  p->addInfo(frameraw, rawdata);
  p->addInfo(framebp, bptag);

  return p;
}

DBGRequestPack* DBGRequestPackBuilder::buildVars(int scope_id)
{
  DBGRequestPack *p = new DBGRequestPack(DBGA_REQUEST);

  DBGRequestTagEval* eval = new DBGRequestTagEval(0, scope_id);

  DBGFrame* frame = new DBGFrame(FRAME_EVAL, eval->tagSize());

  p->addInfo(frame, eval);
  return p;
}

DBGRequestPack* DBGRequestPackBuilder::buildSrcTree()
{
  DBGRequestPack *p = new DBGRequestPack(DBGA_REQUEST);

  DBGFrame* frame = new DBGFrame(FRAME_SRC_TREE, 0);

  p->addInfo(frame, NULL);
  return p;
}

DBGRequestPack* DBGRequestPackBuilder::buildOptions(int op) {
  DBGRequestPack *p = new DBGRequestPack(DBGA_REQUEST);

  DBGFrame* frame = new DBGFrame(FRAME_SET_OPT, 8);
  DBGRequestTagOptions* tagop = new DBGRequestTagOptions(op);

  p->addInfo(frame, tagop);
  return p;
}
