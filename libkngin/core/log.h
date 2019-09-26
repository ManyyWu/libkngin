#ifndef _K_LOG_H_
#define _K_LOG_H_

#include <cstdarg>
#include "define.h"
#include "mutex.h"
#include "noncopyable.h"

#define __LOG_FILE_DATE_LEN 19
#define __LOG_BUF_SIZE      4096
#define __LOG_FILE_MAX_SIZE 20 * 1024 * 1024 // 20M

// "YYYY/MM/DD hh:mm:ss.ms | type | func[file:line] | fmt \n"
#define __log_format(__t, __f)        "%04d/%02d/%02d %02d:%02d:%02d.%03d | " __t " | %s[%s:%d] | " __f

// "YYYY/MM/DD hh:mm:ss.ms | type | fmt \n"
#define __log_format_noline(__t, __f) "%04d/%02d/%02d %02d:%02d:%02d.%03d | " __t " | " __f

// "YYYY-MM-DD"
#define __log_filename_format  "%s_%04d-%02d-%02d.log"

// "****** func[file:line] ******"
#define __log_assert_format "%04d/%02d/%02d %02d:%02d:%02d.%03d | ASSERT  | %s[%s:%d] | ****** %s *******"

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
enum __LOG_FILE {
    __LOG_FILE_MEMORY = 0,
    __LOG_FILE_SERVER,
    __LOG_FILE_HTTP,
    __LOG_FILE_MAX
};

enum __LOG_MODE {
    __LOG_MODE_FILE,   // only output to file
    __LOG_MODE_STDERR, // only output to stderr
    __LOG_MODE_BOTH    // the above two
};

enum LOG_LEVEL {
    LOG_LEVEL_FATAL = 0,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_MAX
};

class log_mgr;
class log : public noncopyable {
private:
    log           (__LOG_FILE _filetype, __LOG_MODE _mode = __LOG_MODE_FILE);

    ~log          ();

public:
    bool 
    init          ();

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
    log_data    (const char *_data, int _len);

    bool
    log_assert (const char *_func, const char *_file, int _line, const char *_exp);

private:
    bool
    write_log     (LOG_LEVEL _level, const char *_fmt, va_list _vl);

    bool
    write_logfile (LOG_LEVEL _level, const char *_file, const char *_fmt, int _len);

    void
    write_stderr  (LOG_LEVEL _level, const char *_str, int _len);

    void
    write_stderr2 (LOG_LEVEL _level, const char *_fmt, ...);


private:
#ifdef __LOG_MUTEX
    mutex *    m_mutex;
#endif

    __LOG_MODE m_mode;

    __LOG_FILE m_filetype;

private:
    friend class log_mgr;

    friend log_mgr &
    logger                  ();
};

__NAMESPACE_END

#endif /* _K_LOG_H_ */
