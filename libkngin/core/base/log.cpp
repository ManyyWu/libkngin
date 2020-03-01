#include <cstdio>
#include <cstdarg>
#include <cerrno>
#include <cstring>
#include <ctime>
#include "core/base/log.h"
#include "core/base/logmgr.h"
#include "core/base/timestamp.h"

KNGIN_NAMESPACE_K_BEGIN

#ifdef _WIN32
#else
const char * const
log::log_color_begin_tbl[KNGIN_LOG_LEVEL_MAX + 1] = {
    KNGIN_LOG_COLOR_FATAL,
    KNGIN_LOG_COLOR_ERROR,
    KNGIN_LOG_COLOR_WARNING,
    KNGIN_LOG_COLOR_INFO,
    KNGIN_LOG_COLOR_DEBUG
};
#endif

log::log (KNGIN_LOG_FILE filetype, KNGIN_LOG_MODE mode /* = KNGIN_LOG_MODE_FILE */)
    :
#if (ON == KNGIN_ENABLE_LOG_MUTEX)
      mutex_(),
#endif
      mode_(mode),
      filetype_(filetype),
      disable_info_(false),
      disable_debug_(false)
{
}

void
log::fatal (const char *fmt, ...)
{
    assert(fmt);
    va_list vl;
    va_start(vl, fmt);
    write_log(KNGIN_LOG_LEVEL_FATAL, fmt, vl);
    va_end(vl);
    //assert(!"log fatal");
}

void
log::error (const char *fmt, ...)
{
    assert(fmt);
    va_list vl;
    va_start(vl, fmt);
    write_log(KNGIN_LOG_LEVEL_ERROR, fmt, vl);
    va_end(vl);
    //assert(!"log error");
}

void
log::warning (const char *fmt, ...)
{
    assert(fmt);
    va_list vl;
    va_start(vl, fmt);
    write_log(KNGIN_LOG_LEVEL_WARNING, fmt, vl);
    va_end(vl);
}

void
log::info (const char *fmt, ...)
{
    assert(fmt);
    if (disable_info_)
        return;

    va_list vl;
    va_start(vl, fmt);
    write_log(KNGIN_LOG_LEVEL_INFO, fmt, vl);
    va_end(vl);
}

void
log::debug (const char *fmt, ...)
{
    assert(fmt);
    if (disable_debug_)
        return;

    va_list vl;
    va_start(vl, fmt);
    write_log(KNGIN_LOG_LEVEL_DEBUG, fmt, vl);
    va_end(vl);
}

void
log::log_data (const std::string &str)
{
    if (str.empty())
        return;

#if (ON == KNGIN_ASYNC_LOGGER)
    logger().async_log(
        [filetype = filetype_, mode = mode_, str] ()
#else
        [filetype = filetype_, mode = mode_, &str] ()
#endif
    {
        if (KNGIN_LOG_MODE_BOTH == mode or KNGIN_LOG_MODE_FILE == mode)
            write_logfile(logger().filename_at(filetype).c_str(),
                          str.c_str(), str.size());
        if (KNGIN_LOG_MODE_BOTH == mode or KNGIN_LOG_MODE_STDERR == mode)
            write_stderr(KNGIN_LOG_LEVEL_INFO, str.c_str(), str.size());
        if (auto log_cb = logger().get_log_callback())
            log_cb(logger().filename_at(filetype).c_str(), KNGIN_LOG_LEVEL_INFO,
                    str.c_str(), str.size());
#if (ON == KNGIN_ASYNC_LOGGER)
    });
#else
    } ();
#endif
}

bool
log::log_assert (const char *func, const char *file,
                 size_t line, const char *exp)
{
    assert(func);
    assert(file);
    assert(exp);
    fatal(KNGIN_LOG_ASSERT_FORMAT, func, file, line, exp);
    return true;
}

void
log::write_log (KNGIN_LOG_LEVEL level, const char *fmt, va_list vl)
{
    assert(fmt);

    auto func = [&] () {
#if (ON == KNGIN_ASYNC_LOGGER)
        log_data_ptr data_ptr = k::make_shared_array<char>(KNGIN_LOG_BUF_SIZE);
        char *buf = data_ptr.get();
#else
        char  buf[KNGIN_LOG_BUF_SIZE];
#endif

#ifdef NDEBUG
        char datetime[KNGIN_LOG_DATETIME_LEN];
        ::strncpy(buf, get_datetime(datetime, sizeof(datetime)), sizeof(datetime));
        ::vsnprintf(buf + KNGIN_LOG_DATETIME_LEN - 1,
                    KNGIN_LOG_BUF_SIZE - KNGIN_LOG_DATETIME_LEN,
                    fmt, vl);
#else
        ::vsnprintf(buf, KNGIN_LOG_BUF_SIZE, fmt, vl);
#endif
        buf[KNGIN_LOG_BUF_SIZE - 1] = '\0';
        auto len = ::strnlen(buf, KNGIN_LOG_BUF_SIZE);

#if (ON == KNGIN_ASYNC_LOGGER)
        logger().async_log(
            [filetype = filetype_, level, mode = mode_, data_ptr, buf, len] ()
#else
        [filetype = filetype_, level, mode = mode_, buf, len] ()
#endif
        {
            if (KNGIN_LOG_MODE_BOTH == mode or KNGIN_LOG_MODE_FILE == mode)
                write_logfile(logger().filename_at(filetype).c_str(), buf, len);
            if (KNGIN_LOG_MODE_BOTH == mode or KNGIN_LOG_MODE_STDERR == mode)
                write_stderr(level, buf, len);
            if (auto log_cb = logger().get_log_callback())
                log_cb(logger().filename_at(filetype).c_str(), level,
                        buf, len);
#if (ON == KNGIN_ASYNC_LOGGER)
        });
#else
        } ();
#endif
    }; // end of write_log

#if (ON == KNGIN_ENABLE_LOG_MUTEX)
    if (logger().inited()) {
        local_lock lock(mutex_);
        func();
    } else
#endif
        func();
}

bool
log::write_logfile (const char *file, const char *str, size_t len) noexcept
{
    assert(file);
    assert(str);

    bool         fail = false;
    size_t       ret = 0;
    static char  buf[KNGIN_LOG_BUF_SIZE];
    static char  filename[FILENAME_MAX];
    tm           tm;
    time_t       t = ::time(nullptr);
    FILE *       fplog = nullptr;

    get_localtime(&tm, &t);
    ::snprintf(filename, FILENAME_MAX, KNGIN_LOG_FILENAME_FORMT, file,
               tm.tm_year + 1900, tm.tm_mon, tm.tm_mday);
    fplog = ::fopen(filename, "a");
    if (!fplog) {
        write_stderr2(KNGIN_LOG_LEVEL_FATAL,
                      "failed to open \"%s\", %s[%#x]",
                      filename, ::strerror(errno), errno);
        return false;
    }

    ::fseek(fplog, 0, SEEK_END);
    if (0 == ftell(fplog)) {
        // write head info
        char datetime[KNGIN_LOG_DATETIME_LEN];
        ::snprintf(buf, KNGIN_LOG_BUF_SIZE,
                   "=========================================================\n"
                   "= %19s                                   =\n"
                   "=========================================================\n",
                   get_datetime(datetime, sizeof(datetime)));
        auto str_len = ::strnlen(buf, KNGIN_LOG_BUF_SIZE);
        ret = ::fwrite(buf, 1, str_len , fplog);
        if (ret < 0) {
            write_stderr2(KNGIN_LOG_LEVEL_FATAL,
                          "failed to write log to \"%s\", %s[%#x]",
                          filename, ::strerror(errno), errno);
            goto fail;
        } else if (static_cast<size_t>(ret) != str_len) {
             write_stderr2(KNGIN_LOG_LEVEL_FATAL,
                           "the content been written to \"%s\" are too short, "
                           "and the disk space may be insufficient",
                           filename);
            goto fail;
        }
    }

    ret = ::fwrite(str, 1, len, fplog);
    if (ret < 0) {
        write_stderr2(KNGIN_LOG_LEVEL_FATAL,
                      "failed to write log to \"%s\", %s[%#x]",
                      filename, ::strerror(errno), errno);
        goto fail;
    }
    ::fputc('\n', fplog);
    ::fflush(fplog);

    fail = true;
fail:
    ::fclose(fplog);
    return fail;
}

const char *
log::get_datetime (char datetime[], size_t size) noexcept
{
    assert(size >= KNGIN_LOG_DATETIME_LEN);
    time_t t = ::time(nullptr);
    struct ::tm tm;
    get_localtime(&tm, &t);
    ::snprintf(datetime, KNGIN_LOG_DATETIME_LEN,
               KNGIN_LOG_DATETIME_FORMT,
               tm.tm_year + 1900, tm.tm_mon, tm.tm_mday,
               tm.tm_hour, tm.tm_min, tm.tm_sec);
    datetime[KNGIN_LOG_DATETIME_LEN - 1] = '\0';
    return datetime;
}

const char *
log::color_begin (KNGIN_LOG_LEVEL level) noexcept
{
#ifdef _WIN32
    return "";
#else
    return log::log_color_begin_tbl[level];
#endif
}

const char *
log::color_end () noexcept
{
#ifdef _WIN32
    return "";
#else
    return KNGIN_LOG_COLOR_NONE;
#endif
}

void
log::write_stderr (KNGIN_LOG_LEVEL level, const char *str, size_t len) noexcept
{
    assert(str);

    ::fputs(color_begin(level), stderr);
    ::fwrite(str, 1, len, stderr);
    ::fputs(color_end(), stderr);
    ::fputc('\n', stderr);
}

void
log::write_stderr2 (KNGIN_LOG_LEVEL level, const char *fmt, ...) noexcept
{
    assert(fmt);

    va_list vl;
    char buf[KNGIN_LOG_BUF_SIZE];

    va_start(vl, fmt);
    ::vsnprintf(buf, KNGIN_LOG_BUF_SIZE, fmt, vl);
    va_end(vl);

#ifdef _WIN32
#else
    ::fputs(color_begin(level), stderr);
#endif
    ::fwrite(buf, 1, ::strnlen(buf, KNGIN_LOG_BUF_SIZE), stderr);
#ifdef _WIN32
#else
    ::fputs(KNGIN_LOG_COLOR_NONE, stderr);
#endif
    ::fputc('\n', stderr);
}

KNGIN_NAMESPACE_K_END
