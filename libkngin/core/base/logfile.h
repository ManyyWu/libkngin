#ifndef KNGIN_LOGFILE_H
#define KNGIN_LOGFILE_H

#include "core/base/define.h"
#include "core/base/log.h"
#include "core/base/logmgr.h"

// for log
#if !defined(NDEBUG) and (ON == KNGIN_LOG_RELATIVE_PATH)
#define kngin_make_log(level, level_str, file_type, fmt, ...) \
    do {                                                      \
        if (k::logger().inited())                             \
            k::logger()[file_type]                            \
                .level(KNGIN_LOG_FORMAT(level_str, fmt),      \
                       __FUNCTION__, KNGIN_FILENAME, __LINE__,\
                       ##__VA_ARGS__);                        \
    } while (false)

#define kngin_make_log_noline(level, level_str, file_type, fmt, ...)\
    do {                                                            \
        if (k::logger().inited())                                   \
            k::logger()[file_type]                                  \
                .level(KNGIN_LOG_FORMAT_NOLINE(level_str, fmt),     \
                       ##__VA_ARGS__);                              \
    } while (false)
#else
#define kngin_make_log(level, level_str, file_type, fmt, ...)\
    do {                                                     \
        if (k::logger().inited())                            \
            k::logger()[file_type]                           \
                .level(KNGIN_LOG_FORMAT(level_str, fmt),     \
                         __FUNCTION__, __FILE__, __LINE__,   \
                         ##__VA_ARGS__);                     \
    } while (false)

#define kngin_make_log_noline(level, level_str, file_type, fmt, ...)\
    do {                                                            \
        if (k::logger().inited())                                   \
            k::logger()[file_type]                                  \
                .level(KNGIN_LOG_FORMAT_NOLINE(level_str, fmt),     \
                         ##__VA_ARGS__);                            \
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
#if defined(NDEBUG) and (ON == KNGIN_LOG_RELATIVE_PATH)
#define assert_log(exp)                                     \
    k::logger().inited()                                    \
        ? k::logger()[k::KNGIN_LOG_FILE_SERVER].log_assert( \
             __FUNCTION__, KNGIN_FILENAME, __LINE__, #exp)  \
        : 0

#else
#define assert_log(exp)                                    \
    k::logger().inited()                                   \
        ? k::logger()[k::KNGIN_LOG_FILE_SERVER].log_assert(\
             __FUNCTION__, __FILE__, __LINE__, #exp)       \
        : 0
#endif
#define log_dump(str)                                             \
    do {                                                          \
        if (k::logger().inited())                                 \
            k::logger()[k::KNGIN_LOG_FILE_SERVER].log_data((str));\
    } while (false)
#define log_fatal   server_fatal
#define log_error   server_error
#define log_warning server_warning
#define log_info    server_info
#ifndef NDEBUG
#define log_debug   server_debug
#else
#define log_debug(fmt, ...) static_cast<void>(0)
#endif

KNGIN_NAMESPACE_K_BEGIN
KNGIN_NAMESPACE_K_END

#endif /* KNGIN_LOGFILE_H */
