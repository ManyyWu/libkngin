#ifndef _LOGFILE_H_
#define _LOGFILE_H_

#include <cstdio>
#include "log.h"
#include "logmgr.h"
#include "time.h"

#define __make_log(__level, __level_str, __file_type, __fmt, ...) \
        do {                                                      \
            if (logger().inited())                                \
                logger()[__file_type]                             \
                    .__level(__log_format(__level_str, __fmt),    \
                             __FUNCTION__, __FILE__, __LINE__,    \
                             ##__VA_ARGS__);                      \
        } while (false)

#define __make_log_noline(__level, __level_str, __file_type, __fmt, ...) \
        do {                                                             \
            if (logger().inited())                                       \
                logger()[__file_type]                                    \
                    .__level(__log_format_noline(__level_str, __fmt),    \
                                                 ##__VA_ARGS__);         \
        } while (false)

#define server_fatal(__fmt, ...)   __make_log(       fatal,   "FATAL  ", k::__LOG_FILE_SERVER, __fmt, ##__VA_ARGS__)
#define server_error(__fmt, ...)   __make_log(       error,   "ERROR  ", k::__LOG_FILE_SERVER, __fmt, ##__VA_ARGS__)
#define server_warning(__fmt, ...) __make_log_noline(warning, "WARNING", k::__LOG_FILE_SERVER, __fmt, ##__VA_ARGS__)
#define server_info(__fmt, ...)    __make_log_noline(info,    "INFO   ", k::__LOG_FILE_SERVER, __fmt, ##__VA_ARGS__)
#define server_debug(__fmt, ...)   __make_log(       debug,   "DEBUG  ", k::__LOG_FILE_SERVER, __fmt, ##__VA_ARGS__)

// default log
#define assert_log(__exp)       logger().inited() ? logger()[k::__LOG_FILE_SERVER].log_assert(__FUNCTION__, __FILE__, __LINE__, #__exp) : 0
#define log_dump(__data, __len) logger().inited() ? logger()[k::__LOG_FILE_SERVER].log_data((__data), (__len)) : 0
#define log_fatal   server_fatal
#define log_error   server_error
#define log_warning server_warning
#define log_info    server_info
#define log_debug   server_debug


__NAMESPACE_BEGIN

class logfile {
public:
    logfile ();
};

__NAMESPACE_END

#endif /* _LOGFILE_H_ */
