#ifndef _K_LOG_H_
#define _K_LOG_H_

#include <cstdarg>
#include <string>
#include "define.h"
#include "lock.h"

#define __LOG_BUF_SIZE      4096
#define __LOG_FILE_MAX_SIZE 20 * 1024 * 1024 // 20M

// "YYYY/MM/DD hh:mm:ss"
#define __log_datetime_format         "%04d/%02d/%02d %02d:%02d:%02d"
#define __LOG_DATETIME_LEN 20

// "YYYY/MM/DD hh:mm:ss.ms | type | func[file:line] | fmt \n"
#define __log_format(__t, __f)        " | " __t " | %s[%s:%d] | " __f

// "YYYY/MM/DD hh:mm:ss.ms | type | fmt \n"
#define __log_format_noline(__t, __f) " | " __t " | " __f

// "YYYY-MM-DD"
#define __log_filename_format         "%s_%04d-%02d-%02d.log"

// "****** func[file:line] ******"
#define __log_assert_format           " | ASSERT  | %s[%s:%d] | ****** %s *******"

// color
#ifdef _WIN32
#else
#define __COLOR_NONE    "\033[0m"
#define __COLOR_FATAL   "\033[01;32;41m"
#define __COLOR_ERROR   "\033[01;37;43m"
#define __COLOR_WARNING "\033[01;31;49m"
#define __COLOR_INFO    "\033[01;32;49m"
#define __COLOR_DEBUG   ""
#define __COLOR_ASSERT  "\033[05;37;41m"
#endif

__NAMESPACE_BEGIN

/*
 * Type of log file
 */
enum __LOG_FILE {          // reserved type
    __LOG_FILE_SERVER = 0,     // default
};

enum __LOG_MODE {
    __LOG_MODE_FILE = 0,   // only output to file
    __LOG_MODE_STDERR,     // only output to stderr
    __LOG_MODE_BOTH        // the above two
};

enum LOG_LEVEL {
    LOG_LEVEL_FATAL = 0,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
};

class log_mgr;
class log {
public:
    log           () = delete;

private:
    explicit
    log           (__LOG_FILE _filetype, __LOG_MODE _mode = __LOG_MODE_FILE);

    ~log          () = default;

public:
    void
    disable_info  () { m_disable_info = true; }

    void
    disable_debug () { m_disable_debug = true; }

    void
    enable_info   () { m_disable_info = false; }

    void
    enable_debug  () { m_disable_debug = false; }

public:
    bool
    fatal         (const char *_fmt, ...);

    bool
    error         (const char *_fmt, ...);

    bool
    warning       (const char *_fmt, ...);

    bool
    info          (const char *_fmt, ...);

    bool
    debug         (const char *_fmt, ...);

    bool
    log_data      (const std::string &_str);

    bool
    log_assert    (const char *_func, const char *_file, size_t _line, const char *_exp);

private:
    const char *
    get_datetime  ();

    bool
    write_log     (LOG_LEVEL _level, const char *_fmt, va_list _vl);

    bool
    write_logfile (LOG_LEVEL _level, const char *_file, const char *_fmt, size_t _len);

    const char *
    color_begin   (LOG_LEVEL _level);

    const char *
    color_end     ();

    void
    write_stderr  (LOG_LEVEL _level, const char *_str, size_t _len);

    void
    write_stderr2 (LOG_LEVEL _level, const char *_fmt, ...);

private:
#ifdef __LOG_MUTEX
    mutex      m_mutex;
#endif

    __LOG_MODE m_mode;

    __LOG_FILE m_filetype;

    char       m_datetime[__LOG_DATETIME_LEN];

    bool       m_disable_info;

    bool       m_disable_debug;

private:
    friend class log_mgr;

    friend log_mgr &
    logger ();
};

__NAMESPACE_END

#endif /* _K_LOG_H_ */
