#ifndef _K_LOG_H_
#define _K_LOG_H_

#include <cstdarg>
#include <string>
#include "core/define.h"
#include "core/lock.h"

#define KNGIN_LOG_BUF_SIZE                     4096
#define KNGIN_LOG_FILE_MAX_SIZE                20 * 1024 * 1024 // 20M

// "YYYY/MM/DD hh:mm:ss"
#define KNGIN_LOG_DATETIME_FORMT               "%04d/%02d/%02d %02d:%02d:%02d"
#define KNGIN_LOG_DATETIME_LEN                 20

// "YYYY/MM/DD hh:mm:ss.ms | type | func[file:line] | fmt \n"
#define KNGIN_LOG_LOG_FORMAT(type, fmt)        " | " type " | %s[%s:%d] | " fmt

// "YYYY/MM/DD hh:mm:ss.ms | type | fmt \n"
#define KNGIN_LOG_LOG_FORMAT_NOLINE(type, fmt) " | " type " | " fmt

// "YYYY-MM-DD"
#define KNGIN_LOG_FILENAME_FORMT               "%s_%04d-%02d-%02d.log"

// "****** func[file:line] ******"
#define KNGIN_LOG_ASSERT_FORMAT                " | ASSERT  | %s[%s:%d] | ****** %s *******"

// color
#ifdef _WIN32
#else
#define KNGIN_LOG_COLOR_NONE    "\033[0m"
#define KNGIN_LOG_COLOR_FATAL   "\033[01;32;41m"
#define KNGIN_LOG_COLOR_ERROR   "\033[01;37;43m"
#define KNGIN_LOG_COLOR_WARNING "\033[01;31;49m"
#define KNGIN_LOG_COLOR_INFO    "\033[01;32;49m"
#define KNGIN_LOG_COLOR_DEBUG   ""
#define KNGIN_LOG_COLOR_ASSERT  "\033[05;37;41m"
#endif

KNGIN_NAMESPACE_K_BEGIN

/*
 * Type of log file
 */
enum KNGIN_LOG_FILE {          // reserved type
    KNGIN_LOG_FILE_SERVER = 0, // default
};

enum KNGIN_LOG_MODE {
    KNGIN_LOG_MODE_FILE = 0,   // only output to file
    KNGIN_LOG_MODE_STDERR,     // only output to stderr
    KNGIN_LOG_MODE_BOTH        // the above two
};

enum KNGIN_LOG_LEVEL {
    KNGIN_LOG_LEVEL_FATAL = 0,
    KNGIN_LOG_LEVEL_ERROR,
    KNGIN_LOG_LEVEL_WARNING,
    KNGIN_LOG_LEVEL_INFO,
    KNGIN_LOG_LEVEL_DEBUG,
};

class log_mgr;
class log {
public:
    log           () = delete;

private:
    explicit
    log           (KNGIN_LOG_FILE _filetype, KNGIN_LOG_MODE _mode = KNGIN_LOG_MODE_FILE);

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
    write_log     (KNGIN_LOG_LEVEL _level, const char *_fmt, va_list _vl);

    bool
    write_logfile (KNGIN_LOG_LEVEL _level, const char *_file, const char *_fmt, size_t _len);

    const char *
    color_begin   (KNGIN_LOG_LEVEL _level);

    const char *
    color_end     ();

    void
    write_stderr  (KNGIN_LOG_LEVEL _level, const char *_str, size_t _len);

    void
    write_stderr2 (KNGIN_LOG_LEVEL _level, const char *_fmt, ...);

private:
#if (ON == KNGIN_ENABLE_LOG_MUTEX)
    mutex          m_mutex;
#endif

    KNGIN_LOG_MODE m_mode;

    KNGIN_LOG_FILE m_filetype;

    char           m_datetime[KNGIN_LOG_DATETIME_LEN];

    bool           m_disable_info;

    bool           m_disable_debug;

private:
    friend class log_mgr;

    friend log_mgr &
    logger ();
};

KNGIN_NAMESPACE_K_END

#endif /* _K_LOG_H_ */
