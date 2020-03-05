#include "kngin/core/base/logger.h"
#include "kngin/core/base/string.h"
#include <cstring>
#include <cstdarg>

#define KNGIN_LOG_BUF_SIZE       4096
#define KNGIN_LOG_FILE_MAX_SIZE  20 * 1024 * 1024 // 20M

// "YYYY/MM/DD hh:mm:ss"
#define KNGIN_LOG_DATETIME_FORMT "%04d/%02d/%02d %02d:%02d:%02d"
#define KNGIN_LOG_DATETIME_LEN   20

// "YYYY/MM/DD hh:mm:ss.ms | type | func[file:line] | fmt \n"
#define KNGIN_LOG_FORMAT(type, fmt) "%s | " type " | %s[%s:%d] | " fmt

// "YYYY/MM/DD hh:mm:ss.ms | type | fmt \n"
#define KNGIN_LOG_FORMAT_NOLINE(type, fmt) "%s | " type " | " fmt

// "name_YYYY-MM-dd.log"
#define KNGIN_LOG_FILENAME_FORMT "%s_%04d-%02d-%02d.log"

// "****** func[file:line] ******"
#define KNGIN_LOG_ASSERT_FORMAT "%s | ASSERT  | %s[%s:%d] | ****** %s *******"

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
static
logger &
get_logger () {
  static logger logger;
  return logger;
}
logger &log = get_logger();
extern logger &log;

#if defined(KNGIN_SYSTEM_WIN32)
# define color_prefix(level) ""
# define color_prefix_len(level) 0
#else
struct color_prefix {
  const char *str;
  int len;
};

static color_prefix log_color_prefix_entry[KNGIN_LOG_LEVEL_MAX] = {
  { KNGIN_LOG_COLOR_FATAL, 7},
  { KNGIN_LOG_COLOR_ERROR, 14},
  { KNGIN_LOG_COLOR_WARNING, 14},
  { KNGIN_LOG_COLOR_INFO, 14},
  { KNGIN_LOG_COLOR_DEBUG, 0},
};
# define color_prefix_len(level) (log_color_prefix_entry[level].str)
# define color_prefix_len(level) (log_color_prefix_entry[level].len)
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

void
logger::format_log (KNGIN_LOG_LEVEL level, std::string &result,
                    const char *fmt, va_list vl) {
  size_t offset = KNGIN_LOG_DATETIME_LEN - 1 + color_prefix_len(level);
  format_string(result, fmt, vl, offset);

  char datetime[KNGIN_LOG_DATETIME_LEN];
  ::memcpy(result.data(), logger::get_datetime(datetime, KNGIN_LOG_DATETIME_LEN),
           KNGIN_LOG_DATETIME_LEN);
  ::memcpy(result.data() + KNGIN_LOG_DATETIME_LEN - 1, color_prefix(level),
           color_prefix_len(level));
}

void
logger::write_log (KNGIN_LOG_LEVEL level, log_file &file,
                   std::string &&data, size_type size) {

}

bool
logger::write_logfile (const char *file, KNGIN_LOG_LEVEL level,
                       const char *data, size_t len) noexcept {

}

void
logger::write_stderr (KNGIN_LOG_LEVEL level, const char *data, size_t len) noexcept {

}

void
logger::write_stderr2 (KNGIN_LOG_LEVEL level, const char *data, ...) noexcept {

}

const char *
logger::get_datetime (char datetime[], size_t size) noexcept {
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
logger::color_begin (KNGIN_LOG_LEVEL level) noexcept {
#if defined(KNGIN_SYSTEM_WIN32)
#else
  return log::log_color_begin_tbl[_level];
#endif /* defined(KNGIN_SYSTEM_WIN32) */
}

const char *
logger::color_end () noexcept {
#if defined(KNGIN_SYSTEM_WIN32)
#else
  return KNGIN_LOG_COLOR_NONE;
#endif /* defined(KNGIN_SYSTEM_WIN32) */
}

KNGIN_NAMESPACE_K_END

