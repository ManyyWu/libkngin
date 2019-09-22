#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <cerrno>
#include <ctime>
#include "log.h"
#include "logfile.h"
#include "logmgr.h"
#include "common.h"
#include "define.h"
#include "mutex.h"
#include "time.h"

__NAMESPACE_BEGIN

#ifdef __LOG_MUTEX
log::log (__LOG_FILE _filetype, __LOG_MODE _mode /* = __LOG_MODE_FILE */)
    : m_filetype(_filetype), m_mutex(NULL), m_mode(_mode)
#else
log::log (__LOG_FILE _filetype, __LOG_MODE _mode /* = __LOG_MODE_FILE*/)
    : m_filetype(_filetype), m_mode(_mode)
#endif
{
    m_mode = __LOG_MODE_BOTH;
}

log::~log ()
{
#ifdef __LOG_MUTEX
    if (m_mutex)
        m_mutex->release();
#endif
}

bool
log::init ()
{
#ifdef __LOG_MUTEX
    m_mutex = mutex::create();
    if_not (m_mutex) {
        write_stderr2(LOG_LEVEL_FATAL, "mutex create failed");
        return false;
    }
#endif
    return true;
}

bool
log::fatal (const char *_fmt, ...)
{
    kassert_r0(_fmt);

    va_list _vl;
    char _buf[__LOG_BUF_SIZE + 1] = {0};

    va_start(_vl, _fmt);
    vsnprintf(_buf, __LOG_BUF_SIZE, _fmt, _vl);
    va_end(_vl);
    int _len = (int)strnlen(_buf, __LOG_BUF_SIZE);
    if (__LOG_MODE_BOTH == m_mode || __LOG_MODE_FILE == m_mode)
        if (!this->write_logfile(LOG_LEVEL_FATAL, logger().filename_at(m_filetype).c_str(), _buf, _len))
            return false;
    if (__LOG_MODE_BOTH == m_mode || __LOG_MODE_STDERR == m_mode)
        this->write_stderr(LOG_LEVEL_FATAL, _buf, _len);

    return true;
}

bool
log::error (const char *_fmt, ...)
{
    kassert_r0(_fmt);

    va_list _vl;
    char _buf[__LOG_BUF_SIZE + 1] = {0};
    va_start(_vl, _fmt);
    vsnprintf(_buf, __LOG_BUF_SIZE, _fmt, _vl);
    va_end(_vl);
    int _len = (int)strnlen(_buf, __LOG_BUF_SIZE);
    if (__LOG_MODE_BOTH == m_mode || __LOG_MODE_FILE == m_mode)
        if (!this->write_logfile(LOG_LEVEL_ERROR, logger().filename_at(m_filetype).c_str(), _buf, _len))
            return false;
    if (__LOG_MODE_BOTH == m_mode || __LOG_MODE_STDERR == m_mode)
        this->write_stderr(LOG_LEVEL_ERROR, _buf, _len);

    return true;
}

bool
log::warning (const char *_fmt, ...)
{
    kassert_r0(_fmt);

    va_list _vl;
    char _buf[__LOG_BUF_SIZE + 1] = {0};
    va_start(_vl, _fmt);
    vsnprintf(_buf, __LOG_BUF_SIZE, _fmt, _vl);
    va_end(_vl);
    int _len = (int)strnlen(_buf, __LOG_BUF_SIZE);
    if (__LOG_MODE_BOTH == m_mode || __LOG_MODE_FILE == m_mode)
        if (!this->write_logfile(LOG_LEVEL_WARNING, logger().filename_at(m_filetype).c_str(), _buf, _len))
            return false;
    if (__LOG_MODE_BOTH == m_mode || __LOG_MODE_STDERR == m_mode)
        this->write_stderr(LOG_LEVEL_WARNING, _buf, _len);
    return true;
}

bool
log::info (const char *_fmt, ...)
{
    kassert_r0(_fmt);

    va_list _vl;
    char _buf[__LOG_BUF_SIZE + 1] = {0};
    va_start(_vl, _fmt);
    vsnprintf(_buf, __LOG_BUF_SIZE, _fmt, _vl);
    va_end(_vl);
    int _len = (int)strnlen(_buf, __LOG_BUF_SIZE);
    if (__LOG_MODE_BOTH == m_mode || __LOG_MODE_FILE == m_mode)
        if (!this->write_logfile(LOG_LEVEL_INFO, logger().filename_at(m_filetype).c_str(), _buf, _len))
            return false;
    if (__LOG_MODE_BOTH == m_mode || __LOG_MODE_STDERR == m_mode)
        this->write_stderr(LOG_LEVEL_INFO, _buf, _len);
    return true;
}

bool
log::debug (const char *_fmt, ...)
{
    kassert_r0(_fmt);

    va_list _vl;
    char _buf[__LOG_BUF_SIZE + 1] = {0};
    va_start(_vl, _fmt);
    vsnprintf(_buf, __LOG_BUF_SIZE, _fmt, _vl);
    va_end(_vl);
    int _len = (int)strnlen(_buf, __LOG_BUF_SIZE);
    if (__LOG_MODE_BOTH == m_mode || __LOG_MODE_FILE == m_mode)
        if (!this->write_logfile(LOG_LEVEL_DEBUG, logger().filename_at(m_filetype).c_str(), _buf, _len))
            return false;
    if (__LOG_MODE_BOTH == m_mode || __LOG_MODE_STDERR == m_mode)
        this->write_stderr(LOG_LEVEL_DEBUG, _buf, _len);

    return true;
}

bool
log::write_data (const char *_data, int _len)
{
    kassert_r0(_data && _len);

    if (__LOG_MODE_BOTH == m_mode || __LOG_MODE_FILE == m_mode)
        if (!this->write_logfile(LOG_LEVEL_DEBUG, logger().filename_at(m_filetype).c_str(), _data, _len))
            return false;
    if (__LOG_MODE_BOTH == m_mode || __LOG_MODE_STDERR == m_mode)
        this->write_stderr(LOG_LEVEL_DEBUG, _data, _len);
}

bool
log::write_logfile (LOG_LEVEL _level, const char *_file, const char *_str, int _len)
{
    kassert_r0(_str);
#ifdef __LOG_MUTEX
    kassert_r0(m_mutex);
    m_mutex->lock();
#endif

    int     _ret = 0;
    char    _buf[__LOG_BUF_SIZE + 1] = {0};
    char    _filename[FILENAME_MAX + 1];
    tm      _tm;
    time_t  _t = time(NULL);
    FILE *  _fplog = NULL;

    __localtime(&_tm, &_t);
    snprintf(_filename, FILENAME_MAX, __log_filename_format, _file,
             _tm.tm_year + 1900, _tm.tm_mon, _tm.tm_mday);
    _fplog = fopen(_filename, "a");
    if (!_fplog) {
        this->write_stderr2(LOG_LEVEL_FATAL,
                            __log_format("FATAL", "failed to open \"%s\" - %s[%#x]"),
                            _tm.tm_year + 1900, _tm.tm_mon, _tm.tm_mday,
                            _tm.tm_hour, _tm.tm_min, _tm.tm_sec, 0,
                            __FUNCTION__, __FILE__, __LINE__,
                            _filename,
                            strerror(errno),
                            errno);
        goto fail;
    }

    fseek(_fplog, 0, SEEK_END);
    if (0 == ftell(_fplog)) {
        // write head info
        snprintf(_buf, __LOG_BUF_SIZE,
                 "=========================================================\n" \
                 "current time: %04d/%02d/%02d %02d:%02d:%02d\n"               \
                 "=========================================================\n",
                 _tm.tm_year, _tm.tm_mon, _tm.tm_mday,
                 _tm.tm_hour, _tm.tm_min, _tm.tm_sec
                 );
        size_t _str_len = strnlen(_buf, __LOG_BUF_SIZE);
        _ret = (int)fwrite(_buf, 1, _str_len , _fplog);
        if (_ret < 0) {
            this->write_stderr2(LOG_LEVEL_FATAL,
                                __log_format("FATAL", "failed to write log to \"%s\" - %s[%#x]"),
                                _tm.tm_year + 1900, _tm.tm_mon, _tm.tm_mday,
                                _tm.tm_hour, _tm.tm_min, _tm.tm_sec, 0,
                                __FUNCTION__, __FILE__, __LINE__,
                                _filename,
                                strerror(errno),
                                errno);
            fclose(_fplog);
            goto fail;
        } else if (_ret != _str_len) {
             this->write_stderr2(LOG_LEVEL_FATAL,
                                 __log_format("ERROR", 
                                              "the content been writen to \"%s\" are too short, "
                                              "and the disk space may be insufficient"),
                                 _tm.tm_year + 1900, _tm.tm_mon, _tm.tm_mday,
                                 _tm.tm_hour, _tm.tm_min, _tm.tm_sec, 0,
                                 __FUNCTION__, __FILE__, __LINE__,
                                 _filename);
            fclose(_fplog);
            goto fail;
        }
    }

    _ret = (int)fwrite(_str, 1, _len, _fplog);
    if (_ret < 0) {
        this->write_stderr2(LOG_LEVEL_FATAL,
                            __log_format("FATAL", "failed to write log to \"%s\" - write %s[%#x]"),
                            _tm.tm_year + 1900, _tm.tm_mon, _tm.tm_mday,
                            _tm.tm_hour, _tm.tm_min, _tm.tm_sec, 0,
                            __FUNCTION__, __FILE__, __LINE__,
                            _filename,
                            strerror(errno),
                            errno);
        fclose(_fplog);
        goto fail;
    }
    fputc('\n', _fplog);
    fflush(_fplog);
    fclose(_fplog);

#ifdef __LOG_MUTEX
    m_mutex->unlock();
#endif
    return true;
fail:
    return false;
}

void
log::write_stderr (LOG_LEVEL _level, const char *_str, int _len)
{
    kassert_r(_str);

#ifdef __LOG_MUTEX
    kassert_r(m_mutex);
    m_mutex->lock();
#endif

#ifdef _WIN32
#else
    switch (_level) {
    case LOG_LEVEL_FATAL:
        fputs(__COLOR_FATAL, stderr);
        break;
    case LOG_LEVEL_ERROR:
        fputs(__COLOR_ERROR, stderr);
        break;
    case LOG_LEVEL_WARNING:
        fputs(__COLOR_WARNING, stderr);
        break;
    case LOG_LEVEL_INFO:
        fputs(__COLOR_INFO, stderr);
        break;
    case LOG_LEVEL_DEBUG:
        fputs(__COLOR_DEBUG, stderr);
        break;
    default:
        fputs(__COLOR_ASSERT, stderr);
        break;
    }
#endif
    size_t _ret = fwrite(_str, 1, _len, stderr);
#ifdef _WIN32
#else
    fputs(__COLOR_NONE, stderr);
#endif
    fputc('\n', stderr);
    fflush(stderr);

fail:
#ifdef __LOG_MUTEX
    m_mutex->unlock();
#endif
    return;
}

void
log::write_stderr2 (LOG_LEVEL _level, const char *_fmt, ...)
{
    kassert_r(_fmt);

#ifdef __LOG_MUTEX
    kassert_r(m_mutex);
    m_mutex->lock();
#endif

    va_list _vl;
    char _buf[__LOG_BUF_SIZE + 1];

#ifdef _WIN32
#else
    switch (_level) {
        case LOG_LEVEL_FATAL:
            fputs(__COLOR_FATAL, stderr);
            break;
        case LOG_LEVEL_ERROR:
            fputs(__COLOR_ERROR, stderr);
            break;
        case LOG_LEVEL_WARNING:
            fputs(__COLOR_WARNING, stderr);
            break;
        case LOG_LEVEL_INFO:
            fputs(__COLOR_INFO, stderr);
            break;
        case LOG_LEVEL_DEBUG:
            fputs(__COLOR_DEBUG, stderr);
            break;
        default:
            fputs(__COLOR_ASSERT, stderr);
            break;
    }
#endif
    va_start(_vl, _fmt);
    vsnprintf(_buf, __LOG_BUF_SIZE, _fmt, _vl);
    size_t _len = strnlen(_buf, __LOG_BUF_SIZE);
    size_t _ret = fwrite(_buf, 1, _len, stderr);
#ifdef _WIN32
#else
    fputs(__COLOR_NONE, stderr);
#endif
    fputc('\n', stderr);
    fflush(stderr);
    va_end(_vl);

fail:
#ifdef __LOG_MUTEX
    m_mutex->unlock();
#endif
}

__NAMESPACE_END