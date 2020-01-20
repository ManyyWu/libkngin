#include <cstdio>
#include <cstdarg>
#include <cerrno>
#include <cstring>
#include <ctime>
#include "core/base/log.h"
#include "core/base/logmgr.h"
#include "core/base/common.h"
#include "core/base/lock.h"
#include "core/base/timestamp.h"

KNGIN_NAMESPACE_K_BEGIN

const char * const
log::log_color_begin_tbl[KNGIN_LOG_LEVEL_MAX + 1] = {
    KNGIN_LOG_COLOR_FATAL,
    KNGIN_LOG_COLOR_ERROR,
    KNGIN_LOG_COLOR_WARNING,
    KNGIN_LOG_COLOR_INFO,
    KNGIN_LOG_COLOR_DEBUG
};

log::log (KNGIN_LOG_FILE _filetype, KNGIN_LOG_MODE _mode /* = KNGIN_LOG_MODE_FILE */)
    :
#if (ON == KNGIN_ENABLE_LOG_MUTEX)
      m_mutex(),
#endif
      m_mode(_mode),
      m_filetype(_filetype),
      m_disable_info(false),
      m_disable_debug(false)
{
}

bool
log::fatal (const char *_fmt, ...) KNGIN_NOEXCP
{
    assert(_fmt);
    va_list _vl;
    va_start(_vl, _fmt);
    bool _ret = write_log(KNGIN_LOG_LEVEL_FATAL, _fmt, _vl);
    va_end(_vl);
    assert(!"log fatal");
    return _ret;
}

bool
log::error (const char *_fmt, ...) KNGIN_NOEXCP
{
    assert(_fmt);
    va_list _vl;
    va_start(_vl, _fmt);
    bool _ret = write_log(KNGIN_LOG_LEVEL_ERROR, _fmt, _vl);
    va_end(_vl);
    //assert(!"log error");
    return _ret;
}

bool
log::warning (const char *_fmt, ...) KNGIN_NOEXCP
{
    assert(_fmt);
    va_list _vl;
    va_start(_vl, _fmt);
    bool _ret = write_log(KNGIN_LOG_LEVEL_WARNING, _fmt, _vl);
    va_end(_vl);
    return _ret;
}

bool
log::info (const char *_fmt, ...) KNGIN_NOEXCP
{
    assert(_fmt);
    if (m_disable_info)
        return true;

    va_list _vl;
    va_start(_vl, _fmt);
    bool _ret = write_log(KNGIN_LOG_LEVEL_INFO, _fmt, _vl);
    va_end(_vl);
    return _ret;
}

bool
log::debug (const char *_fmt, ...) KNGIN_NOEXCP
{
    assert(_fmt);
    if (m_disable_debug)
        return true;

    va_list _vl;
    va_start(_vl, _fmt);
    bool _ret = write_log(KNGIN_LOG_LEVEL_DEBUG, _fmt, _vl);
    va_end(_vl);
    return _ret;
}

bool
log::log_data (const std::string &_str) KNGIN_NOEXCP
{
    if (_str.empty())
        return false;

    bool _ret = true;
    if (KNGIN_LOG_MODE_BOTH == m_mode or KNGIN_LOG_MODE_FILE == m_mode)
        _ret = write_logfile(KNGIN_LOG_LEVEL_DEBUG,
                             logger().filename_at(m_filetype).c_str(),
                             _str.c_str(), _str.size());
    if (KNGIN_LOG_MODE_BOTH == m_mode or KNGIN_LOG_MODE_STDERR == m_mode)
        write_stderr(KNGIN_LOG_LEVEL_DEBUG, _str.c_str(), _str.size());
    return _ret;
}

bool
log::log_assert (const char *_func, const char *_file,
                 size_t _line, const char *_exp) KNGIN_NOEXCP
{
    assert(_func);
    assert(_file);
    assert(_exp);
    return fatal(KNGIN_LOG_ASSERT_FORMAT, _func, _file, _line, _exp);
}

bool
log::write_log (KNGIN_LOG_LEVEL _level, const char *_fmt, va_list _vl) KNGIN_NOEXCP
{
    assert(_fmt);

    auto _func = [&] () -> bool {
        bool _ret = true;
#if (ON == KNGIN_ASYNC_LOGGER)
        log_data_ptr _data_ptr = k::make_shared_array<char>(KNGIN_LOG_BUF_SIZE);
        char *_buf = _data_ptr.get();
#else
        char  _buf[KNGIN_LOG_BUF_SIZE];
        char *_data_ptr = _buf;
#endif
        char _datetime[KNGIN_LOG_DATETIME_LEN];

        ::strncpy(_buf, get_datetime(_datetime, sizeof(_datetime)), sizeof(_datetime));
        ::vsnprintf(_buf + KNGIN_LOG_DATETIME_LEN - 1,
                    KNGIN_LOG_BUF_SIZE - KNGIN_LOG_DATETIME_LEN,
                    _fmt, _vl);
        _buf[KNGIN_LOG_BUF_SIZE - 1] = '\0';
        size_t _len = ::strnlen(_buf, KNGIN_LOG_BUF_SIZE);

#if (ON == KNGIN_ASYNC_LOGGER)
        logger().async_log(
#endif
        [_filetype = m_filetype, _level, _mode = m_mode,
         _data_ptr, _buf, _len] () mutable {
            if (KNGIN_LOG_MODE_BOTH == _mode or KNGIN_LOG_MODE_FILE == _mode)
                write_logfile(_level, logger().filename_at(_filetype).c_str(),
                                     _buf, _len);
            if (KNGIN_LOG_MODE_BOTH == _mode or KNGIN_LOG_MODE_STDERR == _mode)
                write_stderr(_level, _buf, _len);
#if (ON == KNGIN_ASYNC_LOGGER)
        });
#else
        ();
#endif
        return _ret;
    }; // end of write_log

#if (ON == KNGIN_ENABLE_LOG_MUTEX)
    if (logger().inited()) {
        local_lock _lock(m_mutex);
        return _func();
    } else
#endif
        return _func();
}

bool
log::write_logfile (KNGIN_LOG_LEVEL _level, const char *_file,
                    const char *_str, size_t _len) KNGIN_NOEXCP
{
    assert(_file);
    assert(_str);

    bool         _fail = false;
    size_t       _ret = 0;
    static char  _buf[KNGIN_LOG_BUF_SIZE];
    static char  _filename[FILENAME_MAX];
    tm           _tm;
    time_t       _t = ::time(nullptr);
    FILE *       _fplog = nullptr;

    get_localtime(&_tm, &_t);
    ::snprintf(_filename, FILENAME_MAX, KNGIN_LOG_FILENAME_FORMT, _file,
               _tm.tm_year + 1900, _tm.tm_mon, _tm.tm_mday);
    _fplog = ::fopen(_filename, "a");
    if (!_fplog) {
        write_stderr2(KNGIN_LOG_LEVEL_FATAL,
                      "failed to open \"%s\", %s[%#x]",
                      _filename, ::strerror(errno), errno);
        return false;
    }

    ::fseek(_fplog, 0, SEEK_END);
    if (0 == ftell(_fplog)) {
        // write head info
        char _datetime[KNGIN_LOG_DATETIME_LEN];
        ::snprintf(_buf, KNGIN_LOG_BUF_SIZE,
                   "=========================================================\n"
                   "= %19s                                   =\n"
                   "=========================================================\n",
                   get_datetime(_datetime, sizeof(_datetime)));
        size_t _str_len = ::strnlen(_buf, KNGIN_LOG_BUF_SIZE);
        _ret = ::fwrite(_buf, 1, _str_len , _fplog);
        if (_ret < 0) {
            write_stderr2(KNGIN_LOG_LEVEL_FATAL,
                          "failed to write log to \"%s\", %s[%#x]",
                          _filename, ::strerror(errno), errno);
            goto fail;
        } else if (static_cast<size_t>(_ret) != _str_len) {
             write_stderr2(KNGIN_LOG_LEVEL_FATAL,
                           "the content been written to \"%s\" are too short, "
                           "and the disk space may be insufficient",
                           _filename);
            goto fail;
        }
    }

    _ret = ::fwrite(_str, 1, _len, _fplog);
    if (_ret < 0) {
        write_stderr2(KNGIN_LOG_LEVEL_FATAL,
                      "failed to write log to \"%s\", %s[%#x]",
                      _filename, ::strerror(errno), errno);
        goto fail;
    }
    ::fputc('\n', _fplog);
    ::fflush(_fplog);

    _fail = true;
fail:
    ::fclose(_fplog);
    return _fail;
}

const char *
log::get_datetime (char _datetime[], size_t _size) KNGIN_NOEXCP
{
    assert(_size >= KNGIN_LOG_DATETIME_LEN);
    time_t _t = ::time(nullptr);
    struct ::tm _tm;
    get_localtime(&_tm, &_t);
    ::snprintf(_datetime, KNGIN_LOG_DATETIME_LEN,
               KNGIN_LOG_DATETIME_FORMT,
               _tm.tm_year + 1900, _tm.tm_mon, _tm.tm_mday,
               _tm.tm_hour, _tm.tm_min, _tm.tm_sec);
    _datetime[KNGIN_LOG_DATETIME_LEN - 1] = '\0';
    return _datetime;
}

const char *
log::color_begin (KNGIN_LOG_LEVEL _level) KNGIN_NOEXCP
{
#ifdef _WIN32
#else
    return log::log_color_begin_tbl[_level];
#endif
}

const char *
log::color_end () KNGIN_NOEXCP
{
#ifdef _WIN32
#else
    return KNGIN_LOG_COLOR_NONE;
#endif
}

void
log::write_stderr (KNGIN_LOG_LEVEL _level, const char *_str, size_t _len) KNGIN_NOEXCP
{
    assert(_str);

    ::fputs(color_begin(_level), stderr);
    ::fwrite(_str, 1, _len, stderr);
    ::fputs(color_end(), stderr);
    ::fputc('\n', stderr);
}

void
log::write_stderr2 (KNGIN_LOG_LEVEL _level, const char *_fmt, ...) KNGIN_NOEXCP
{
    assert(_fmt);

    va_list _vl;
    char _buf[KNGIN_LOG_BUF_SIZE];

    va_start(_vl, _fmt);
    ::vsnprintf(_buf, KNGIN_LOG_BUF_SIZE, _fmt, _vl);
    va_end(_vl);

    ::fputs(color_begin(_level), stderr);
    ::fwrite(_buf, 1, ::strnlen(_buf, KNGIN_LOG_BUF_SIZE), stderr);
    ::fputs(KNGIN_LOG_COLOR_NONE, stderr);
    ::fputc('\n', stderr);
}

KNGIN_NAMESPACE_K_END
