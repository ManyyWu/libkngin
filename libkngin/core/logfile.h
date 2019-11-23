#ifndef _LOGFILE_H_
#define _LOGFILE_H_

#include "define.h"
#include "log.h"
#include "logmgr.h"

#if !defined(NDEBUG) && (ON == KNGIN_LOG_RELATIVE_PATH)
#define kngin_make_log(__level, __level_str, __file_type, __fmt, ...) \
        do {                                                          \
            if (logger().inited())                                    \
                logger()[__file_type]                                 \
                    .__level(KNGIN_LOG_LOG_FORMAT(__level_str, __fmt),\
                             __FUNCTION__, __FILENAME__, __LINE__,    \
                             ##__VA_ARGS__);                          \
        } while (false)

#define kngin_make_log_noline(__level, __level_str, __file_type, __fmt, ...) \
        do {                                                                 \
            if (logger().inited())                                           \
                logger()[__file_type]                                        \
                    .__level(KNGIN_LOG_LOG_FORMAT_NOLINE(__level_str, __fmt),\
                             ##__VA_ARGS__);                                 \
        } while (false)
#else
#define kngin_make_log(__level, __level_str, __file_type, __fmt, ...) \
        do {                                                          \
            if (logger().inited())                                    \
                logger()[__file_type]                                 \
                    .__level(KNGIN_LOG_LOG_FORMAT(__level_str, __fmt),\
                             __FUNCTION__, __FILE__, __LINE__,        \
                             ##__VA_ARGS__);                          \
        } while (false)

#define kngin_make_log_noline(__level, __level_str, __file_type, __fmt, ...) \
        do {                                                                 \
            if (logger().inited())                                           \
                logger()[__file_type]                                        \
                    .__level(KNGIN_LOG_LOG_FORMAT_NOLINE(__level_str, __fmt),\
                             ##__VA_ARGS__);                                 \
        } while (false)
#endif

// server log
#define server_fatal(__fmt, ...)  \
    kngin_make_log(       fatal,   "FATAL  ", k::KNGIN_LOG_FILE_SERVER, __fmt, ##__VA_ARGS__)
#define server_error(__fmt, ...)  \
    kngin_make_log(       error,   "ERROR  ", k::KNGIN_LOG_FILE_SERVER, __fmt, ##__VA_ARGS__)
#define server_warning(__fmt, ...)\
    kngin_make_log_noline(warning, "WARNING", k::KNGIN_LOG_FILE_SERVER, __fmt, ##__VA_ARGS__)
#define server_info(__fmt, ...)   \
    kngin_make_log_noline(info,    "INFO   ", k::KNGIN_LOG_FILE_SERVER, __fmt, ##__VA_ARGS__)
#define server_debug(__fmt, ...)  \
    kngin_make_log(       debug,   "DEBUG  ", k::KNGIN_LOG_FILE_SERVER, __fmt, ##__VA_ARGS__)

// default log
#if !defined(NDEBUG) && (ON == KNGIN_LOG_RELATIVE_PATH)
#define assert_log(__exp)                                \
    logger().inited()                                    \
        ? logger()[k::KNGIN_LOG_FILE_SERVER].log_assert( \
            __FUNCTION__, __FILENAME__, __LINE__, #__exp)\
        : 0
#else
#define assert_log(__exp)                               \
    logger().inited()                                   \
        ? logger()[k::KNGIN_LOG_FILE_SERVER].log_assert(\
            __FUNCTION__, __FILE__, __LINE__, #__exp)   \
        : 0
#endif
#define log_dump(__str)                                       \
    logger().inited()                                         \
        ? logger()[k::KNGIN_LOG_FILE_SERVER].log_data((__str))\
        : 0

#define log_fatal   server_fatal
#define log_error   server_error
#define log_warning server_warning
#define log_info    server_info
#ifndef NDEBUG
#define log_debug   server_debug
#else
#define log_debug(__fmt, ...) (void)(0)
#endif

KNGIN_NAMESPACE_K_BEGIN
KNGIN_NAMESPACE_K_END

#endif /* _LOGFILE_H_ */
