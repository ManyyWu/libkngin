#ifndef _LOGFILE_H_
#define _LOGFILE_H_

#include "define.h"
#include "log.h"
#include "logmgr.h"

#if !defined(NDEBUG) && (ON == KNGIN_LOG_RELATIVE_PATH)
#define kngin_make_log(level, level_str, file_type, fmt, ...)   \
        do {                                                    \
            if (logger().inited())                              \
                logger()[file_type]                             \
                    .level(KNGIN_LOG_LOG_FORMAT(level_str, fmt),\
                             FUNCTION, KNGIN_FILENAME, LINE,    \
                             ##VA_ARGS);                        \
        } while (false)

#define kngin_make_log_noline(level, level_str, file_type, fmt, ...)   \
        do {                                                           \
            if (logger().inited())                                     \
                logger()[file_type]                                    \
                    .level(KNGIN_LOG_LOG_FORMAT_NOLINE(level_str, fmt),\
                             ##VA_ARGS);                               \
        } while (false)
#else
#define kngin_make_log(level, level_str, file_type, fmt, ...)   \
        do {                                                    \
            if (logger().inited())                              \
                logger()[file_type]                             \
                    .level(KNGIN_LOG_LOG_FORMAT(level_str, fmt),\
                             __FUNCTION__, __FILE__, __LINE__,  \
                             ##__VA_ARGS__);                    \
        } while (false)

#define kngin_make_log_noline(level, level_str, file_type, fmt, ...)   \
        do {                                                           \
            if (logger().inited())                                     \
                logger()[file_type]                                    \
                    .level(KNGIN_LOG_LOG_FORMAT_NOLINE(level_str, fmt),\
                             ##__VA_ARGS__);                           \
        } while (false)
#endif

// server log
#define server_fatal(fmt, ...)  \
    kngin_make_log(       fatal,   "FATAL  ", k::KNGIN_LOG_FILE_SERVER, fmt, ##__VA_ARGS__)
#define server_error(fmt, ...)  \
    kngin_make_log(       error,   "ERROR  ", k::KNGIN_LOG_FILE_SERVER, fmt, ##__VA_ARGS__)
#define server_warning(fmt, ...)\
    kngin_make_log_noline(warning, "WARNING", k::KNGIN_LOG_FILE_SERVER, fmt, ##__VA_ARGS__)
#define server_info(fmt, ...)   \
    kngin_make_log_noline(info,    "INFO   ", k::KNGIN_LOG_FILE_SERVER, fmt, ##__VA_ARGS__)
#define server_debug(fmt, ...)  \
    kngin_make_log(       debug,   "DEBUG  ", k::KNGIN_LOG_FILE_SERVER, fmt, ##__VA_ARGS__)

// default log
#if !defined(NDEBUG) && (ON == KNGIN_LOG_RELATIVE_PATH)
#define assert_log(exp)                                 \
    logger().inited()                                   \
        ? logger()[k::KNGIN_LOG_FILE_SERVER].log_assert(\
            FUNCTION, KNGIN_FILENAME, LINE, #exp)       \
        : 0
#else
#define assert_log(exp)                                 \
    logger().inited()                                   \
        ? logger()[k::KNGIN_LOG_FILE_SERVER].log_assert(\
            __FUNCTION__, __FILE__, __LINE__, #exp)     \
        : 0
#endif
#define log_dump(str)                                       \
    logger().inited()                                       \
        ? logger()[k::KNGIN_LOG_FILE_SERVER].log_data((str))\
        : 0

#define log_fatal   server_fatal
#define log_error   server_error
#define log_warning server_warning
#define log_info    server_info
#ifndef NDEBUG
#define log_debug   server_debug
#else
#define log_debug(fmt, ...) (void)(0)
#endif

KNGIN_NAMESPACE_K_BEGIN
KNGIN_NAMESPACE_K_END

#endif /* _LOGFILE_H_ */
