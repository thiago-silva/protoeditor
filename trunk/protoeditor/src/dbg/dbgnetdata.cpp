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

#include "dbgnetdata.h"
#include "dbgnet.h"
#include "dbgresponsepack.h"

/*************************** Receivable *******************************/

Receivable::Receivable()
{}
Receivable::~Receivable()
{}

dbgint Receivable::fromNetwork(char* data, dbgint offset)  const
{
  return ((data[offset]   & 0xff) << 24)
         | ((data[offset+1] & 0xff) << 16)
         | ((data[offset+2] & 0xff) << 8)
         | (data[offset+3]  & 0xff);
}

/****************************** Requestable ***************************/

Requestable::Requestable()
    : m_raw(0L)
{}

Requestable::~Requestable()
{
  clearRawData();
}

char* Requestable::toNetwork(dbgint num, char* buffer, dbgint offset) const
{
  buffer[offset] =   (0xff & (num >> 24));
  buffer[offset+1] = (0xff & (num >> 16));
  buffer[offset+2] = (0xff & (num >> 8 ));
  buffer[offset+3] = 0xff & num;
  return buffer;
}

void  Requestable::clearRawData()
{
  delete [] m_raw;
  m_raw = 0L;
};

/********************************** HEADER ******************************/

const dbgint DBGHeader::SIZE = 16;

DBGHeader::DBGHeader()
    : Receivable(), Requestable(), m_sync(DBGSYNC), m_cmd(0), m_flags(0), m_bodysize(0)
{}
DBGHeader::~DBGHeader()
{}

DBGHeader::DBGHeader(char* responseBuffer)
    : Receivable(), Requestable()
{
  m_sync     = fromNetwork(responseBuffer,0);
  m_cmd      = fromNetwork(responseBuffer,4);
  m_flags    = fromNetwork(responseBuffer,8);
  m_bodysize = fromNetwork(responseBuffer,12);
}

void DBGHeader::setSync(dbgint sync)
{
  m_sync = sync;
}

void DBGHeader::setCmd(dbgint cmd)
{
  m_cmd = cmd;
}

void DBGHeader::setFlags(dbgint flags)
{
  m_flags = flags;
}

void DBGHeader::setBodysize(dbgint bodysize)
{
  m_bodysize = bodysize;
}

dbgint DBGHeader::sync() const
{
  return m_sync;
}

dbgint DBGHeader::cmd() const
{
  return m_cmd;
}

dbgint DBGHeader::flags() const
{
  return m_flags;
}

dbgint DBGHeader::bodysize() const
{
  return m_bodysize;
}

char* DBGHeader::toArray()
{
  clearRawData();
  m_raw = new char[DBGHeader::SIZE];

  m_raw = toNetwork(m_sync    , m_raw, 0);
  m_raw = toNetwork(m_cmd     , m_raw, 4);
  m_raw = toNetwork(m_flags   , m_raw, 8);
  m_raw = toNetwork(m_bodysize, m_raw, 12);

  return m_raw;
}

/*********************************** FRAME ********************************/

const dbgint DBGFrame::SIZE = 8;

DBGFrame::DBGFrame()
    :  Receivable(), Requestable(), m_name(0), m_datasize(0)
{}
DBGFrame::DBGFrame(dbgint name, dbgint datasize)
    : Receivable(), Requestable()
{
  m_name = name;
  m_datasize = datasize;
}

DBGFrame::DBGFrame(char* responseBuffer)
    : Receivable(), Requestable()
{
  m_name     = fromNetwork(responseBuffer, 0);
  m_datasize = fromNetwork(responseBuffer, 4);
}

DBGFrame::~DBGFrame()
{}

void DBGFrame::setName(dbgint name)
{
  m_name = name;
}

void DBGFrame::setDatasize(dbgint datasize)
{
  m_datasize = datasize;
}

dbgint DBGFrame::name() const
{
  return m_name;
}

dbgint DBGFrame::datasize() const
{
  return m_datasize;
}

char* DBGFrame::toArray()
{
  clearRawData();
  m_raw = new char[DBGFrame::SIZE];

  m_raw = toNetwork(m_name     , m_raw, 0);
  m_raw = toNetwork(m_datasize , m_raw, 4);

  return m_raw;
}

/************************ TAGS ************************************/

/*************************************************************
 * DBGTagBase
 *************************************************************/

DBGBaseTag::DBGBaseTag(dbgint frameName)
    : m_frameName(frameName)
{}

DBGBaseTag::~DBGBaseTag()
{}

void DBGBaseTag::setName(dbgint frameName)
{
  m_frameName = frameName;
}

dbgint DBGBaseTag::name() const
{
  return m_frameName;
}

/*************************************************************
 * DBGRequestTag
 *************************************************************/

DBGRequestTag::DBGRequestTag(dbgint frameName)
    : DBGBaseTag(frameName)
{

  //size is only needed for the requestable tags

  switch(frameName)
  {
    case FRAME_RAWDATA:
      setTagSize(TAG_RAWDATA_SIZE);
      break;
    case FRAME_SOURCE:
      setTagSize(TAG_REQ_SOURCE_SIZE);
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
      setTagSize(TAG_REQ_SRCLINESINFO_SIZE);
      break;
    case FRAME_SRCCTXINFO:
      setTagSize(TAG_REQ_SRCCTXINFO_SIZE);
      break;
    case FRAME_PROF:
      setTagSize(TAG_REQ_PROF_SIZE);
      break;
    case FRAME_PROF_C:
      setTagSize(TAG_REQ_PROF_C_SIZE);
      break;
    case FRAME_SET_OPT:
      setTagSize(TAG_REQ_SET_OPT_SIZE);
      break;
  }

}

DBGRequestTag::~DBGRequestTag()
{}

void DBGRequestTag::setTagSize(long tagSize)
{
  m_tagSize = tagSize;
}

long DBGRequestTag::tagSize() const
{
  return m_tagSize;
}

/*************************************************************
 * DBGResponseTag
 *************************************************************/

DBGResponseTag::DBGResponseTag(dbgint frameName)
    : DBGBaseTag(frameName)
{}

DBGResponseTag::~DBGResponseTag()
{}
;

/*************************************************************
 * DBGTagRawdata
 *************************************************************/

DBGTagRawdata::DBGTagRawdata()
    : DBGBaseTag(FRAME_RAWDATA),
    DBGRequestTag(FRAME_RAWDATA),
    DBGResponseTag(FRAME_RAWDATA),
    m_id(0), m_size(0), m_data(0)
{}

DBGTagRawdata::DBGTagRawdata(dbgint id, const char* data, long datasize)
    : DBGBaseTag(FRAME_RAWDATA),
    DBGRequestTag(FRAME_RAWDATA),
    DBGResponseTag(FRAME_RAWDATA)
{
  m_id = id;

  dbgint mod = datasize % NET_SIZE_T;
  m_data = new char[datasize + mod];
  //m_data = (char*) data;

  dbgint i;
  for(i = 0; i < datasize; i++)
  {
    m_data[i] = data[i];
  }

  m_size = datasize + mod;

  for( ; i < m_size; i++)
  {
    m_data[i] = 0;
  }
}

DBGTagRawdata::DBGTagRawdata(char* responseBuffer)
    : DBGBaseTag(FRAME_RAWDATA),
    DBGRequestTag(FRAME_RAWDATA),
    DBGResponseTag(FRAME_RAWDATA)
{
  m_id   = fromNetwork(responseBuffer, 0);
  m_size = fromNetwork(responseBuffer, 4);

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

void DBGTagRawdata::setId(dbgint id)
{
  m_id = id;
}

void DBGTagRawdata::setData(char* data, long size)
{
  delete [] m_data;
  m_size = size;

  m_data = new char[m_size];
  for(long i = 0; i < m_size; i++)
  {
    m_data[i] = data[i];
  }
  //m_data = data;
}

dbgint DBGTagRawdata::id() const
{
  return m_id;
}
long DBGTagRawdata::size() const
{
  return m_size;
}
char* DBGTagRawdata::data() const
{
  return m_data;
}

long DBGTagRawdata::tagSize() const
{
  return m_tagSize + m_size + (m_size % 4);
}

char* DBGTagRawdata::toArray()
{
  clearRawData();

  long bsize = tagSize();
  m_raw = new char[bsize];

  m_raw = toNetwork(m_id  , m_raw, 0);
  m_raw = toNetwork(m_size, m_raw, 4);

  long i = 8;
  for(long j = 0; j < m_size; i++, j++)
  {
    m_raw[i] = m_data[j];
  }

  for(; i < bsize; i++)
  {
    m_raw[i] = 0;
  }

  return m_raw;
}

void DBGTagRawdata::process(DBGNet*, DBGResponsePack*) const
{
  //empty
}

/*************************************************************
 * DBGResponseTagSid
 *************************************************************/

DBGResponseTagSid::DBGResponseTagSid()
    : DBGBaseTag(FRAME_SID), DBGResponseTag(FRAME_SID),
    m_isid(0), m_sesstype(0)
{}

DBGResponseTagSid::DBGResponseTagSid(char* responseBuffer)
    : DBGBaseTag(FRAME_SID), DBGResponseTag(FRAME_SID)
{
  m_isid     = fromNetwork(responseBuffer, 0);
  m_sesstype = fromNetwork(responseBuffer, 4);
}

DBGResponseTagSid::~DBGResponseTagSid()
{}

void DBGResponseTagSid::setIsid(dbgint isid)
{
  m_isid = isid;
}
void DBGResponseTagSid::setSesstype(dbgint sesstype)
{
  m_sesstype = sesstype;
}
dbgint DBGResponseTagSid::isid() const
{
  return m_isid;
}
dbgint DBGResponseTagSid::sesstype() const
{
  return m_sesstype;
}

void DBGResponseTagSid::process(DBGNet* net, DBGResponsePack* pack) const
{
  //net->processSessionId(m_sesstype, pack->retrieveRawdata(m_isid)->data());
  net->processSessionId(this, pack);
}

/*************************************************************
 * DBGResponseTagStack
 *************************************************************/

DBGResponseTagStack::DBGResponseTagStack()
    : DBGBaseTag(FRAME_STACK), DBGResponseTag(FRAME_STACK),
    m_line_no(0), m_mod_no(0), m_scope_id(0), m_idescr(0)
{}

DBGResponseTagStack::DBGResponseTagStack(char* responseBuffer)
    : DBGBaseTag(FRAME_STACK), DBGResponseTag(FRAME_STACK)
{
  m_line_no  = fromNetwork(responseBuffer, 0);
  m_mod_no   = fromNetwork(responseBuffer, 4);
  m_scope_id = fromNetwork(responseBuffer, 8);
  m_idescr   = fromNetwork(responseBuffer, 12);
}

DBGResponseTagStack::~DBGResponseTagStack()
{}

void DBGResponseTagStack::setLineNo(dbgint line_no)
{
  m_line_no = line_no;
}
void DBGResponseTagStack::setModNo(dbgint mod_no)
{
  m_mod_no = mod_no;
}
void DBGResponseTagStack::setScopeId(dbgint scope_id)
{
  m_scope_id = scope_id;
}
void DBGResponseTagStack::setIdescr(dbgint idescr)
{
  m_idescr = idescr;
}
dbgint DBGResponseTagStack::lineNo() const
{
  return m_line_no;
}
dbgint DBGResponseTagStack::modNo() const
{
  return m_mod_no;
}
dbgint DBGResponseTagStack::scopeId() const
{
  return m_scope_id;
}
dbgint DBGResponseTagStack::idescr() const
{
  return m_idescr;
}

void DBGResponseTagStack::process(DBGNet* net, DBGResponsePack* pack) const
{
  //net->processStack(m_mod_no,
  //                  pack->retrieveRawdata(m_idescr)->data(),m_line_no, m_scope_id);
  net->processStack(this, pack);
}

/*************************************************************
 * DBGResponseTagVersion
 *************************************************************/

DBGResponseTagVersion::DBGResponseTagVersion()
    : DBGBaseTag(FRAME_VER), DBGResponseTag(FRAME_VER),
    m_major_version(0), m_minor_version(0), m_idescription(0)
{}
DBGResponseTagVersion::DBGResponseTagVersion(char* responseBuffer)
    : DBGBaseTag(FRAME_VER), DBGResponseTag(FRAME_VER)
{
  m_major_version = fromNetwork(responseBuffer, 0);
  m_minor_version = fromNetwork(responseBuffer, 4);
  m_idescription  = fromNetwork(responseBuffer, 8);
}

DBGResponseTagVersion::~DBGResponseTagVersion()
{}
void DBGResponseTagVersion::setMajorVersion(dbgint major_version)
{
  m_major_version = major_version;
}
void DBGResponseTagVersion::setMinorVersion(dbgint minor_version)
{
  m_minor_version = minor_version;
}
void DBGResponseTagVersion::setIdescription(dbgint idescription)
{
  m_idescription = idescription;
}
dbgint DBGResponseTagVersion::majorVersion() const
{
  return m_major_version;
}
dbgint DBGResponseTagVersion::minorVersion() const
{
  return m_minor_version;
}
dbgint DBGResponseTagVersion::idescription() const
{
  return m_idescription;
}

void DBGResponseTagVersion::process(DBGNet* net, DBGResponsePack* pack) const
{
  //net->processDBGVersion(m_major_version, m_minor_version,
  //                       pack->retrieveRawdata(m_idescription)->data());
  net->processDBGVersion(this, pack);
}

/*************************************************************
 * DBGResponseTagSrcLinesInfo
 *************************************************************/

DBGResponseTagSrcTree::DBGResponseTagSrcTree()
    : DBGBaseTag(FRAME_SRC_TREE), DBGResponseTag(FRAME_SRC_TREE),
    m_parent_mod_no(0), m_parent_line_no(0), m_mod_no(0), m_imod_name(0)
{}
DBGResponseTagSrcTree::DBGResponseTagSrcTree(char* responseBuffer)
    : DBGBaseTag(FRAME_SRC_TREE), DBGResponseTag(FRAME_SRC_TREE)
{
  m_parent_mod_no  = fromNetwork(responseBuffer, 0);
  m_parent_line_no = fromNetwork(responseBuffer, 4);
  m_mod_no         = fromNetwork(responseBuffer, 8);
  m_imod_name      = fromNetwork(responseBuffer, 12);
}

DBGResponseTagSrcTree::~DBGResponseTagSrcTree()
{}

void DBGResponseTagSrcTree::setParentModNo(dbgint parentmodno)
{
  m_parent_mod_no = parentmodno;
}

void DBGResponseTagSrcTree::setParentLineNo(dbgint parentLineNo)
{
  m_parent_line_no = parentLineNo;
}

void DBGResponseTagSrcTree::setModNo(dbgint modno)
{
  m_mod_no = modno;
}

void DBGResponseTagSrcTree::setImodName(dbgint imodname)
{
  m_imod_name = imodname;
}

dbgint DBGResponseTagSrcTree::parentModNo() const
{
  return m_parent_mod_no;
}

dbgint DBGResponseTagSrcTree::parentLineNo() const
{
  return m_parent_line_no;
}

dbgint DBGResponseTagSrcTree::modNo() const
{
  return m_mod_no;
}

dbgint DBGResponseTagSrcTree::imodName() const
{
  return m_imod_name;
}

void DBGResponseTagSrcTree::process(DBGNet* net, DBGResponsePack* pack) const
{
  //net->processSrcTree(m_mod_no, pack->retrieveRawdata(m_imod_name)->data());
  net->processSrcTree(this, pack);
}


/****************************
 * DBGResponseTagSrcLinesInfo
 **************************/
DBGResponseTagSrcLinesInfo::DBGResponseTagSrcLinesInfo()
  : DBGBaseTag(FRAME_SRCLINESINFO), DBGResponseTag(FRAME_SRCLINESINFO),
    m_mod_no(0),
    m_start_line_no(0),
    m_lines_count(0),
    m_ctx_id(0)
    
{
}

DBGResponseTagSrcLinesInfo::DBGResponseTagSrcLinesInfo(char* responseBuffer)
  : DBGBaseTag(FRAME_SRCLINESINFO), DBGResponseTag(FRAME_SRCLINESINFO)
{
  m_mod_no         = fromNetwork(responseBuffer, 0);
  m_start_line_no = fromNetwork(responseBuffer, 4);
  m_lines_count   = fromNetwork(responseBuffer, 8);
  m_ctx_id        = fromNetwork(responseBuffer, 12);
  
}

DBGResponseTagSrcLinesInfo::~DBGResponseTagSrcLinesInfo()
{
}

void DBGResponseTagSrcLinesInfo::setModNo(dbgint modno)
{
  m_mod_no = modno;
}

void DBGResponseTagSrcLinesInfo::setStartLineNo(dbgint startline)
{
  m_start_line_no = startline;
}

void DBGResponseTagSrcLinesInfo::setLinesCount(dbgint linescount)
{
  m_lines_count = linescount;
}

void DBGResponseTagSrcLinesInfo::setCtxId(dbgint ctxid)
{
  m_ctx_id = ctxid;
}

dbgint DBGResponseTagSrcLinesInfo::modNo() const
{
  return m_mod_no;
}

dbgint DBGResponseTagSrcLinesInfo::startLineNo() const
{
  return m_start_line_no;
}

dbgint DBGResponseTagSrcLinesInfo::linesCount() const
{
  return m_lines_count;
}

dbgint DBGResponseTagSrcLinesInfo::ctxId() const
{
  return m_ctx_id;
}
  
void DBGResponseTagSrcLinesInfo::process(DBGNet* net, DBGResponsePack* pack) const
{
  net->processSrcLinesInfo(this, pack);
}

/****************************
 * DBGResponseTagSrcCtxInfo
 **************************/
DBGResponseTagSrcCtxInfo::DBGResponseTagSrcCtxInfo()
  : DBGBaseTag(FRAME_SRCCTXINFO), DBGResponseTag(FRAME_SRCCTXINFO),
    m_modno(0),
    m_ctxid(0),
    m_ifunctionName(0)
{
}

DBGResponseTagSrcCtxInfo::DBGResponseTagSrcCtxInfo(char* responseBuffer)
  : DBGBaseTag(FRAME_SRCCTXINFO), DBGResponseTag(FRAME_SRCCTXINFO)
{
  m_modno         = fromNetwork(responseBuffer, 0);
  m_ctxid         = fromNetwork(responseBuffer, 4);
  m_ifunctionName = fromNetwork(responseBuffer, 8);
}

DBGResponseTagSrcCtxInfo::~DBGResponseTagSrcCtxInfo()
{
}

void DBGResponseTagSrcCtxInfo::setModNo(dbgint modno)
{
  m_modno = modno;
}

void DBGResponseTagSrcCtxInfo::setCtxId(dbgint ctxid)
{
  m_ctxid = ctxid;
}

void DBGResponseTagSrcCtxInfo::setIFunctionName(dbgint ifunctionName)
{
  m_ifunctionName = ifunctionName;
}

dbgint DBGResponseTagSrcCtxInfo::modno() const
{
  return m_modno;
}

dbgint DBGResponseTagSrcCtxInfo::ctxid() const
{
  return m_ctxid;
}

dbgint DBGResponseTagSrcCtxInfo::ifunctionName() const
{
  return m_ifunctionName;
}

void DBGResponseTagSrcCtxInfo::process(DBGNet* net, DBGResponsePack* pack) const
{
  net->processSrcCtxInfo(this, pack);
}


/****************************
 * DBGResponseTagEval
 **************************/

DBGResponseTagEval::DBGResponseTagEval()
    : DBGBaseTag(FRAME_EVAL), DBGResponseTag(FRAME_EVAL),
    m_istr(0), m_iresult(0), m_ierror(0)
{}

DBGResponseTagEval::DBGResponseTagEval(char* responseBuffer)
    : DBGBaseTag(FRAME_EVAL), DBGResponseTag(FRAME_EVAL)
{
  m_istr    = fromNetwork(responseBuffer, 0);
  m_iresult = fromNetwork(responseBuffer, 4);
  m_ierror  = fromNetwork(responseBuffer, 8);
}

DBGResponseTagEval::~DBGResponseTagEval()
{}

void DBGResponseTagEval::setIstr(dbgint istr)
{
  m_istr = istr;
}
void DBGResponseTagEval::setIresult(dbgint iresult)
{
  m_iresult = iresult;
}
void DBGResponseTagEval::setIerror(dbgint ierror)
{
  m_ierror = ierror;
}
dbgint DBGResponseTagEval::istr() const
{
  return m_istr;
}
dbgint DBGResponseTagEval::iresult() const
{
  return m_iresult;
}
dbgint DBGResponseTagEval::ierror() const
{
  return m_ierror;
}

void DBGResponseTagEval::process(DBGNet* net, DBGResponsePack* pack) const
{
  net->processEval(this, pack);
}


/****************************
 * DBGResponseTagLog
 **************************/
DBGResponseTagLog::DBGResponseTagLog()
    : DBGBaseTag(FRAME_LOG), DBGResponseTag(FRAME_LOG),
    m_ilog(0), m_type(0), m_modNo(0),
    m_lineNo(0), m_imodName(0), m_extInfo(0)
{}

DBGResponseTagLog::DBGResponseTagLog(char* responseBuffer)
    : DBGBaseTag(FRAME_LOG), DBGResponseTag(FRAME_LOG)
{
  m_ilog     = fromNetwork(responseBuffer, 0);
  m_type     = fromNetwork(responseBuffer, 4);
  m_modNo    = fromNetwork(responseBuffer, 8);
  m_lineNo   = fromNetwork(responseBuffer, 12);
  m_imodName = fromNetwork(responseBuffer, 16);
  m_extInfo  = fromNetwork(responseBuffer, 20);
}

DBGResponseTagLog::~DBGResponseTagLog()
{}

void DBGResponseTagLog::setIlog(dbgint ilog)
{
  m_ilog = ilog;
}
void DBGResponseTagLog::setType(dbgint type)
{
  m_type = type;
}

void DBGResponseTagLog::setModNo(dbgint modno)
{
  m_modNo = modno;
}

void DBGResponseTagLog::setLineNo(dbgint lineno)
{
  m_lineNo = lineno;
}

void DBGResponseTagLog::setImodName(dbgint imodname)
{
  m_imodName = imodname;
}

void DBGResponseTagLog::setExtInfo(dbgint extinfo)
{
  m_extInfo = extinfo;
}

dbgint DBGResponseTagLog::ilog() const
{
  return m_ilog;
}

dbgint DBGResponseTagLog::type() const
{
  return m_type;
}

dbgint DBGResponseTagLog::modNo() const
{
  return m_modNo;
}

dbgint DBGResponseTagLog::lineNo() const
{
  return m_lineNo;
}

dbgint DBGResponseTagLog::imodName() const
{
  return m_imodName;
}

dbgint DBGResponseTagLog::extInfo() const
{
  return m_extInfo;
}

void DBGResponseTagLog::process(DBGNet* net, DBGResponsePack* pack) const
{
  net->processLog(this, pack);
}

/****************************
 * DBGResponseTagError
 **************************/

DBGResponseTagError::DBGResponseTagError()
    : DBGBaseTag(FRAME_ERROR), DBGResponseTag(FRAME_ERROR),
    m_type(0), m_imessage(0)
{}

DBGResponseTagError::DBGResponseTagError(char* responseBuffer)
    : DBGBaseTag(FRAME_ERROR), DBGResponseTag(FRAME_ERROR)
{
  m_type      = fromNetwork(responseBuffer, 0);
  m_imessage  = fromNetwork(responseBuffer, 4);
}

DBGResponseTagError::~DBGResponseTagError()
{}

void DBGResponseTagError::setType(dbgint type)
{
  m_type = type;
}

void DBGResponseTagError::setImessage(dbgint imessage)
{
  m_imessage = imessage;
}


dbgint DBGResponseTagError::type() const
{
  return m_type;
}

dbgint DBGResponseTagError::imessage() const
{
  return m_imessage;
}

void DBGResponseTagError::process(DBGNet* net, DBGResponsePack* pack) const
{
  net->processError(this, pack);
}

/****************************
 * DBGResponseTagProf
 **************************/
DBGResponseTagProf::DBGResponseTagProf()
    : DBGBaseTag(FRAME_PROF), DBGResponseTag(FRAME_PROF),
    m_modNo(0), m_lineNo(0), m_hitCount(0), m_minLo(0), m_minHi(0), m_maxLo(0), m_maxHi(0),
    m_sumLo(0), m_sumHi(0)
{}

DBGResponseTagProf::DBGResponseTagProf(char* responseBuffer)
    : DBGBaseTag(FRAME_PROF), DBGResponseTag(FRAME_PROF)
{
  m_modNo = fromNetwork(responseBuffer, 0);
  m_lineNo = fromNetwork(responseBuffer, 4);
  m_hitCount = fromNetwork(responseBuffer, 8);
  m_minLo = fromNetwork(responseBuffer, 12);
  m_minHi = fromNetwork(responseBuffer, 16);
  m_maxLo = fromNetwork(responseBuffer, 20);
  m_maxHi = fromNetwork(responseBuffer, 24);
  m_sumLo = fromNetwork(responseBuffer, 28);
  m_sumHi = fromNetwork(responseBuffer, 32);
}

DBGResponseTagProf::~DBGResponseTagProf()
{}

void DBGResponseTagProf::setModNo(dbgint modno)
{
  m_modNo = modno;
}
void DBGResponseTagProf::setLineNo(dbgint lineno)
{
  m_lineNo = lineno;
}

void DBGResponseTagProf::setHitCount(dbgint hitcount)
{
  m_hitCount = hitcount;
}
void DBGResponseTagProf::setMinLo(dbgint minlo)
{
  m_minLo = minlo;
}
void DBGResponseTagProf::setMinHi(dbgint minhi)
{
  m_minHi = minhi;
}
void DBGResponseTagProf::setMaxLo(dbgint maxlo)
{
  m_maxLo = maxlo;
}
void DBGResponseTagProf::setMaxHi(dbgint maxhi)
{
  m_maxHi = maxhi;
}
void DBGResponseTagProf::setSumLo(dbgint sumlo)
{
  m_sumLo = sumlo;
}

void DBGResponseTagProf::setSumHi(dbgint sumhi)
{
  m_sumHi = sumhi;
}

dbgint DBGResponseTagProf::modNo() const
{
  return m_modNo;
}

dbgint DBGResponseTagProf::lineNo() const
{
  return m_lineNo;
}
dbgint DBGResponseTagProf::hitCount() const
{
  return m_hitCount;
}

dbgint DBGResponseTagProf::minLo() const
{
  return m_minLo;
}
dbgint DBGResponseTagProf::minHi() const
{
  return m_minHi;
}
dbgint DBGResponseTagProf::maxLo() const
{
  return m_maxLo;
}
dbgint DBGResponseTagProf::maxHi() const
{
  return m_maxHi;
}
dbgint DBGResponseTagProf::sumLo() const
{
  return m_sumLo;
}
dbgint DBGResponseTagProf::sumHi() const
{
  return m_sumHi;
}

void DBGResponseTagProf::process(DBGNet* net, DBGResponsePack* pack) const
{
  net->processProf(this, pack);
}

/****************************
 * DBGResponseTagProfC
 **************************/
DBGResponseTagProfC::DBGResponseTagProfC()
  : DBGBaseTag(FRAME_PROF_C), DBGResponseTag(FRAME_PROF_C),
  m_freqlo(0),
  m_freqhi(0),
  m_diffmin(0),
  m_diffmax(0),
  m_diffm(0)
{
}

DBGResponseTagProfC::DBGResponseTagProfC(char* responseBuffer)
  : DBGBaseTag(FRAME_PROF_C), DBGResponseTag(FRAME_PROF_C)
{
  m_freqlo = fromNetwork(responseBuffer, 0);
  m_freqhi = fromNetwork(responseBuffer, 4);
  m_diffmin = fromNetwork(responseBuffer, 8);
  m_diffmax = fromNetwork(responseBuffer, 12);
  m_diffm = fromNetwork(responseBuffer, 16);
}

DBGResponseTagProfC::~DBGResponseTagProfC()
{
}

void DBGResponseTagProfC::setFreqLo(dbgint freqlo)
{
  m_freqlo = freqlo;
}

void DBGResponseTagProfC::setFreqHi(dbgint freqhi)
{
  m_freqhi = freqhi;
}

void DBGResponseTagProfC::setDiffMin(dbgint diffmin)
{
  m_diffmin = diffmin;
}

void DBGResponseTagProfC::setDiffMax(dbgint diffmax)
{
  m_diffmax = diffmax;
}

void DBGResponseTagProfC::setDiffM(dbgint diffm)
{
  m_diffm = diffm;
}

dbgint DBGResponseTagProfC::freqlo() const
{
  return m_freqlo;
}

dbgint DBGResponseTagProfC::freqhi() const
{
  return m_freqhi;
}

dbgint DBGResponseTagProfC::diffmin() const
{
  return m_diffmin;
}

dbgint DBGResponseTagProfC::diffmax() const
{
  return m_diffmax;
}

dbgint DBGResponseTagProfC::diffm() const
{
  return m_diffm;
}

void DBGResponseTagProfC::process(DBGNet* net, DBGResponsePack* pack) const
{
  net->processProfC(this, pack);
}
  
/****************************
 * DBGRequestTagEval
 **************************/

DBGRequestTagEval::DBGRequestTagEval()
    : DBGBaseTag(FRAME_EVAL), DBGRequestTag(FRAME_EVAL),
    m_istr(0), m_scopeId(0)
{}

DBGRequestTagEval::DBGRequestTagEval(dbgint istr, dbgint scopeId)
    : DBGBaseTag(FRAME_EVAL), DBGRequestTag(FRAME_EVAL)
{
  m_istr = istr;
  m_scopeId = scopeId;
}

DBGRequestTagEval::~DBGRequestTagEval()
{}

void DBGRequestTagEval::setIstr(dbgint istr)
{
  m_istr = istr;
}

void DBGRequestTagEval::setScopeId(dbgint scopeId)
{
  m_scopeId = scopeId;
}

dbgint DBGRequestTagEval::istr() const
{
  return m_istr;
}
dbgint DBGRequestTagEval::scopeId() const
{
  return m_scopeId;
}

char* DBGRequestTagEval::toArray()
{
  clearRawData();
  m_raw = new char[tagSize()];

  m_raw = toNetwork(m_istr     , m_raw, 0);
  m_raw = toNetwork(m_scopeId  , m_raw, 4);

  return m_raw;
}


/****************************
 * DBGTagBreakpoint
 **************************/

DBGTagBreakpoint::DBGTagBreakpoint()
    : DBGBaseTag(FRAME_BPS), DBGRequestTag(FRAME_BPS), DBGResponseTag(FRAME_BPS),
    m_mod_no(0), m_line_no(0), m_imod_name(0), m_state(0),
    m_istemp(0), m_hitcount(0), m_skiphits(0), m_icondition(0),
    m_bp_no(0), m_isunderhit(0)

{}

DBGTagBreakpoint::DBGTagBreakpoint(char* responseBuffer)
    : DBGBaseTag(FRAME_BPS), DBGRequestTag(FRAME_BPS), DBGResponseTag(FRAME_BPS)
{
  m_mod_no      = fromNetwork(responseBuffer, 0);
  m_line_no     = fromNetwork(responseBuffer, 4);
  m_imod_name   = fromNetwork(responseBuffer, 8);
  m_state       = fromNetwork(responseBuffer, 12);
  m_istemp      = fromNetwork(responseBuffer, 16);
  m_hitcount    = fromNetwork(responseBuffer, 20);
  m_skiphits    = fromNetwork(responseBuffer, 24);
  m_icondition  = fromNetwork(responseBuffer, 28);
  m_bp_no       = fromNetwork(responseBuffer, 32);
  m_isunderhit  = fromNetwork(responseBuffer, 36);
}

DBGTagBreakpoint::DBGTagBreakpoint(dbgint modno, dbgint lineno, dbgint imodname, dbgint state, dbgint istemp, dbgint hitcount,
                                   dbgint skiphits, dbgint icondition, dbgint bp_no, dbgint isunderhit)
    : DBGBaseTag(FRAME_BPS), DBGRequestTag(FRAME_BPS), DBGResponseTag(FRAME_BPS)
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
{}

void DBGTagBreakpoint::setModNo(dbgint modno)
{
  m_mod_no = modno;
}
void DBGTagBreakpoint::setLineNo(dbgint lineno)
{
  m_line_no = lineno;
}
void DBGTagBreakpoint::setImodName(dbgint imodname)
{
  m_imod_name = imodname;
}
void DBGTagBreakpoint::setState(dbgint state)
{
  m_state = state;
}
void DBGTagBreakpoint::setIsTemp(dbgint istemp)
{
  m_istemp = istemp;
}
void DBGTagBreakpoint::setHitCount(dbgint hitCount)
{
  m_hitcount = hitCount;
}
void DBGTagBreakpoint::setSkipHits(dbgint skipHits)
{
  m_skiphits = skipHits;
}
void DBGTagBreakpoint::setIcondition(dbgint icondition)
{
  m_icondition = icondition;
}
void DBGTagBreakpoint::setBpNo(dbgint bpno)
{
  m_bp_no = bpno;
}
void DBGTagBreakpoint::setIsUnderHit(dbgint isunderhit)
{
  m_isunderhit = isunderhit;
}
dbgint DBGTagBreakpoint::modNo() const
{
  return m_mod_no;
}
dbgint DBGTagBreakpoint::lineNo() const
{
  return m_line_no;
}

dbgint DBGTagBreakpoint::imodname() const
{
  return m_imod_name;
}
dbgint DBGTagBreakpoint::state() const
{
  return m_state;
}
dbgint DBGTagBreakpoint::isTemp() const
{
  return m_istemp;
}
dbgint DBGTagBreakpoint::hitCount() const
{
  return m_hitcount;
}
dbgint DBGTagBreakpoint::skipHits() const
{
  return m_skiphits;
}
dbgint DBGTagBreakpoint::icondition() const
{
  return m_icondition;
}
dbgint DBGTagBreakpoint::bpNo() const
{
  return m_bp_no;
}
dbgint DBGTagBreakpoint::isUnderHit() const
{
  return m_isunderhit;
}

char* DBGTagBreakpoint::toArray()
{
  clearRawData();
  m_raw = new char[tagSize()];

  m_raw = toNetwork(m_mod_no     , m_raw , 0);
  m_raw = toNetwork(m_line_no    , m_raw , 4);
  m_raw = toNetwork(m_imod_name  , m_raw , 8);
  m_raw = toNetwork(m_state      , m_raw , 12);
  m_raw = toNetwork(m_istemp     , m_raw , 16);
  m_raw = toNetwork(m_hitcount   , m_raw , 20);
  m_raw = toNetwork(m_skiphits   , m_raw , 24);
  m_raw = toNetwork(m_icondition , m_raw , 28);
  m_raw = toNetwork(m_bp_no      , m_raw , 32);
  m_raw = toNetwork(m_isunderhit , m_raw , 36);

  return m_raw;
}

void DBGTagBreakpoint::process(DBGNet* net, DBGResponsePack* pack) const
{
  net->processBreakpoint(this, pack);
}

/****************************
 * DBGRequestTagBpl
 **************************/

DBGRequestTagBpl::DBGRequestTagBpl()
    : DBGBaseTag(FRAME_BPL), DBGRequestTag(FRAME_BPL), m_bp_no(0)
{}

DBGRequestTagBpl::DBGRequestTagBpl(dbgint bpNo)
    : DBGBaseTag(FRAME_BPL), DBGRequestTag(FRAME_BPL)
{
  m_bp_no = bpNo;
}

DBGRequestTagBpl::~DBGRequestTagBpl()
{}

void DBGRequestTagBpl::setBpNo(dbgint no)
{
  m_bp_no = no;
}

dbgint DBGRequestTagBpl::bpNo() const
{
  return m_bp_no;
}

char* DBGRequestTagBpl::toArray()
{
  clearRawData();
  m_raw = new char[tagSize()];

  m_raw = toNetwork(m_bp_no     , m_raw , 0);
  return m_raw;
}

/****************************
 * DBGRequestTagOptions
 **************************/

DBGRequestTagOptions::DBGRequestTagOptions()
    : DBGBaseTag(FRAME_SET_OPT), DBGRequestTag(FRAME_SET_OPT), m_opt_flags(0)
{}

DBGRequestTagOptions::DBGRequestTagOptions(dbgint op)
    : DBGBaseTag(FRAME_SET_OPT), DBGRequestTag(FRAME_SET_OPT), m_opt_flags(op)
{}
DBGRequestTagOptions::~DBGRequestTagOptions()
{}

void DBGRequestTagOptions::setOp(dbgint op)
{
  m_opt_flags = op;
}

dbgint DBGRequestTagOptions::op() const
{
  return m_opt_flags;
}

char* DBGRequestTagOptions::toArray()
{
  m_raw = new char[tagSize()];

  m_raw = toNetwork(m_opt_flags, m_raw , 0);
  return m_raw;
}

/****************************
 * DBGRequesttagSrcLinesInfo
 **************************/

DBGRequestTagSrcLinesInfo::DBGRequestTagSrcLinesInfo()
  : DBGBaseTag(FRAME_SRCLINESINFO), DBGRequestTag(FRAME_SRCLINESINFO), m_modno(0)
{
}

DBGRequestTagSrcLinesInfo::DBGRequestTagSrcLinesInfo(dbgint modno)
  : DBGBaseTag(FRAME_SRCLINESINFO), DBGRequestTag(FRAME_SRCLINESINFO), m_modno(modno)
{
}

DBGRequestTagSrcLinesInfo::~DBGRequestTagSrcLinesInfo()
{
}
    
void DBGRequestTagSrcLinesInfo::setModNo(dbgint modno)
{
  m_modno = modno;
}

dbgint DBGRequestTagSrcLinesInfo::modno() const
{
  return m_modno;
}

char* DBGRequestTagSrcLinesInfo::toArray()
{
  m_raw = new char[tagSize()];

  m_raw = toNetwork(m_modno, m_raw , 0);
  return m_raw;  
}

/****************************
 * DBGRequesttagSrcLinesInfo
 **************************/
 
/* request tag for context data */
DBGRequestTagSrcCtxInfo::DBGRequestTagSrcCtxInfo()
  : DBGBaseTag(FRAME_SRCCTXINFO), DBGRequestTag(FRAME_SRCCTXINFO), m_modno(0)
{
}

DBGRequestTagSrcCtxInfo::DBGRequestTagSrcCtxInfo(dbgint modno)
  : DBGBaseTag(FRAME_SRCCTXINFO), DBGRequestTag(FRAME_SRCCTXINFO), m_modno(modno)
{
}

DBGRequestTagSrcCtxInfo::~DBGRequestTagSrcCtxInfo()
{
}
    
void DBGRequestTagSrcCtxInfo::setModNo(dbgint modno)
{
  m_modno = modno;
}

dbgint DBGRequestTagSrcCtxInfo::modno() const
{
  return m_modno;
}

char* DBGRequestTagSrcCtxInfo::toArray()
{
  m_raw = new char[tagSize()];

  m_raw = toNetwork(m_modno, m_raw , 0);
  return m_raw;
}


/****************************
 * DBGRequestTagProf
 **************************/

DBGRequestTagProf::DBGRequestTagProf()
    : DBGBaseTag(FRAME_PROF), DBGRequestTag(FRAME_PROF), m_modno(0)
{}

DBGRequestTagProf::DBGRequestTagProf(dbgint modno)
    : DBGBaseTag(FRAME_PROF), DBGRequestTag(FRAME_PROF), m_modno(modno)
{}

DBGRequestTagProf::~DBGRequestTagProf()
{}

void  DBGRequestTagProf::setModNo(dbgint modno)
{
  m_modno = modno;
}

dbgint DBGRequestTagProf::modno()
{
  return m_modno;
}

char* DBGRequestTagProf::toArray()
{
  m_raw = new char[tagSize()];

  m_raw = toNetwork(m_modno, m_raw , 0);
  return m_raw;
}

/****************************
 * DBGRequestTagProfC
 **************************/
 
DBGRequestTagProfC::DBGRequestTagProfC()
  : DBGBaseTag(FRAME_PROF_C), DBGRequestTag(FRAME_PROF_C), m_testLoops(0)
{}

DBGRequestTagProfC::DBGRequestTagProfC(dbgint testLoops)
  : DBGBaseTag(FRAME_PROF_C), DBGRequestTag(FRAME_PROF_C), m_testLoops(testLoops)
{}


DBGRequestTagProfC::~DBGRequestTagProfC()
{
}

void DBGRequestTagProfC::setTestLoops(dbgint testLoops)
{
  m_testLoops = testLoops;
}

dbgint DBGRequestTagProfC::testLoops()
{
  return m_testLoops;
}

char* DBGRequestTagProfC::toArray()
{
  m_raw = new char[tagSize()];

  m_raw = toNetwork(m_testLoops, m_raw , 0);
  return m_raw;
}
