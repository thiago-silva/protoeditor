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


#include "dbgtags.h"
#include "dbg_defs.h"
#include "dbgnethelper.h"

/*************************************************************
 * DBGTagBase
 *************************************************************/

DBGBaseTag::DBGBaseTag()
  : m_frameName(0)
{
}

DBGBaseTag::DBGBaseTag(int tagName)
{
  setName(tagName);
}

DBGBaseTag::~DBGBaseTag()
{
}

void DBGBaseTag::setName(int frameName)
{
  m_frameName = frameName;
}

int DBGBaseTag::name()
{
    return m_frameName;
}

/*************************************************************
 * DBGRequestTag
 *************************************************************/

DBGRequestTag::DBGRequestTag(int tagName)
  : DBGBaseTag(tagName), DBGAbstractDataRequest()
{
  switch(tagName) {
    case FRAME_RAWDATA:
      setTagSize(TAG_RAWDATA_SIZE);
      break;
    case FRAME_SOURCE:
      //
      break;
    case FRAME_BPL:
      setTagSize(TAG_REQ_BPL_SIZE);
      break;
    case FRAME_EVAL:
      setTagSize(TAG_REQ_EVAL_SIZE);
      break;
   case FRAME_BPS:
      setTagSize(TAG_BPS_SIZE);
      break;
    case FRAME_SRCLINESINFO:
      //
      break;
    case FRAME_SRCCTXINFO:
      //
      break;
    case FRAME_PROF:
      //
      break;
    case FRAME_PROF_C:
      //
      break;
    case FRAME_SET_OPT:
      setTagSize(FRAME_SET_OPT_SIZE);
      break;
  }
}

DBGRequestTag::~DBGRequestTag()
{
}

void DBGRequestTag::setTagSize(long tagSize)
{
  m_tagSize = tagSize;
}
long DBGRequestTag::tagSize()
{
  return m_tagSize;
}

/*************************************************************
 * DBGTagRawdata
 *************************************************************/

DBGTagRawdata::DBGTagRawdata()
 : DBGRequestTag(FRAME_RAWDATA), m_id(0), m_size(0), m_data(0)
{
}

DBGTagRawdata::DBGTagRawdata(int id, const char* data, long datasize)
  : DBGRequestTag(FRAME_RAWDATA)
{
  m_id = id;

  int mod = datasize % 4;
  m_data = new char[datasize + mod];
  //m_data = (char*) data;



  int i;
  for(i = 0; i < datasize; i++) {
    m_data[i] = data[i];
  }

  m_size = datasize + mod;

  for( ; i < m_size; i++) {
    m_data[i] = 0;
  }
}

DBGTagRawdata::DBGTagRawdata(char* responseBuffer)
  : DBGRequestTag(FRAME_RAWDATA)
{
  m_id   = DBGNetHelper::fromNetwork(responseBuffer, 0);
  m_size = DBGNetHelper::fromNetwork(responseBuffer, 4);

  m_data = new char[m_size];

  for(long i = 0; i < m_size; i++)
  {
    m_data[i] = responseBuffer[i+8];
  }
}

DBGTagRawdata::~DBGTagRawdata()
{
  delete [] m_data;
}

void DBGTagRawdata::setId(int id)
{
  m_id = id;
}

void DBGTagRawdata::setData(char* data, long size)
{
  delete [] m_data;
  m_size = size;

  m_data = new char[m_size];
  for(long i = 0; i < m_size; i++) {
    m_data[i] = data[i];
  }
  //m_data = data;
}

int DBGTagRawdata::id()
{
  return m_id;
}
long DBGTagRawdata::size()
{
  return m_size;
}
char* DBGTagRawdata::data()
{
  return m_data;
}

long DBGTagRawdata::tagSize()
{
  return m_tagSize + m_size + (m_size % 4);
}

char* DBGTagRawdata::toArray()
{
  clearRawData();

  long bsize = tagSize();
  m_raw = new char[bsize];

  m_raw = DBGNetHelper::toNetwork(m_id  , m_raw, 0);
  m_raw = DBGNetHelper::toNetwork(m_size, m_raw, 4);

  long i;
  for(long j = 0, i = 8; j < m_size; i++, j++) {
    m_raw[i] = m_data[j];
  }

  for(; i < bsize; i++) {
    m_raw[i] = 0;
  }

  return m_raw;
}

/*************************************************************
 * DBGResponseTagSid
 *************************************************************/

DBGResponseTagSid::DBGResponseTagSid()
  : DBGBaseTag(FRAME_SID), m_isid(0), m_sesstype(0)
{
}

DBGResponseTagSid::DBGResponseTagSid(char* responseBuffer)
 : DBGBaseTag(FRAME_SID)
{
  m_isid     = DBGNetHelper::fromNetwork(responseBuffer, 0);
  m_sesstype = DBGNetHelper::fromNetwork(responseBuffer, 4);
}

DBGResponseTagSid::~DBGResponseTagSid() {
}

void DBGResponseTagSid::setIsid(int isid)
{
  m_isid = isid;
}
void DBGResponseTagSid::setSesstype(int sesstype)
{
  m_sesstype = sesstype;
}
int DBGResponseTagSid::isid()
{
  return m_isid;
}
int DBGResponseTagSid::sesstype()
{
  return m_sesstype;
}

/*************************************************************
 * DBGResponseTagStack
 *************************************************************/

DBGResponseTagStack::DBGResponseTagStack()
 : DBGBaseTag(FRAME_STACK), m_line_no(0), m_mod_no(0), m_scope_id(0), m_idescr(0)
{
}
DBGResponseTagStack::DBGResponseTagStack(char* responseBuffer)
 : DBGBaseTag(FRAME_STACK)
{
  m_line_no  = DBGNetHelper::fromNetwork(responseBuffer, 0);
  m_mod_no   = DBGNetHelper::fromNetwork(responseBuffer, 4);
  m_scope_id = DBGNetHelper::fromNetwork(responseBuffer, 8);
  m_idescr   = DBGNetHelper::fromNetwork(responseBuffer, 12);
}

DBGResponseTagStack::~DBGResponseTagStack()
{
}

void DBGResponseTagStack::setLineNo(int line_no)
{
  m_line_no = line_no;
}
void DBGResponseTagStack::setModNo(int mod_no)
{
  m_mod_no = mod_no;
}
void DBGResponseTagStack::setScopeId(int scope_id)
{
  m_scope_id = scope_id;
}
void DBGResponseTagStack::setIdescr(int idescr)
{
  m_idescr = idescr;
}
int DBGResponseTagStack::lineNo()
{
  return m_line_no;
}
int DBGResponseTagStack::modNo()
{
  return m_mod_no;
}
int DBGResponseTagStack::scopeId()
{
  return m_scope_id;
}
int DBGResponseTagStack::idescr()
{
  return m_idescr;
}

/*************************************************************
 * DBGResponseTagVersion
 *************************************************************/

DBGResponseTagVersion::DBGResponseTagVersion()
 : DBGBaseTag(FRAME_VER), m_major_version(0), m_minor_version(0), m_idescription(0)
{
}
DBGResponseTagVersion::DBGResponseTagVersion(char* responseBuffer)
  : DBGBaseTag(FRAME_VER)
{
  m_major_version = DBGNetHelper::fromNetwork(responseBuffer, 0);
  m_minor_version = DBGNetHelper::fromNetwork(responseBuffer, 4);
  m_idescription  = DBGNetHelper::fromNetwork(responseBuffer, 8);
}

DBGResponseTagVersion::~DBGResponseTagVersion()
{
}
void DBGResponseTagVersion::setMajorVersion(int major_version)
{
  m_major_version = major_version;
}
void DBGResponseTagVersion::setMinorVersion(int minor_version)
{
  m_minor_version = minor_version;
}
void DBGResponseTagVersion::setIdescription(int idescription)
{
  m_idescription = idescription;
}
int DBGResponseTagVersion::majorVersion()
{
  return m_major_version;
}
int DBGResponseTagVersion::minorVersion()
{
  return m_minor_version;
}
int DBGResponseTagVersion::idescription()
{
  return m_idescription;
}

/*************************************************************
 * DBGResponseTagSrcLinesInfo
 *************************************************************/

DBGResponseTagSrcLinesInfo::DBGResponseTagSrcLinesInfo()
 : DBGBaseTag(FRAME_SRC_TREE), m_parent_mod_no(0), m_parent_line_no(0), m_mod_no(0), m_imod_name(0)
{
}
DBGResponseTagSrcLinesInfo::DBGResponseTagSrcLinesInfo(char* responseBuffer)
  : DBGBaseTag(FRAME_SRC_TREE)
{
  m_parent_mod_no  = DBGNetHelper::fromNetwork(responseBuffer, 0);
  m_parent_line_no = DBGNetHelper::fromNetwork(responseBuffer, 4);
  m_mod_no         = DBGNetHelper::fromNetwork(responseBuffer, 8);
  m_imod_name      = DBGNetHelper::fromNetwork(responseBuffer, 12);
}

DBGResponseTagSrcLinesInfo::~DBGResponseTagSrcLinesInfo()
{
}

void DBGResponseTagSrcLinesInfo::setParentModNo(int parentmodno)
{
  m_parent_mod_no = parentmodno;
}

void DBGResponseTagSrcLinesInfo::setParentLineNo(int parentLineNo)
{
  m_parent_line_no = parentLineNo;
}

void DBGResponseTagSrcLinesInfo::setModNo(int modno)
{
  m_mod_no = modno;
}

void DBGResponseTagSrcLinesInfo::setImodName(int imodname)
{
  m_imod_name = imodname;
}

int DBGResponseTagSrcLinesInfo::parentModNo()
{
  return m_parent_mod_no;
}

int DBGResponseTagSrcLinesInfo::parentLineNo()
{
  return m_parent_line_no;
}

int DBGResponseTagSrcLinesInfo::modNo()
{
  return m_mod_no;
}

int DBGResponseTagSrcLinesInfo::imodName()
{
  return m_imod_name;
}

/****************************
 * DBGResponseTagEval
 **************************/

DBGResponseTagEval::DBGResponseTagEval()
 : DBGBaseTag(FRAME_EVAL), m_istr(0), m_iresult(0), m_ierror(0)
{
}

DBGResponseTagEval::DBGResponseTagEval(char* responseBuffer)
  : DBGBaseTag(FRAME_EVAL)
{
  m_istr    = DBGNetHelper::fromNetwork(responseBuffer, 0);
  m_iresult = DBGNetHelper::fromNetwork(responseBuffer, 4);
  m_ierror  = DBGNetHelper::fromNetwork(responseBuffer, 8);
}

DBGResponseTagEval::~DBGResponseTagEval()
{
}

void DBGResponseTagEval::setIstr(int istr)
{
  m_istr = istr;
}
void DBGResponseTagEval::setIresult(int iresult)
{
  m_iresult = iresult;
}
void DBGResponseTagEval::setIerror(int ierror)
{
  m_ierror = ierror;
}
int DBGResponseTagEval::istr()
{
  return m_istr;
}
int DBGResponseTagEval::iresult()
{
  return m_iresult;
}
int DBGResponseTagEval::ierror()
{
  return m_ierror;
}


/****************************
 * DBGResponseTagLog
 **************************/
DBGResponseTagLog::DBGResponseTagLog()
  : DBGBaseTag(FRAME_LOG), m_ilog(0), m_type(0), m_modNo(0),
  m_lineNo(0), m_imodName(0), m_extInfo(0)
{
}

DBGResponseTagLog::DBGResponseTagLog(char* responseBuffer)
  : DBGBaseTag(FRAME_LOG)
{
  m_ilog     = DBGNetHelper::fromNetwork(responseBuffer, 0);
  m_type     = DBGNetHelper::fromNetwork(responseBuffer, 4);
  m_modNo    = DBGNetHelper::fromNetwork(responseBuffer, 8);
  m_lineNo   = DBGNetHelper::fromNetwork(responseBuffer, 12);
  m_imodName = DBGNetHelper::fromNetwork(responseBuffer, 16);
  m_extInfo  = DBGNetHelper::fromNetwork(responseBuffer, 20);
}

DBGResponseTagLog::~DBGResponseTagLog()
{
}

void DBGResponseTagLog::setIlog(int ilog)
{
  m_ilog = ilog;
}
void DBGResponseTagLog::setType(int type)
{
  m_type = type;
}

void DBGResponseTagLog::setModNo(int modno)
{
  m_modNo = modno;
}

void DBGResponseTagLog::setLineNo(int lineno)
{
  m_lineNo = lineno;
}

void DBGResponseTagLog::setImodName(int imodname)
{
  m_imodName = imodname;
}

void DBGResponseTagLog::setExtInfo(int extinfo)
{
  m_extInfo = extinfo;
}

int DBGResponseTagLog::ilog()
{
  return m_ilog;
}

int DBGResponseTagLog::type()
{
  return m_type;
}

int DBGResponseTagLog::modNo()
{
  return m_modNo;
}

int DBGResponseTagLog::lineNo()
{
  return m_lineNo;
}

int DBGResponseTagLog::imodName()
{
  return m_imodName;
}

int DBGResponseTagLog::extInfo()
{
  return m_extInfo;
}


/****************************
 * DBGResponseTagError
 **************************/

DBGResponseTagError::DBGResponseTagError()
  : DBGBaseTag(FRAME_ERROR), m_type(0), m_imessage(0)
{
}

DBGResponseTagError::DBGResponseTagError(char* responseBuffer)
  : DBGBaseTag(FRAME_ERROR)
{
  m_type      = DBGNetHelper::fromNetwork(responseBuffer, 0);
  m_imessage  = DBGNetHelper::fromNetwork(responseBuffer, 4);
}

DBGResponseTagError::~DBGResponseTagError()
{
}

void DBGResponseTagError::setType(int type)
{
  m_type = type;
}

void DBGResponseTagError::setImessage(int imessage)
{
  m_imessage = imessage;
}


int DBGResponseTagError::type()
{
  return m_type;
}

int DBGResponseTagError::imessage()
{
  return m_imessage;
}

/****************************
 * DBGRequestTagEval
 **************************/

DBGRequestTagEval::DBGRequestTagEval()
  : DBGRequestTag(FRAME_EVAL), m_istr(0), m_scopeId(0)
{
}

DBGRequestTagEval::DBGRequestTagEval(int istr, int scopeId)
  : DBGRequestTag(FRAME_EVAL)
{
  m_istr = istr;
  m_scopeId = scopeId;
}

DBGRequestTagEval::~DBGRequestTagEval()
{
}

void DBGRequestTagEval::setIstr(int istr)
{
  m_istr = istr;
}

void DBGRequestTagEval::setScopeId(int scopeId)
{
  m_scopeId = scopeId;
}

int DBGRequestTagEval::istr()
{
  return m_istr;
}
int DBGRequestTagEval::scopeId()
{
  return m_scopeId;
}

char* DBGRequestTagEval::toArray()
{
  clearRawData();
  m_raw = new char[tagSize()];

  m_raw = DBGNetHelper::toNetwork(m_istr     , m_raw, 0);
  m_raw = DBGNetHelper::toNetwork(m_scopeId  , m_raw, 4);

  return m_raw;
}


/****************************
 * DBGTagBreakpoint
 **************************/

DBGTagBreakpoint::DBGTagBreakpoint()
 : DBGRequestTag(FRAME_BPS), m_mod_no(0), m_line_no(0), m_imod_name(0), m_state(0)
     , m_istemp(0), m_hitcount(0), m_skiphits(0), m_icondition(0), m_bp_no(0), m_isunderhit(0)

{
}

DBGTagBreakpoint::DBGTagBreakpoint(char* responseBuffer)
  : DBGRequestTag(FRAME_BPS)
{
  m_mod_no      = DBGNetHelper::fromNetwork(responseBuffer, 0);
  m_line_no     = DBGNetHelper::fromNetwork(responseBuffer, 4);
  m_imod_name   = DBGNetHelper::fromNetwork(responseBuffer, 8);
  m_state       = DBGNetHelper::fromNetwork(responseBuffer, 12);
  m_istemp      = DBGNetHelper::fromNetwork(responseBuffer, 16);
  m_hitcount    = DBGNetHelper::fromNetwork(responseBuffer, 20);
  m_skiphits    = DBGNetHelper::fromNetwork(responseBuffer, 24);
  m_icondition  = DBGNetHelper::fromNetwork(responseBuffer, 28);
  m_bp_no       = DBGNetHelper::fromNetwork(responseBuffer, 32);
  m_isunderhit  = DBGNetHelper::fromNetwork(responseBuffer, 36);
}

DBGTagBreakpoint::DBGTagBreakpoint(int modno, int lineno, int imodname, int state, int istemp, int hitcount,
                  int skiphits, int icondition, int bp_no, int isunderhit)
  : DBGRequestTag(FRAME_BPS)
{
  m_mod_no = modno;
  m_line_no = lineno;
  m_imod_name = imodname;
  m_state = state;
  m_istemp = istemp;
  m_hitcount = hitcount;
  m_skiphits = skiphits;
  m_icondition = icondition;
  m_bp_no = bp_no;
  m_isunderhit = isunderhit;
}

DBGTagBreakpoint::~DBGTagBreakpoint()
{
}

void DBGTagBreakpoint::setModNo(int modno)
{
  m_mod_no = modno;
}
void DBGTagBreakpoint::setLineNo(int lineno)
{
  m_line_no = lineno;
}
void DBGTagBreakpoint::setImodName(int imodname)
{
  m_imod_name = imodname;
}
void DBGTagBreakpoint::setState(int state)
{
  m_state = state;
}
void DBGTagBreakpoint::setIsTemp(int istemp)
{
  m_istemp = istemp;
}
void DBGTagBreakpoint::setHitCount(int hitCount)
{
  m_hitcount = hitCount;
}
void DBGTagBreakpoint::setSkipHits(int skipHits)
{
  m_skiphits = skipHits;
}
void DBGTagBreakpoint::setIcondition(int icondition)
{
  m_icondition = icondition;
}
void DBGTagBreakpoint::setBpNo(int bpno)
{
  m_bp_no = bpno;
}
void DBGTagBreakpoint::setIsUnderHit(int isunderhit)
{
  m_isunderhit = isunderhit;
}
int DBGTagBreakpoint::modNo()
{
  return m_mod_no;
}
int DBGTagBreakpoint::lineNo()
{
  return m_line_no;
}

int DBGTagBreakpoint::imodname()
{
  return m_imod_name;
}
int DBGTagBreakpoint::state()
{
  return m_state;
}
int DBGTagBreakpoint::isTemp()
{
  return m_istemp;
}
int DBGTagBreakpoint::hitCount()
{
  return m_hitcount;
}
int DBGTagBreakpoint::skipHits()
{
  return m_skiphits;
}
int DBGTagBreakpoint::icondition()
{
  return m_icondition;
}
int DBGTagBreakpoint::bpNo()
{
  return m_bp_no;
}
int DBGTagBreakpoint::isUnderHit()
{
  return m_isunderhit;
}

char* DBGTagBreakpoint::toArray()
{
  clearRawData();
  m_raw = new char[tagSize()];

  m_raw = DBGNetHelper::toNetwork(m_mod_no     , m_raw , 0);
  m_raw = DBGNetHelper::toNetwork(m_line_no    , m_raw , 4);
  m_raw = DBGNetHelper::toNetwork(m_imod_name  , m_raw , 8);
  m_raw = DBGNetHelper::toNetwork(m_state      , m_raw , 12);
  m_raw = DBGNetHelper::toNetwork(m_istemp     , m_raw , 16);
  m_raw = DBGNetHelper::toNetwork(m_hitcount   , m_raw , 20);
  m_raw = DBGNetHelper::toNetwork(m_skiphits   , m_raw , 24);
  m_raw = DBGNetHelper::toNetwork(m_icondition , m_raw , 28);
  m_raw = DBGNetHelper::toNetwork(m_bp_no      , m_raw , 32);
  m_raw = DBGNetHelper::toNetwork(m_isunderhit , m_raw , 36);

  return m_raw;
}

/****************************
 * DBGRequestTagBpl
 **************************/

DBGRequestTagBpl::DBGRequestTagBpl()
  : DBGRequestTag(FRAME_BPL), m_bp_no(0)
{
}

DBGRequestTagBpl::DBGRequestTagBpl(int bpNo)
  : DBGRequestTag(FRAME_BPL)
{
  m_bp_no = bpNo;
}

DBGRequestTagBpl::~DBGRequestTagBpl()
{
}

void DBGRequestTagBpl::setBpNo(int no)
{
  m_bp_no = no;
}

int DBGRequestTagBpl::bpNo()
{
  return m_bp_no;
}

char* DBGRequestTagBpl::toArray()
{
  clearRawData();
  m_raw = new char[tagSize()];

  m_raw = DBGNetHelper::toNetwork(m_bp_no     , m_raw , 0);
  return m_raw;
}

/****************************
 * DBGRequestTagOptions
 **************************/

DBGRequestTagOptions::DBGRequestTagOptions()
  : DBGRequestTag(FRAME_SET_OPT), m_opt_flags(0)
{
}

DBGRequestTagOptions::DBGRequestTagOptions(int op)
  : DBGRequestTag(FRAME_SET_OPT), m_opt_flags(op)
{
}
DBGRequestTagOptions::~DBGRequestTagOptions()
{
}

void DBGRequestTagOptions::setOp(int op) {
  m_opt_flags = op;
}

int DBGRequestTagOptions::op() {
  return m_opt_flags;
}

char* DBGRequestTagOptions::toArray() {
  m_raw = new char[tagSize()];

  m_raw = DBGNetHelper::toNetwork(m_opt_flags, m_raw , 0);
  return m_raw;
}
