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


#ifndef DBGTAGS_H
#define DBGTAGS_H

#include "dbgabstractdatarequest.h"


/** Ancestral tag */

class DBGBaseTag {
public:
  DBGBaseTag();
  DBGBaseTag(int frameName);
  virtual ~DBGBaseTag();

  virtual void setName(int frameName);
  virtual int  name();
protected:
  int m_frameName;
};

/** Base for request tags */

class DBGRequestTag : public DBGBaseTag, public DBGAbstractDataRequest
{
public:
  DBGRequestTag(int tagName);
  virtual ~DBGRequestTag();

  virtual void setTagSize(long tagSize);
  virtual long tagSize();

protected:
  int m_tagSize;
};


/** response/request rawdata tag */
class DBGTagRawdata : public DBGRequestTag
{
public:
  DBGTagRawdata();
  DBGTagRawdata(int id, const char* data, long size);
  DBGTagRawdata(char* responseBuffer);
  virtual ~DBGTagRawdata();

  void setId(int id);
  void setData(char* data, long size);

  int id();
  long size();
  char* data();

  virtual long tagSize();
  virtual char* toArray();

private:
  int   m_id;
  long   m_size;
  char* m_data;

};

/** response tag for session id information */
class DBGResponseTagSid : public DBGBaseTag
{
public:
  DBGResponseTagSid();
  DBGResponseTagSid(char* responseBuffer);
  ~DBGResponseTagSid();

  void setIsid(int isid);
  void setSesstype(int sesstype);

  int isid();
  int sesstype();
private:
  int m_isid;
  int m_sesstype;
};

/** response tag for stack information */
class DBGResponseTagStack : public DBGBaseTag
{
public:
  DBGResponseTagStack();
  DBGResponseTagStack(char* responseBuffer);
  ~DBGResponseTagStack();

  void setLineNo(int line_no);
  void setModNo(int mod_no);
  void setScopeId(int scope_id);
  void setIdescr(int idescr);

  int lineNo();
  int modNo();
  int scopeId();
  int idescr();
private:
  int m_line_no;
  int m_mod_no;
  int m_scope_id;
  int m_idescr;
};

/** response tag for dbg version information */
class DBGResponseTagVersion : public DBGBaseTag
{
public:
  DBGResponseTagVersion();
  DBGResponseTagVersion(char* responseBuffer);
  ~DBGResponseTagVersion();

  void setMajorVersion(int major_version);
  void setMinorVersion(int minor_version);
  void setIdescription(int idescription);

  int majorVersion();
  int minorVersion();
  int idescription();
private:
  int m_major_version;
  int m_minor_version;
  int m_idescription;
};

/** response tag for information about the scripts tree */
class DBGResponseTagSrcLinesInfo : public DBGBaseTag
{
public:
  DBGResponseTagSrcLinesInfo();
  DBGResponseTagSrcLinesInfo(char* responseBuffer);
  ~DBGResponseTagSrcLinesInfo();

  void setParentModNo(int parentmodno);
  void setParentLineNo(int parentLineNo);
  void setModNo(int modno);
  void setImodName(int imodname);

  int parentModNo();
  int parentLineNo();
  int modNo();
  int imodName();
private:
  int m_parent_mod_no;
  int m_parent_line_no;
  int m_mod_no;
  int m_imod_name;
};

/** response tag for evaluation information */
class DBGResponseTagEval : public DBGBaseTag
{
public:
    DBGResponseTagEval();
    DBGResponseTagEval(char* responseBuffer);
    ~DBGResponseTagEval();

    void setIstr(int istr);
    void setIresult(int iresult);
    void setIerror(int ierror);

    int istr();
    int iresult();
    int ierror();
private:
  int m_istr;
  int m_iresult;
  int m_ierror;
};

/** response tag for notice/warnings */
class DBGResponseTagLog : public DBGBaseTag
{
public:
  DBGResponseTagLog();
  DBGResponseTagLog(char* responseBuffer);
  ~DBGResponseTagLog();

  void setIlog(int);
  void setType(int);
  void setModNo(int);
  void setLineNo(int);
  void setImodName(int);
  void setExtInfo(int);

  int ilog();
  int type();
  int modNo();
  int lineNo();
  int imodName();
  int extInfo();

private:
  int m_ilog;
  int m_type;
  int m_modNo;
  int m_lineNo;
  int m_imodName;
  int m_extInfo;
};

class DBGResponseTagError : public DBGBaseTag
{
public:
  DBGResponseTagError();
  DBGResponseTagError(char* responseBuffer);
  ~DBGResponseTagError();

  void setType(int);
  void setImessage(int);

  int type();
  int imessage();
private:
  int m_type;
  int m_imessage;
};

/** request tag for evaluation information */
class DBGRequestTagEval : public DBGRequestTag
{
public:
  DBGRequestTagEval();
  DBGRequestTagEval(int istr, int scopeId);
  ~DBGRequestTagEval();

  void setIstr(int istr);
  void setScopeId(int scopeId);

  int istr();
  int scopeId();

  virtual char* toArray();
private:
  int m_istr;
  int m_scopeId;
};

/** request/response tag to set breakpoint and receive breakpoint information */
class DBGTagBreakpoint : public DBGRequestTag
{
public:
  DBGTagBreakpoint();
  DBGTagBreakpoint(char* responseBuffer);
  DBGTagBreakpoint(int modno, int lineno, int imodame, int state, int istemp, int hitcount,
                  int skiphits, int icondition, int bp_no, int isunderhit);
  ~DBGTagBreakpoint();

  void setModNo(int modno);
  void setLineNo(int lineNo);
  void setImodName(int imodname);
  void setState(int state);
  void setIsTemp(int istemp);
  void setHitCount(int hitCount);
  void setSkipHits(int skipHits);
  void setIcondition(int icondition);
  void setBpNo(int bpno);
  void setIsUnderHit(int isunderhit);

  int modNo();
  int lineNo();
  int state();
  int imodname();
  int isTemp();
  int hitCount();
  int skipHits();
  int icondition();
  int bpNo();
  int isUnderHit();

  virtual char* toArray();
private:
  int m_mod_no;
  int m_line_no;
  int m_imod_name;
  int m_state;
  int m_istemp;
  int m_hitcount;
  int m_skiphits;
  int m_icondition;
  int m_bp_no;
  int m_isunderhit;
};

/** request tag for breakpoint information */
class DBGRequestTagBpl : public DBGRequestTag
{
public:
  DBGRequestTagBpl();
  DBGRequestTagBpl(int bpNo);
  ~DBGRequestTagBpl();

  void setBpNo(int no);
  int bpNo();

  virtual char* toArray();
private:
  int m_bp_no;
};

class DBGRequestTagOptions : public DBGRequestTag
{
public:
  DBGRequestTagOptions();
  DBGRequestTagOptions(int);
  ~DBGRequestTagOptions();

  void setOp(int);
  int op();

  virtual char* toArray();
private:
  int m_opt_flags;
};

#endif
