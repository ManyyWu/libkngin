#ifndef _LOGFILE_H_
#define _LOGFILE_H_

#include <cstdio>
#include "log.h"
#include "logmgr.h"
#include "time.h"

#define __make_log(__level, __level_str, __file_type, __fmt, ...)                         \
        do {                                                                            \
            time_t _t = time(NULL);                                                     \
            tm _tm;                                                                     \
            __localtime(&_tm, &_t);                                                     \
            logger()[__file_type].__level(__log_format(__level_str, __fmt),               \
                                         _tm.tm_year + 1900, _tm.tm_mon, _tm.tm_mday,   \
                                         _tm.tm_hour, _tm.tm_min, _tm.tm_sec, 0,        \
                                         __FUNCTION__, __FILE__, __LINE__,              \
                                         ##__VA_ARGS__                                  \
                                         );                                             \
        } while (false)


#define server_fatal(__fmt, ...)   __make_log(fatal,   "FATAL  ", k::__LOG_FILE_SERVER, __fmt, ##__VA_ARGS__)
#define server_error(__fmt, ...)   __make_log(error,   "ERROR  ", k::__LOG_FILE_SERVER, __fmt, ##__VA_ARGS__)
#define server_warning(__fmt, ...) __make_log(warning, "WARNING", k::__LOG_FILE_SERVER, __fmt, ##__VA_ARGS__)
#define server_info(__fmt, ...)    __make_log(info,    "INFO   ", k::__LOG_FILE_SERVER, __fmt, ##__VA_ARGS__)
#define server_debug(__fmt, ...)   __make_log(debug,   "DEBUG  ", k::__LOG_FILE_SERVER, __fmt, ##__VA_ARGS__)
#define http_fatal(__fmt, ...)     __make_log(fatal,   "FATAL  ", k::__LOG_FILE_HTTP, __fmt, ##__VA_ARGS__)
#define http_error(__fmt, ...)     __make_log(error,   "ERROR  ", k::__LOG_FILE_HTTP, __fmt, ##__VA_ARGS__)
#define http_warning(__fmt, ...)   __make_log(warning, "WARNING", k::__LOG_FILE_HTTP, __fmt, ##__VA_ARGS__)
#define http_info(__fmt, ...)      __make_log(info,    "INFO   ", k::__LOG_FILE_HTTP, __fmt, ##__VA_ARGS__)
#define http_debug(__fmt, ...)     __make_log(debug,   "DEBUG  ", k::__LOG_FILE_HTTP, __fmt, ##__VA_ARGS__)

__NAMESPACE_BEGIN

__NAMESPACE_END

#endif /* _LOGFILE_H_ */
