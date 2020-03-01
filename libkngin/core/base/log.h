#ifndef KNGIN_K_LOG_H
#define KNGIN_K_LOG_H

#include <cstdarg>
#include <string>
#include <functional>
#include "core/base/define.h"
#include "core/base/lock.h"
#include "core/base/memory.h"
#include "core/base/noncopyable.h"

#if (ON == KNGIN_ASYNC_LOGGER)
#define KNGIN_ENABLE_LOG_MUTEX OFF
#else
#define KNGIN_ENABLE_LOG_MUTEX ON
#endif

#define KNGIN_LOG_BUF_SIZE                 4096
#define KNGIN_LOG_FILE_MAX_SIZE            20 * 1024 * 1024 // 20M

// "YYYY/MM/DD hh:mm:ss"
#define KNGIN_LOG_DATETIME_FORMT           "%04d/%02d/%02d %02d:%02d:%02d"
#define KNGIN_LOG_DATETIME_LEN             20

// "YYYY/MM/DD hh:mm:ss.ms | type | func[file:line] | fmt \n"
#define KNGIN_LOG_FORMAT(type, fmt)        " | " type " | %s[%s:%d] | " fmt

// "YYYY/MM/DD hh:mm:ss.ms | type | fmt \n"
#define KNGIN_LOG_FORMAT_NOLINE(type, fmt) " | " type " | " fmt

// "YYYY-MM-DD"
#define KNGIN_LOG_FILENAME_FORMT           "%s_%04d-%02d-%02d.log"

// "****** func[file:line] ******"
#define KNGIN_LOG_ASSERT_FORMAT            " | ASSERT  | %s[%s:%d] | ****** %s *******"

// color
#ifdef _WIN32
#else
#define KNGIN_LOG_COLOR_NONE    "\033[0m"
#define KNGIN_LOG_COLOR_FATAL   "\033[01;32;41m"
#define KNGIN_LOG_COLOR_ERROR   "\033[01;37;43m"
#define KNGIN_LOG_COLOR_WARNING "\033[01;31;49m"
#define KNGIN_LOG_COLOR_INFO    "\033[01;32;49m"
#define KNGIN_LOG_COLOR_DEBUG   ""
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
    KNGIN_LOG_LEVEL_MAX = KNGIN_LOG_LEVEL_DEBUG
};

class log_mgr;
class log : public noncopyable {
    friend class log_mgr;

#if (ON == KNGIN_ASYNC_LOGGER)
public:
    typedef std::shared_ptr<char>      log_data_ptr;
    // XXX: use k::make_shared_array() to create log_data_ptr
    // FIXME: use std::shared<char []> in c++ 20

    typedef std::function<void (void)> async_log_data;

#endif

public:
    log           () = delete;

    log           (KNGIN_LOG_FILE _filetype,
                   KNGIN_LOG_MODE _mode = KNGIN_LOG_MODE_FILE);

    ~log          () = default;

public:
    void
    disable_info  () noexcept { m_disable_info = true; }

    void
    disable_debug () noexcept { m_disable_debug = true; }

    void
    enable_info   () noexcept { m_disable_info = false; }

    void
    enable_debug  () noexcept { m_disable_debug = false; }

public:
    void
    fatal         (const char *_fmt, ...);

    void
    error         (const char *_fmt, ...);

    void
    warning       (const char *_fmt, ...);

    void
    info          (const char *_fmt, ...);

    void
    debug         (const char *_fmt, ...);

    void
    log_data      (const std::string &_str);

    bool
    log_assert    (const char *_func, const char *_file,
                   size_t _line, const char *_exp);

    void
    write_log     (KNGIN_LOG_LEVEL _level,
                   const char *_fmt, va_list _vl);

private:
    static bool
    write_logfile (const char *_file, const char *_fmt, size_t _len) noexcept;

    static void
    write_stderr  (KNGIN_LOG_LEVEL _level,
                   const char *_str, size_t _len) noexcept;

    static void
    write_stderr2 (KNGIN_LOG_LEVEL _level,
                   const char *_fmt, ...) noexcept;

private:
    static const char *
    get_datetime  (char _datetime[], size_t _size) noexcept;

    static const char *
    color_begin   (KNGIN_LOG_LEVEL _level) noexcept;

    static const char *
    color_end     () noexcept;

private:
#if (ON == KNGIN_ENABLE_LOG_MUTEX)
    mutex          m_mutex;
#endif

    KNGIN_LOG_MODE m_mode;

    KNGIN_LOG_FILE m_filetype;

    bool           m_disable_info;

    bool           m_disable_debug;

private:
    static const char * const log_color_begin_tbl[KNGIN_LOG_LEVEL_MAX + 1];

    friend log_mgr &
    logger ();
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_K_LOG_H */
