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


/***************************
* from dbg_defs.h          *
****************************/
#ifndef DBG_DEFS
#define DBG_DEFS

typedef long dbgint;

const int NET_SIZE_T = sizeof(int);

const int DBG_API_MAJOR_VESION     = 0x02;
const int DBG_API_MINOR_VESION     = 0x11;
const int DBG_API_RELEASE_STAGE    = 0x3;   /* 0-dev, 1-beta, 2-prerelease, 3-release */
const int DBG_API_MINOR_SUB_VESION = 0x30;

const int DBG_DEFAULT_PORT = 7869; //default client listen port

/*  tagSESSTYPE */
const int DBG_COMPAT  = 0x0001;
const int DBG_JIT     = 0x0002;
const int DBG_REQ     = 0x0003;
const int DBG_EMB     = 0x0004;

/* debugger_flags */
/* state: */
const int DBGF_STARTED        = 0x0001;    /* debugger has been started */
const int DBGF_FINISHED       = 0x0002;    /* DBGC_END notification has been sent */
const int DBGF_WAITACK        = 0x0004;    /* awaiting replay|request */
const int DBGF_UNSYNC         = 0x0008;    /* protocol has been unsynchronized */
const int DBGF_REQUESTPENDING = 0x0010;    /* Debug session request pending */
const int DBGF_REQUESTFOUND   = 0x0020;    /* Debug session request found */
const int DBGF_REJECTIONFOUND = 0x0040;      /* DBGSESSID=-1 found - session rejection */
/* execution:  */
const int DBGF_STEPINTO       = 0x0100;    /* break on next script instruction */
const int DBGF_STEPOVER       = 0x0200;    /* break on next script instruction on the same context */
const int DBGF_STEPOUT        = 0x0400;    /* break on next script instruction on the outer context */
const int DBGF_ABORT          = 0x0800;    /* stop instruction passed */

const int DBGSYNC =  0x5953;


/*
  PHP ENGINE to DEBUGGER CLIENT WAY:
*/

/* php-engine commands/events */
const int DBGC_REPLY          = 0x0000;    /* reply to previous DBGA_REQUEST request */
const int DBGC_STARTUP        = 0x0001;    /* script startup */
const int DBGC_END            = 0x0002;    /* script done */
const int DBGC_BREAKPOINT     = 0x0003;    /* user definded breakpoint occured */
const int DBGC_STEPINTO_DONE  = 0x0004;    /* step to the next statement is completed */
const int DBGC_STEPOVER_DONE  = 0x0005;    /* step to the next statement is completed */
const int DBGC_STEPOUT_DONE   = 0x0006;    /* step to the next statement is completed */
const int DBGC_EMBEDDED_BREAK = 0x0007;    /* breakpoint caused by DebugBreak() function */
const int DBGC_ERROR          = 0x0010;    /* error occured */
const int DBGC_LOG            = 0x0011;    /* logging support */
const int DBGC_SID            = 0x0012;    /* send SID */
const int DBGC_PAUSE          = 0x0013;    /* pause current session as soon as possible */


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

const int CURLOC_SCOPE_ID =  1;   /* nested locations are 2,3... and so on in backward order, so 2 represents most out-standing stack context*/
const int GLOBAL_SCOPE_ID = -1;    /* it is global context, not stacked */

/*
  DEBUGGER CLIENT to DBG SERVER WAY:
*/

/* debugger client makes acknowledgement */

const int DBGA_CONTINUE  =  0x8001;    /* php should continue run */
const int DBGA_STOP      =  0x8002;
const int DBGA_STEPINTO  =  0x8003;
const int DBGA_STEPOVER  =  0x8004;
const int DBGA_STEPOUT   =  0x8005;
const int DBGA_IGNORE    =  0x8006;
const int DBGA_REQUEST   =  0x8010;  /* debugger client requests some information from PHP engine */


/* TODO: from dbg_base_intf.h */

/* breakpoint states */
const int BPS_DELETED  = 0;
const int BPS_DISABLED = 1;
const int BPS_ENABLED  = 2;
const int BPS_UNRESOLVED = 0x0100;

/* Session flags */
const long  SOF_BREAKONLOAD         = 0x0001;
const long  SOF_BREAKONFINISH       = 0x0002;
const long  SOF_MATCHFILESINLOWCASE = 0x0004;

const long  SOF_SEND_LOGS             = 0x0010;
const long  SOF_SEND_ERRORS           = 0x0020;
const long  SOF_SEND_OUTPUT           = 0x0040;
const long  SOF_SEND_OUTPUT_DETAILED  = 0x00080;

/* LT_type */
const long  LT_ODS      = 1;    /* OutputDebugString */
const long  LT_ERROR    = 2;    /* Error/Warning/Notice while executing */
const long  LT_OUTPUT   = 3;    /* Any echo(), print() or header() results */
const long  LT_FATALERROR   = 256;    /* Fatal error (currently if error occured while evaluating)*/


#if 0

typedef enum tagBP_ACTION{
    BP_ABORT = 0,   // Abort the application
    BP_CONTINUE = 1,  // Continue running
    BP_STEP_INTO = 2, // Step into a procedure
    BP_STEP_OVER = 3, // Step over a procedure
    BP_STEP_OUT = 4,    // Step out of the current procedure
    BP_DELAY_HANDLING = 5,  // IDE didn't accept HandleBreakpoint request
} BP_ACTION;


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

#endif


/******************************************
 * internal definitions (for request tags)*
 ******************************************/

const int TAG_RAWDATA_SIZE            =  8;
const int TAG_REQ_SOURCE_SIZE         =  8;
const int TAG_REQ_BPL_SIZE            =  4;
const int TAG_REQ_EVAL_SIZE           =  8;
const int TAG_BPS_SIZE                =  40;
const int TAG_REQ_SRCLINESINFO_SIZE   =  4;
const int TAG_REQ_SRCCTXINFO_SIZE     =  4;
const int TAG_REQ_PROF_SIZE           =  4;
const int TAG_REQ_PROF_C_SIZE         =  4;
const int TAG_REQ_SET_OPT_SIZE        =  8; //for some reason, "4" here and DBG doesn't ack

/* custom def, complements CURLOC_SCOPE_ID and GLOBAL_SCOPE_ID. Used on DBGNet */
const int WATCH_SCOPE_ID = -2;

const int FRAME_SIZE = 8;
#endif
