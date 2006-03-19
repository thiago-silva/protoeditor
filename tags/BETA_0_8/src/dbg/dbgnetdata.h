/***************************************************************************
 *   Copyright (C) 2004-2005 by Thiago Silva                                    *
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

#ifndef DBGNETDATA_H
#define DBGNETDATA_H
#include "dbg_defs.h"

class DBGNet;
class DBGResponsePack;

/* Base for incoming data */
class Receivable
{
public:
  Receivable();
  virtual ~Receivable();
protected:
  virtual dbgint fromNetwork(char* data, dbgint offset) const;
};

/* Base for outgoing data */

class Requestable
{
public:
  Requestable();
  virtual ~Requestable();

  virtual char* toArray() = 0;
protected:
  virtual char* toNetwork(dbgint num, char* buffer, dbgint offset)  const;
  virtual void  clearRawData();

  char* m_raw;
};

/**************************************************************/

/* header data */

class DBGHeader : public Receivable, public Requestable
{
public:
  DBGHeader();
  DBGHeader(char* responseBuffer);
  virtual ~DBGHeader();

  void setSync(dbgint sync);
  void setCmd(dbgint cmd);
  void setFlags(dbgint flags);
  void setBodysize(dbgint bodysize);

  dbgint sync() const;
  dbgint cmd() const;
  dbgint flags() const;
  dbgint bodysize() const;

  virtual char* toArray();

  static const dbgint SIZE;
private:
  dbgint m_sync;
  dbgint m_cmd;
  dbgint m_flags;
  dbgint m_bodysize;
};

/* Frame data */
class DBGFrame : public Receivable, public Requestable
{
public:
  DBGFrame();
  DBGFrame(dbgint name, dbgint datasize = 0);
  DBGFrame(char* responseBuffer);
  virtual ~DBGFrame();

  void setName(dbgint name);
  void setDatasize(dbgint datasize);

  dbgint name() const;
  dbgint datasize() const;

  virtual char* toArray();

  static const dbgint SIZE;
private:
  dbgint      m_name;
  dbgint      m_datasize;
};

/****************************** TAGS ************************************/

/* Ancestral tag */

class DBGBaseTag
{
public:
  //DBGBaseTag();
  DBGBaseTag(dbgint frameName);
  virtual ~DBGBaseTag();

  virtual void setName(dbgint frameName);
  virtual dbgint  name() const;

protected:
  dbgint m_frameName;
};

/* Base for request tags */
class DBGRequestTag : public virtual DBGBaseTag, public Requestable
{
public:
  DBGRequestTag(dbgint frameName);
  virtual ~DBGRequestTag();

  virtual void setTagSize(long tagSize);
  virtual long tagSize() const;
protected:
  dbgint   m_tagSize;
};

/* Base for response tags */
class DBGResponseTag : public virtual DBGBaseTag, public Receivable
{
public:
  DBGResponseTag(dbgint frameName);
  virtual ~DBGResponseTag();

  virtual void process(DBGNet*, DBGResponsePack*)  const = 0;
};

/************************************/


/* rawdata tag */
class DBGTagRawdata : public DBGRequestTag, public DBGResponseTag
{
public:
  DBGTagRawdata();
  DBGTagRawdata(dbgint id, const char* data, long size);
  DBGTagRawdata(char* responseBuffer);
  virtual ~DBGTagRawdata();

  void setId(dbgint id);
  void setData(char* data, long size);

  dbgint id() const;
  long size() const;
  char* data() const;

  virtual long  tagSize() const;
  virtual char* toArray();

  virtual void process(DBGNet*, DBGResponsePack*) const;
private:
  dbgint   m_id;
  long  m_size;
  char* m_data;

};


/* response tag for session id information */
class DBGResponseTagSid : public DBGResponseTag
{
public:
  DBGResponseTagSid();
  DBGResponseTagSid(char* responseBuffer);
  ~DBGResponseTagSid();

  void setIsid(dbgint isid);
  void setSesstype(dbgint sesstype);

  dbgint isid() const;
  dbgint sesstype() const;

  virtual void process(DBGNet*, DBGResponsePack*) const;
private:
  dbgint m_isid;
  dbgint m_sesstype;
};

/* response tag for stack information */
class DBGResponseTagStack : public DBGResponseTag
{
public:
  DBGResponseTagStack();
  DBGResponseTagStack(char* responseBuffer);
  ~DBGResponseTagStack();

  void setLineNo(dbgint line_no);
  void setModNo(dbgint mod_no);
  void setScopeId(dbgint scope_id);
  void setIdescr(dbgint idescr);

  dbgint lineNo() const;
  dbgint modNo() const;
  dbgint scopeId() const;
  dbgint idescr() const;

  virtual void process(DBGNet*, DBGResponsePack*) const;
private:
  dbgint m_line_no;
  dbgint m_mod_no;
  dbgint m_scope_id;
  dbgint m_idescr;
};

/* response tag for dbg version information */
class DBGResponseTagVersion : public DBGResponseTag
{
public:
  DBGResponseTagVersion();
  DBGResponseTagVersion(char* responseBuffer);
  ~DBGResponseTagVersion();

  void setMajorVersion(dbgint major_version);
  void setMinorVersion(dbgint minor_version);
  void setIdescription(dbgint idescription);

  dbgint majorVersion() const;
  dbgint minorVersion() const;
  dbgint idescription() const;

  virtual void process(DBGNet*, DBGResponsePack*) const;

private:
  dbgint m_major_version;
  dbgint m_minor_version;
  dbgint m_idescription;
};

/* response tag for information about the scripts tree */
class DBGResponseTagSrcTree : public DBGResponseTag
{
public:
  DBGResponseTagSrcTree();
  DBGResponseTagSrcTree(char* responseBuffer);
  ~DBGResponseTagSrcTree();

  void setParentModNo(dbgint parentmodno);
  void setParentLineNo(dbgint parentLineNo);
  void setModNo(dbgint modno);
  void setImodName(dbgint imodname);

  dbgint parentModNo() const;
  dbgint parentLineNo() const;
  dbgint modNo() const;
  dbgint imodName() const;

  virtual void process(DBGNet*, DBGResponsePack*) const;

private:
  dbgint m_parent_mod_no;
  dbgint m_parent_line_no;
  dbgint m_mod_no;
  dbgint m_imod_name;
};

/* response tag for srclinesinfo */
class DBGResponseTagSrcLinesInfo : public DBGResponseTag
{
public:
  DBGResponseTagSrcLinesInfo();
  DBGResponseTagSrcLinesInfo(char* responseBuffer);
  ~DBGResponseTagSrcLinesInfo();

  void setModNo(dbgint);
  void setStartLineNo(dbgint);
  void setLinesCount(dbgint);
  void setCtxId(dbgint);

  dbgint modNo() const;
  dbgint startLineNo() const;
  dbgint linesCount() const;
  dbgint ctxId() const;

  virtual void process(DBGNet*, DBGResponsePack*) const;
private:
  dbgint m_mod_no;
  dbgint m_start_line_no;
  dbgint m_lines_count;
  dbgint m_ctx_id;
};

/* response tag for context information */
class DBGResponseTagSrcCtxInfo : public DBGResponseTag
{
public:
  DBGResponseTagSrcCtxInfo();
  DBGResponseTagSrcCtxInfo(char* responseBuffer);
  ~DBGResponseTagSrcCtxInfo();

  void setModNo(dbgint);
  void setCtxId(dbgint);
  void setIFunctionName(dbgint);

  dbgint modno() const;
  dbgint ctxid() const;
  dbgint ifunctionName() const;

  virtual void process(DBGNet*, DBGResponsePack*) const;
private:
  dbgint m_modno;
  dbgint m_ctxid;
  dbgint m_ifunctionName;
};

/* response tag for variable information */
class DBGResponseTagEval : public DBGResponseTag
{
public:
  DBGResponseTagEval();
  DBGResponseTagEval(char* responseBuffer);
  ~DBGResponseTagEval();

  void setIstr(dbgint istr);
  void setIresult(dbgint iresult);
  void setIerror(dbgint ierror);

  dbgint istr() const;
  dbgint iresult() const;
  dbgint ierror() const;

  virtual void process(DBGNet*, DBGResponsePack*) const;

private:
  dbgint m_istr;
  dbgint m_iresult;
  dbgint m_ierror;
};

/* response tag for notice/warnings */
class DBGResponseTagLog : public DBGResponseTag
{
public:
  DBGResponseTagLog();
  DBGResponseTagLog(char* responseBuffer);
  ~DBGResponseTagLog();


  void setIlog(dbgint);
  void setType(dbgint);
  void setModNo(dbgint);
  void setLineNo(dbgint);
  void setImodName(dbgint);
  void setExtInfo(dbgint);

  dbgint ilog() const;
  dbgint type() const;
  dbgint modNo() const;
  dbgint lineNo() const;
  dbgint imodName() const;
  dbgint extInfo() const;

  virtual void process(DBGNet*, DBGResponsePack*) const;

private:
  dbgint m_ilog;
  dbgint m_type;
  dbgint m_modNo;
  dbgint m_lineNo;
  dbgint m_imodName;
  dbgint m_extInfo;
};

/* response tag for errors */
class DBGResponseTagError : public DBGResponseTag
{
public:
  DBGResponseTagError();
  DBGResponseTagError(char* responseBuffer);
  ~DBGResponseTagError();

  void setType(dbgint);
  void setImessage(dbgint);

  dbgint type() const;
  dbgint imessage() const;

  virtual void process(DBGNet*, DBGResponsePack*) const;

private:
  dbgint m_type;
  dbgint m_imessage;
};

/* response tag for profile data */
class DBGResponseTagProf : public DBGResponseTag
{
public:
  DBGResponseTagProf();
  DBGResponseTagProf(char* responseBuffer);
  ~DBGResponseTagProf();

  void setModNo(dbgint);
  void setLineNo(dbgint);
  void setHitCount(dbgint);
  void setMinLo(dbgint);
  void setMinHi(dbgint);
  void setMaxLo(dbgint);
  void setMaxHi(dbgint);
  void setSumLo(dbgint);
  void setSumHi(dbgint);

  dbgint modNo() const;
  dbgint lineNo() const;
  dbgint hitCount() const;
  dbgint minLo() const;
  dbgint minHi() const;
  dbgint maxLo() const;
  dbgint maxHi() const;
  dbgint sumLo() const;
  dbgint sumHi() const;

  virtual void process(DBGNet*, DBGResponsePack*) const;

private:
  dbgint m_modNo;
  dbgint m_lineNo;
  dbgint m_hitCount;
  dbgint m_minLo;
  dbgint m_minHi;
  dbgint m_maxLo;
  dbgint m_maxHi;
  dbgint m_sumLo;
  dbgint m_sumHi;
};

/* response tag for profile req */
class DBGResponseTagProfC : public DBGResponseTag
{
public:
  DBGResponseTagProfC();
  DBGResponseTagProfC(char* responseBuffer);
  ~DBGResponseTagProfC();

  void setFreqLo(dbgint);
  void setFreqHi(dbgint);
  void setDiffMin(dbgint);
  void setDiffMax(dbgint);
  void setDiffM(dbgint);

  dbgint freqlo() const;
  dbgint freqhi() const;
  dbgint diffmin() const;
  dbgint diffmax() const;
  dbgint diffm() const;

  virtual void process(DBGNet*, DBGResponsePack*) const;
private:
  dbgint m_freqlo;
  dbgint m_freqhi;
  dbgint m_diffmin;
  dbgint m_diffmax;
  dbgint m_diffm;
};


/**********
  REQUESTS
 **********/

/* request tag for evaluation information */
class DBGRequestTagEval : public DBGRequestTag
{
public:
  DBGRequestTagEval();
  DBGRequestTagEval(dbgint istr, dbgint scopeId);
  ~DBGRequestTagEval();

  void setIstr(dbgint istr);
  void setScopeId(dbgint scopeId);

  dbgint istr() const;
  dbgint scopeId() const;

  virtual char* toArray();
private:
  dbgint m_istr;
  dbgint m_scopeId;
};

/* request/response tag to set breakpodbgint and receive breakpodbgint information */
class DBGTagBreakpoint : public DBGRequestTag, public DBGResponseTag
{
public:
  DBGTagBreakpoint();
  DBGTagBreakpoint(char* responseBuffer);
  DBGTagBreakpoint(dbgint modno, dbgint lineno, dbgint imodame, dbgint state, dbgint istemp, dbgint hitcount,
                   dbgint skiphits, dbgint icondition, dbgint bp_no, dbgint isunderhit);
  ~DBGTagBreakpoint();

  void setModNo(dbgint modno);
  void setLineNo(dbgint lineNo);
  void setImodName(dbgint imodname);
  void setState(dbgint state);
  void setIsTemp(dbgint istemp);
  void setHitCount(dbgint hitCount);
  void setSkipHits(dbgint skipHits);
  void setIcondition(dbgint icondition);
  void setBpNo(dbgint bpno);
  void setIsUnderHit(dbgint isunderhit);

  dbgint modNo() const;
  dbgint lineNo() const;
  dbgint state() const;
  dbgint imodname() const;
  dbgint isTemp() const;
  dbgint hitCount() const;
  dbgint skipHits() const;
  dbgint icondition() const;
  dbgint bpNo() const;
  dbgint isUnderHit() const;

  virtual char* toArray();

  virtual void process(DBGNet*, DBGResponsePack*) const;

private:
  dbgint m_mod_no;
  dbgint m_line_no;
  dbgint m_imod_name;
  dbgint m_state;
  dbgint m_istemp;
  dbgint m_hitcount;
  dbgint m_skiphits;
  dbgint m_icondition;
  dbgint m_bp_no;
  dbgint m_isunderhit;
};

/* request tag for breakpodbgint information */
class DBGRequestTagBpl : public DBGRequestTag
{
public:
  DBGRequestTagBpl();
  DBGRequestTagBpl(dbgint bpNo);
  ~DBGRequestTagBpl();

  void setBpNo(dbgint no);
  dbgint bpNo() const;

  virtual char* toArray();
private:
  dbgint m_bp_no;
};

/* request tag for options information */
class DBGRequestTagOptions : public DBGRequestTag
{
public:
  DBGRequestTagOptions();
  DBGRequestTagOptions(dbgint);
  ~DBGRequestTagOptions();

  void setOp(dbgint);
  dbgint op() const;

  virtual char* toArray();
private:
  dbgint m_opt_flags;
};

/* request tag for srclines data */
class DBGRequestTagSrcLinesInfo : public DBGRequestTag
{
public:
  DBGRequestTagSrcLinesInfo();
  DBGRequestTagSrcLinesInfo(dbgint);
  ~DBGRequestTagSrcLinesInfo();

  void setModNo(dbgint);
  dbgint modno() const;

  virtual char* toArray();
private:
  dbgint m_modno;
};

/* request tag for context data */
class DBGRequestTagSrcCtxInfo : public DBGRequestTag
{
public:
  DBGRequestTagSrcCtxInfo();
  DBGRequestTagSrcCtxInfo(dbgint);
  ~DBGRequestTagSrcCtxInfo();

  void setModNo(dbgint);
  dbgint modno() const;

  virtual char* toArray();
private:
  dbgint m_modno;
};

/* request tag for profile data */
class DBGRequestTagProf : public DBGRequestTag
{
public:
  DBGRequestTagProf();
  DBGRequestTagProf(dbgint);
  ~DBGRequestTagProf();

  void  setModNo(dbgint);
  dbgint modno();

  virtual char* toArray();
private:
  dbgint m_modno;
};

/* request tag for profile freq data  */
class DBGRequestTagProfC : public DBGRequestTag
{
public:
  DBGRequestTagProfC();
  DBGRequestTagProfC(dbgint);
  ~DBGRequestTagProfC();

  void setTestLoops(dbgint);
  dbgint testLoops();

  virtual char* toArray();
private:
  dbgint m_testLoops;
};


#endif
