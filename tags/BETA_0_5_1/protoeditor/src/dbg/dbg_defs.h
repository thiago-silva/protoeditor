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



const int DBGSYNC =  0x5953;

const int FRAME_STACK          =  100000;    /* "call:stack" - e.g. backtrace */
const int FRAME_SOURCE         =  100100;    /* source text */
const int FRAME_SRC_TREE       =  100200;    /* tree of source files */
const int FRAME_RAWDATA        =  100300;    /* raw data or string */
const int FRAME_ERROR          =  100400;    /* error notification */
const int FRAME_EVAL           =  100500;    /* evaluating/watching */
const int FRAME_BPS            =  100600;    /* set/remove breakpoint */
const int FRAME_BPL            =  100700;    /* breakpoint(s) request = get the list */
const int FRAME_VER            =  100800;    /* version request */
const int FRAME_SID            =  100900;    /* session id info*/
const int FRAME_SRCLINESINFO   =  101000;    /* source lines info */
const int FRAME_SRCCTXINFO     =  101100;    /* source contexts info */
const int FRAME_LOG            =  101200;    /* logging */
const int FRAME_PROF           =  101300;    /* profiler */
const int FRAME_PROF_C         =  101400;    /* profiler counter/accuracy */
const int FRAME_SET_OPT        =  101500;    /* set/update options */


const int TAG_STACK_SIZE          =  16;
const int TAG_RAWDATA_SIZE        =  8;
const int TAG_RES_EVAL_SIZE       =  12;
const int TAG_REQ_EVAL_SIZE       =  8;
const int TAG_BPS_SIZE            =  40;
const int TAG_RES_BPL_SIZE        =  40;
const int TAG_REQ_BPL_SIZE        =  4;
const int TAG_VER_SIZE            =  12;
const int TAG_SID_SIZE            =  8;
const int FRAME_SET_OPT_SIZE      =  8;


/* debugger client makes acknowledgement */

const int DBGA_CONTINUE  =  0x8001;    /* php should continue run */
const int DBGA_STOP      =  0x8002;
const int DBGA_STEPINTO  =  0x8003;
const int DBGA_STEPOVER  =  0x8004;
const int DBGA_STEPOUT   =  0x8005;
const int DBGA_IGNORE    =  0x8006;
const int DBGA_REQUEST   =  0x8010;  /* debugger client requests some information from PHP engine */

/** breakpoint states */
const int BPS_DELETED  = 0;
const int BPS_DISABLED = 1;
const int BPS_ENABLED  = 2;
const int BPS_UNRESOLVED = 0x0100;

/** session types */
const int DBG_COMPAT  = 0x0001;
const int DBG_JIT     = 0x0002;
const int DBG_REQ     = 0x0003;
const int DBG_EMB     = 0x0004;


const int CURLOC_SCOPE_ID =  1;   /* nested locations are 2,3... and so on in backward order, so 2 represents most out-standing stack context*/
const int GLOBAL_SCOPE_ID = -1;    /* it is global context, not stacked */

/* Session flags */
const long  SOF_BREAKONLOAD         = 0x0001;
const long  SOF_BREAKONFINISH       = 0x0002;
const long  SOF_MATCHFILESINLOWCASE = 0x0004;

const long  SOF_SEND_LOGS   = 0x0010;
const long  SOF_SEND_ERRORS   = 0x0020;
const long  SOF_SEND_OUTPUT   = 0x0040;
const long  SOF_SEND_OUTPUT_DETAILED  = 0x00080;

/** TODO: from dbg_cli, file dbg_base_intf.h */
#if 0
typedef enum tagBR_REASON {
    BR_UNKNOWN    = 0x0000, //
    BR_STEPINTO     = 0x0001, // Caused by the stepping mode
    BR_STEPOVER     = 0x0002, // Caused by the stepping mode
    BR_STEPOUT      = 0x0003, // Caused by the stepping mode
    BR_BREAKPOINT   = 0x0004, // Caused by an explicit breakpoint
    BR_EMBEDDED     = 0x0005, // Caused by a scripted break e.g. DebugBreak()
    BR_DEBUGGER_REQ = 0x0006, // Caused by debugger IDE requested break e.g. "Pause"
    BR_START_SESSION= 0x1007, // Pseudo breakpoint
    BR_END_SESSION  = 0x1008  // Pseudo breakpoint
} BR_REASON;

const long  ERR_ERROR   = 0x01;
const long  ERR_WARNING   = 0x02;
const long  ERR_PARSE   = 0x04;
const long  ERR_NOTICE    = 0x08;
const long  ERR_CORE_ERROR    = 0x10;
const long  ERR_CORE_WARNING  = 0x20;


typedef enum tagBP_ACTION{
    BP_ABORT = 0,   // Abort the application
    BP_CONTINUE = 1,  // Continue running
    BP_STEP_INTO = 2, // Step into a procedure
    BP_STEP_OVER = 3, // Step over a procedure
    BP_STEP_OUT = 4,    // Step out of the current procedure
    BP_DELAY_HANDLING = 5,  // IDE didn't accept HandleBreakpoint request
} BP_ACTION;

typedef enum tagBPSTATE {
    BPS_DELETED  = 0,
    BPS_DISABLED = 1,
    BPS_ENABLED  = 2,
    BPS_UNRESOLVED = 0x0100
} BPSTATE;


/* Listener status */
typedef enum tagListenerStatus {
  LSTNRS_ok = 0,
  LSTNRS_notinitialized,
  LSTNRS_stopped,
  LSTNRS_initerror
} ListenerStatus;

/* Listener log */
typedef enum tagListenerLogKind {
  LSTNRL_msg = 0,
  LSTNRL_warn,
  LSTNRL_error,
  LSTNRL_debugtrace
} ListenerLogKind;


/* LT_type */
const long  LT_ODS      = 1;    /* OutputDebugString */
const long  LT_ERROR    = 2;    /* Error/Warning/Notice while executing */
const long  LT_OUTPUT   = 3;    /* Any echo(), print() or header() results */
const long  LT_FATALERROR   = 256;    /* Fatal error (currently if error occured while evaluating)*/

typedef enum tagSERVER_TYPE {
  SVR_AUTO  = 0x00,
  SVR_CGI   = 0x01,
  SVR_HTTP  = 0x02,
  SVR_HTTPS = 0x03,
} SERVER_TYPE;

typedef enum tagDBGS_RESULT {
  DBGSE_SUCCESSFULL     = 0,
  DBGSE_FAILED        = 0x1000,
  DBGSE_WRONGREQ        = 0x1001,
  DBGSE_FAILEDCREATETHREAD  = 0x1002,
  DBGSE_FAILEDCREATESOCKET  = 0x1003,
  DBGSE_FAILEDBINDSOCKET    = 0x1004,
  DBGSE_FAILEDLISTENSOCKET  = 0x1005,
  DBGSE_FAILEDCONNECTSOCKET = 0x1006,
  DBGSE_FILENOTFOUND      = 0x1007,
  DBGSE_CGIHANDLERNOTFOUND  = 0x1008,
  DBGSE_HTTPHOSTNOTFOUND    = 0x1009,
  DBGSE_TIMEOUTSTARTSESS    = 0x100A,
  DBGSE_ASSIGNPIPE      = 0x100B,
} DBGS_RESULT;


typedef enum tagBP_ACTION{
    BP_ABORT = 0,   // Abort the application
    BP_CONTINUE = 1,  // Continue running
    BP_STEP_INTO = 2, // Step into a procedure
    BP_STEP_OVER = 3, // Step over a procedure
    BP_STEP_OUT = 4,    // Step out of the current procedure
    BP_DELAY_HANDLING = 5,  // IDE didn't accept HandleBreakpoint request
} BP_ACTION;

typedef enum tagBPSTATE {
    BPS_DELETED  = 0,
    BPS_DISABLED = 1,
    BPS_ENABLED  = 2,
    BPS_UNRESOLVED = 0x0100
} BPSTATE;
#endif

const int DBGF_STARTED    =0x0001;    /* debugger has been started */
const int DBGF_FINISHED   =0x0002;    /* DBGC_END notification has been sent */
const int DBGF_WAITACK    =0x0004;    /* awaiting replay|request */
const int DBGF_UNSYNC     =0x0008;    /* protocol has been unsynchronized */
const int DBGF_REQUESTPENDING =0x0010;    /* Debug session request pending */
const int DBGF_REQUESTFOUND =0x0020;    /* Debug session request found */
const int DBGF_REJECTIONFOUND =0x0040;      /* DBGSESSID=-1 found - session rejection */


const int DBG_DEFAULT_PORT = 7869; //default client listen port
