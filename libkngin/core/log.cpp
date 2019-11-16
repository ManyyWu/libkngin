#include <cstdio>
#include <cstdarg>
#include <cerrno>
#include <cstring>
#include <ctime>
#include "log.h"
#include "logmgr.h"
#include "common.h"
#include "lock.h"
#include "timestamp.h"

__NAMESPACE_BEGIN

log::log (__LOG_FILE _filetype, __LOG_MODE _mode /* = __LOG_MODE_FILE */)
    try
    : m_mutex(),
      m_mode(_mode),
      m_filetype(_filetype),
      m_disable_info(false),
      m_disable_debug(false)
{
} catch (...) {
    throw;
}

bool
log::fatal (const char *_fmt, ...)
{
    va_list _vl;
    va_start(_vl, _fmt);
    bool _ret = write_log(LOG_LEVEL_FATAL, _fmt, _vl);
    va_end(_vl);
    assert(!"log fatal");
    return _ret;
}

bool
log::error (const char *_fmt, ...)
{
    va_list _vl;
    va_start(_vl, _fmt);
    bool _ret = write_log(LOG_LEVEL_ERROR, _fmt, _vl);
    va_end(_vl);
    assert(!"log error");
    return _ret;
}

bool
log::warning (const char *_fmt, ...)
{
    va_list _vl;
    va_start(_vl, _fmt);
    bool _ret = write_log(LOG_LEVEL_WARNING, _fmt, _vl);
    va_end(_vl);
    return _ret;
}

bool
log::info (const char *_fmt, ...)
{
    if (m_disable_info)
        return true;

    va_list _vl;
    va_start(_vl, _fmt);
    bool _ret = write_log(LOG_LEVEL_INFO, _fmt, _vl);
    va_end(_vl);
    return _ret;
}

bool
log::debug (const char *_fmt, ...)
{
    if (m_disable_debug)
        return true;

    va_list _vl;
    va_start(_vl, _fmt);
    bool _ret = write_log(LOG_LEVEL_DEBUG, _fmt, _vl);
    va_end(_vl);
    return _ret;
}

bool
log::log_data (const char *_data, int _len)
{
    assert(_data && _len);

    bool _ret = true;
    if (__LOG_MODE_BOTH == m_mode || __LOG_MODE_FILE == m_mode)
        _ret = write_logfile(LOG_LEVEL_DEBUG, logger().filename_at(m_filetype).c_str(), _data, _len);
    if (__LOG_MODE_BOTH == m_mode || __LOG_MODE_STDERR == m_mode)
        write_stderr(LOG_LEVEL_DEBUG, _data, _len);
    return _ret;
}

bool
log::log_assert (const char *_func, const char *_file, int _line, const char *_exp)
{
    return fatal(__log_assert_format, _func, _file, _line, _exp);
}

const char *
log::get_datetime ()
{
    time_t _t = ::time(nullptr);
    struct tm _tm;
    __localtime(&_tm, &_t);
    ::snprintf(m_datetime, __LOG_DATETIME_LEN + 1,
               __log_datetime_format,
               _tm.tm_year + 1900, _tm.tm_mon, _tm.tm_mday,
               _tm.tm_hour, _tm.tm_min, _tm.tm_sec);
    m_datetime[__LOG_DATETIME_LEN] = '\0';
    return m_datetime;
}

bool
log::write_log (LOG_LEVEL _level, const char *_fmt, va_list _vl)
{
    assert(_fmt);

    bool _ret = true;
    char _buf[__LOG_BUF_SIZE + 1];
    ::strncpy(_buf, get_datetime(), __LOG_DATETIME_LEN + 1);
    ::vsnprintf(_buf + __LOG_DATETIME_LEN, __LOG_BUF_SIZE - __LOG_DATETIME_LEN, _fmt, _vl);
    _buf[__LOG_BUF_SIZE] = '\0';

    int _len = (int)::strnlen(_buf, __LOG_BUF_SIZE);
    if (__LOG_MODE_BOTH == m_mode || __LOG_MODE_FILE == m_mode)
        _ret = write_logfile(_level, logger().filename_at(m_filetype).c_str(), _buf, _len);
    if (__LOG_MODE_BOTH == m_mode || __LOG_MODE_STDERR == m_mode)
        write_stderr(_level, _buf, _len);
    return _ret;
}

bool
log::write_logfile (LOG_LEVEL _level, const char *_file, const char *_str, int _len)
{
    assert(_str);
#ifdef __LOG_MUTEX
    if (logger().inited()) {
        m_mutex.lock();
    }
#endif

    bool         _fail = false;
    int          _ret = 0;
    char         _buf[__LOG_BUF_SIZE + 1] = {0};
    char         _filename[FILENAME_MAX + 1];
    tm           _tm;
    time_t       _t = ::time(nullptr);
    FILE *       _fplog = nullptr;
    const char * _datetime = get_datetime();

    __localtime(&_tm, &_t);
    ::snprintf(_filename, FILENAME_MAX, __log_filename_format, _file,
             _tm.tm_year + 1900, _tm.tm_mon, _tm.tm_mday);
    _fplog = ::fopen(_filename, "a");
    if (!_fplog) {
        write_stderr2(LOG_LEVEL_FATAL,
                            __log_format("FATAL", "failed to open \"%s\" - %s[%#x]"),
                            _datetime, __FUNCTION__, __FILE__, __LINE__, _filename,
                            ::strerror(errno), errno);
        goto fail;
    }

    ::fseek(_fplog, 0, SEEK_END);
    if (0 == ftell(_fplog)) {
        // write head info
        ::snprintf(_buf, __LOG_BUF_SIZE,
                 "=========================================================\n"
                 "current time: %s\n"
                 "=========================================================\n",
                 _datetime);
        size_t _str_len = ::strnlen(_buf, __LOG_BUF_SIZE);
        _ret = (int)::fwrite(_buf, 1, _str_len , _fplog);
        if (_ret < 0) {
            write_stderr2(LOG_LEVEL_FATAL,
                                __log_format("FATAL", "failed to write log to \"%s\" - %s[%#x]"),
                                _datetime, __FUNCTION__, __FILE__, __LINE__, _filename,
                                ::strerror(errno), errno);
            ::fclose(_fplog);
            goto fail;
        } else if ((size_t)_ret != _str_len) {
             write_stderr2(LOG_LEVEL_FATAL,
                                 __log_format("ERROR", "the content been written to \"%s\" are too short, "
                                              "and the disk space may be insufficient"),
                                 _datetime, __FUNCTION__, __FILE__, __LINE__, _filename);
            ::fclose(_fplog);
            goto fail;
        }
    }

    _ret = (int)::fwrite(_str, 1, _len, _fplog);
    if (_ret < 0) {
        write_stderr2(LOG_LEVEL_FATAL,
                            __log_format("FATAL", "failed to write log to \"%s\" - write %s[%#x]"),
                            _datetime, __FUNCTION__, __FILE__, __LINE__,
                            _filename, ::strerror(errno), errno);
        ::fclose(_fplog);
        goto fail;
    }
    ::fputc('\n', _fplog);
    ::fflush(_fplog);
    ::fclose(_fplog);

    _fail = true;
fail:
#ifdef __LOG_MUTEX
    if (logger().inited())
        m_mutex.unlock();
#endif
    return _fail;
}

void
log::write_stderr (LOG_LEVEL _level, const char *_str, int _len)
{
    assert(_str);

#ifdef __LOG_MUTEX
    if (logger().inited()) {
        m_mutex.lock();
    }
#endif

#ifdef _WIN32
#else
    switch (_level) {
    case LOG_LEVEL_FATAL:
        ::fputs(__COLOR_FATAL, stderr);
        break;
    case LOG_LEVEL_ERROR:
        ::fputs(__COLOR_ERROR, stderr);
        break;
    case LOG_LEVEL_WARNING:
        ::fputs(__COLOR_WARNING, stderr);
        break;
    case LOG_LEVEL_INFO:
        ::fputs(__COLOR_INFO, stderr);
        break;
    case LOG_LEVEL_DEBUG:
        ::fputs(__COLOR_DEBUG, stderr);
        break;
    default:
        ::fputs(__COLOR_ASSERT, stderr);
        break;
    }
#endif
    ::fwrite(_str, 1, _len, stderr);
#ifdef _WIN32
#else
    ::fputs(__COLOR_NONE, stderr);
#endif
    ::fputc('\n', stderr);

#ifdef __LOG_MUTEX
    if (logger().inited())
        m_mutex.unlock();
#endif
}

void
log::write_stderr2 (LOG_LEVEL _level, const char *_fmt, ...)
{
    assert(_fmt);

#ifdef __LOG_MUTEX
    if (logger().inited()) {
        m_mutex.lock();
    }
#endif

    va_list _vl;
    char _buf[__LOG_BUF_SIZE + 1];

#ifdef _WIN32
#else
    switch (_level) {
        case LOG_LEVEL_FATAL:
            ::fputs(__COLOR_FATAL, stderr);
            break;
        case LOG_LEVEL_ERROR:
            ::fputs(__COLOR_ERROR, stderr);
            break;
        case LOG_LEVEL_WARNING:
            ::fputs(__COLOR_WARNING, stderr);
            break;
        case LOG_LEVEL_INFO:
            ::fputs(__COLOR_INFO, stderr);
            break;
        case LOG_LEVEL_DEBUG:
            ::fputs(__COLOR_DEBUG, stderr);
            break;
        default:
            ::fputs(__COLOR_ASSERT, stderr);
            break;
    }
#endif
    va_start(_vl, _fmt);
    ::vsnprintf(_buf, __LOG_BUF_SIZE, _fmt, _vl);
    ::fwrite(_buf, 1, ::strnlen(_buf, __LOG_BUF_SIZE), stderr);
#ifdef _WIN32
#else
    ::fputs(__COLOR_NONE, stderr);
#endif
    ::fputc('\n', stderr);
    va_end(_vl);

#ifdef __LOG_MUTEX
    if (logger().inited())
        m_mutex.unlock();
#endif
}

__NAMESPACE_END