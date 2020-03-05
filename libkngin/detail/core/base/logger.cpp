#include "kngin/core/base/logger.h"
#include "kngin/core/base/string.h"
#include <cstring>
#include <cstdarg>

#define KNGIN_LOG_BUF_SIZE       4096
#define KNGIN_LOG_FILE_MAX_SIZE  20 * 1024 * 1024 // 20M

// "YYYY/MM/DD hh:mm:ss"
#define KNGIN_LOG_DATETIME_FORMT "%04d/%02d/%02d %02d:%02d:%02d"
#define KNGIN_LOG_DATETIME_LEN   20

// "name_YYYY-MM-dd.log"
#define KNGIN_LOG_FILENAME_FORMT "%s_%04d-%02d-%02d.log"

// color
#if !defined(KNGIN_SYSTEM_WIN32)
#define KNGIN_LOG_COLOR_NONE    "\033[0m"
#define KNGIN_LOG_COLOR_FATAL   "\033[01;32;41m"
#define KNGIN_LOG_COLOR_ERROR   "\033[01;37;43m"
#define KNGIN_LOG_COLOR_WARNING "\033[01;31;49m"
#define KNGIN_LOG_COLOR_INFO    "\033[01;32;49m"
#define KNGIN_LOG_COLOR_DEBUG   ""
#endif /* !defined(KNGIN_SYSTEM_WIN32) */

KNGIN_NAMESPACE_K_BEGIN

// length of project path prefix
const size_t g_path_prefix_size = (::strlen(__FILE__) -
    ::strlen("libkngin/detail/core/base/logger.cpp"));

// global logger
logger &
get_logger () {
  static logger logger;
  return logger;
}
logger &log = get_logger();
extern logger &log;

#if defined(KNGIN_SYSTEM_WIN32)
# define color_prefix_str(level) ""
# define color_suffix_str()      ""
#else
static const char * const log_color_prefix_entry[
    static_cast<int>(KNGIN_LOG_LEVEL::KNGIN_LOG_LEVEL_MAX)] = {
  KNGIN_LOG_COLOR_FATAL,
  KNGIN_LOG_COLOR_ERROR,
  KNGIN_LOG_COLOR_WARNING,
  KNGIN_LOG_COLOR_INFO,
  KNGIN_LOG_COLOR_DEBUG,
};
# define color_prefix_str(level) log_color_prefix_entry[static_cast<int>(level)]
# define color_suffix_str()      KNGIN_LOG_COLOR_NONE
#endif /* defined(KNGIN_SYSTEM_WIN32) */

logger:: logger () {

}

logger::~logger () noexcept {

}

void
logger::init() {
#if defined(KNGIN_USE_ASYNC_LOGGER)
#endif /* defined(KNGIN_USE_ASYNC_LOGGER) */
}

const char *
logger::get_datetime (char datetime[], size_type size) noexcept {
  assert(size >= KNGIN_LOG_DATETIME_LEN);
  time_t t = ::time(nullptr);
  struct ::tm tm;
  get_localtime(&tm, &t);
  ::snprintf(datetime, KNGIN_LOG_DATETIME_LEN,
             KNGIN_LOG_DATETIME_FORMT,
             tm.tm_year + 1900, tm.tm_mon, tm.tm_mday,
             tm.tm_hour, tm.tm_min, tm.tm_sec);
  return datetime;
}

void
logger::format_log (KNGIN_LOG_LEVEL level, std::string &result,
                    const char *fmt, va_list vl) {
  format_string(result, fmt, vl, KNGIN_LOG_DATETIME_LEN - 1, 0);
  char datetime[KNGIN_LOG_DATETIME_LEN];
  logger::get_datetime(datetime, KNGIN_LOG_DATETIME_LEN);
  ::memcpy(result.data(), datetime, KNGIN_LOG_DATETIME_LEN - 1);
}

void
logger::write_log (KNGIN_LOG_LEVEL level, log_file &file,
                   std::string &&data, size_type size) {
#if defined(KNGIN_USE_ASYNC_LOGGER)
#else
  if (file.mode_ & KNGIN_LOG_MODE_FILE)
    write_logfile(file.file_.c_str(), level, data.c_str(), size);
  if (file.mode_ & KNGIN_LOG_MODE_STDERR)
    write_stderr(level, data.c_str(), size);
  if (file.mode_ & KNGIN_LOG_MODE_CALLBACK and file.cb_)
    file.cb_(file.file_.c_str(), level, data.c_str(), size);
#endif /* defined(KNGIN_USE_ASYNC_LOGGER) */
}

void
logger::write_logfile (const char *file, KNGIN_LOG_LEVEL level,
                       const char *data, size_type len) noexcept {
  assert(file);
  assert(data);
  assert(len);

  static char  filename[FILENAME_MAX];
  size_type    ret = 0;
  tm           tm;
  time_t       t = ::time(nullptr);
  FILE *       fplog = nullptr;

  get_localtime(&tm, &t);
  ::snprintf(filename, FILENAME_MAX, KNGIN_LOG_FILENAME_FORMT, file,
             tm.tm_year + 1900, tm.tm_mon, tm.tm_mday);
  fplog = ::fopen(filename, "a");
  if (!fplog) {
    write_stderr2(KNGIN_LOG_LEVEL::KNGIN_LOG_LEVEL_FATAL,
                  "failed to open \"%s\", %s[%#x]",
                  filename, ::strerror(errno), errno);
    return;
  }

  ret = ::fwrite(data, 1, len, fplog);
  if (ret < 0) {
    write_stderr2(KNGIN_LOG_LEVEL::KNGIN_LOG_LEVEL_FATAL,
                  "failed to write log to \"%s\", %s[%#x]",
                  filename, ::strerror(errno), errno);
    goto fail;
  }
  ::fflush(fplog);
fail:
  ::fclose(fplog);

}

void
logger::write_stderr (KNGIN_LOG_LEVEL level, const char *data, size_type len) noexcept {
  assert(data);
  assert(len);

#if defined(KNGIN_SYSTEM_WIN32)
  ::fwrite(data, 1, len, stderr);
#else
  ::fputs(color_prefix_str(level), stderr);
  ::fwrite(data, 1, len, stderr);
  ::fputs(color_suffix_str(), stderr);
#endif /* defined(KNGIN_SYSTEM_WIN32) */
}

void
logger::write_stderr2 (KNGIN_LOG_LEVEL level, const char *fmt, ...) noexcept {
  assert(fmt);
  va_list vl;
  va_start(vl, fmt);

#if defined(KNGIN_SYSTEM_WIN32)
  ::vfprintf(stderr, KNGIN_LOG_BUF_SIZE, fmt, vl);
#else
  ::fputs(color_prefix_str(level), stderr);
  ::vfprintf(stderr, fmt, vl);
  ::fputs(color_suffix_str(), stderr);
#endif /* defined(KNGIN_SYSTEM_WIN32) */
  va_end(vl);
}

KNGIN_NAMESPACE_K_END

