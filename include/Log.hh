#ifndef   PRX_LOG_INCLUDED
#define   PRX_LOG_INCLUDED

#include <stdarg.h>
#include <stdio.h>

#include "Prx.hh"

#define PRX_FMT_BUFFER_SIZE 128

#define PRX_DBG(msg) prx_log("dbg", msg)
#define PRX_INF(msg) prx_log("inf", msg)
#define PRX_WRN(msg) prx_log("wrn", msg)
#define PRX_ERR(msg) prx_log("err", msg)

#define PRX_DBGx(fmt, ...) prx_log("dbg", fmt, __VA_ARGS__)
#define PRX_INFx(fmt, ...) prx_log("inf", fmt, __VA_ARGS__)
#define PRX_WRNx(fmt, ...) prx_log("wrn", fmt, __VA_ARGS__)
#define PRX_ERRx(fmt, ...) prx_log("err", fmt, __VA_ARGS__)

int prx_log(const char* lvl, const char* fmt, ...);

#endif  //   PRX_LOG_INCLUDED
