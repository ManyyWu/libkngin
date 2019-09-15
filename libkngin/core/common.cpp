#include <cstdarg>
#include <cstring>
#include "common.h"
#include "logfile.h"
#include "log.h"
#include "logmgr.h"

__NAMESPACE_BEGIN

//#ifndef NDEBUG void __log_assert (const char *_fmt, ...) { va_list _vl; char _buf[__LOG_BUF_SIZE + 1] = {0}; va_start(_vl, _fmt); vsnprintf(_buf, __LOG_BUF_SIZE, _fmt, _vl);
//    va_end(_vl);
//    size_t _len = strnlen(_buf, __LOG_BUF_SIZE);
//    logger()[__LOG_FILE_SERVER].write_logfile(LOG_LEVEL_FATAL, "kngin_server", _buf, _len);
//    logger()[__LOG_FILE_SERVER].write_stderr(LOG_LEVEL_MAX, _buf, _len);
//}
//
//void
//__log_assert_dump_stack (const char *_fmt, ...)
//{
//    va_list _vl;
//
//    char _buf[__LOG_BUF_SIZE + 1] = {0};
//    va_start(_vl, _fmt);
//    vsnprintf(_buf, __LOG_BUF_SIZE, _fmt, _vl);
//    va_end(_vl);
//    size_t _len = strnlen(_buf, __LOG_BUF_SIZE);
//    logger()[__LOG_FILE_SERVER].write_logfile(LOG_LEVEL_FATAL, "kngin_server", _buf, _len);
//    logger()[__LOG_FILE_SERVER].write_stderr(LOG_LEVEL_MAX, _buf, _len);
//}
//
//#endif /* NDEBUG */

__NAMESPACE_END
