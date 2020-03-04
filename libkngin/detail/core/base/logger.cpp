#include "kngin/core/base/logger.h"
#include <cstring>
#include <cstdarg>

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

KNGIN_NAMESPACE_K_BEGIN

// length of project path prefix
const size_t g_path_prefix_size = (::strlen(__FILE__) -
    ::strlen("libkngin/detail/core/base/logger.cpp"));

#if defined(NDEBUG)
bool logger::disable_debug = false;
#else
bool logger::enable_debug_ = true;
#endif /* defined(NDEBUG) */
bool logger::enable_info_ = false;

#if !defined(KNGIN_SYSTEM_WIN32)
const char * const log_color_prefix_entry[KNGIN_LOG_LEVEL_MAX] = {
  "\033[0m",
  "\033[01;32;41m",
  "\033[01;37;43m",
  "\033[01;31;49m",
  "\033[01;32;49m",
};
#endif /* !defined(KNGIN_SYSTEM_WIN32) */

logger:: logger () {

}

logger::~logger () {

}

void
fatal (const char *file, KNGIN_LOG_LEVEL level, const char *fmt, ...) {
  assert(fmt);

  va_list vl;
  va_start(vl, fmt);
  write_sog(KNGIN_LOG_LEVEL_FATAL, fmt, vl);
  va_end(vl);
  //assert(!"log fatal");
}

void
error (const char *file, KNGIN_LOG_LEVEL level, const char *fmt, ...) {

}

void
warning (const char *file, KNGIN_LOG_LEVEL level, const char *fmt, ...) {

}

void
info (const char *file, KNGIN_LOG_LEVEL level, const char *fmt, ...) {

}

void
debug (const char *file, KNGIN_LOG_LEVEL level, const char *fmt, ...) {

}

void
logger::init() {
#if defined(KNGIN_USE_ASYNC_LOGGER)
#endif /* defined(KNGIN_USE_ASYNC_LOGGER) */
}

KNGIN_NAMESPACE_K_END

