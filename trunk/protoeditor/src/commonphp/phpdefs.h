/*
  convenience copy from Zend/zend_error.h
 */

#ifndef PHPDEFS_H
#define PHPDEFS_H

enum {
  E_ERROR           = (1<<0L),
  E_WARNING         = (1<<1L),
  E_PARSE           = (1<<2L),
  E_NOTICE          = (1<<3L),
  E_CORE_ERROR      = (1<<4L),
  E_CORE_WARNING    = (1<<5L),
  E_COMPILE_ERROR   = (1<<6L),
  E_COMPILE_WARNING = (1<<7L),
  E_USER_ERROR      = (1<<8L),
  E_USER_WARNING    = (1<<9L),
  E_USER_NOTICE     = (1<<10L),
  E_STRICT          = (1<<11L)

};

#endif
