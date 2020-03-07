#include "kngin/core/base/logfile.h"
#include "kngin/core/base/logger.h"
#include <cstdarg>
#include <cassert>

KNGIN_NAMESPACE_K_BEGIN

extern logger &g_logger;

logfile::logfile (const char *file, int mode, log_callback &&cb)
 : file_(file),
   mode_(mode),
   cb_(std::move(cb)),
   enable_info_(true),
   enable_debug_(true) {
}

void
logfile::write_fatal (const char *fmt, ...) {
  assert(fmt);

  va_list vl;
  va_start(vl, fmt);
  std::string data;
  logger::format_log(KNGIN_LOG_LEVEL::KNGIN_LOG_LEVEL_FATAL, data, fmt, vl);
  g_logger.post_log(KNGIN_LOG_LEVEL::KNGIN_LOG_LEVEL_FATAL, *this, std::move(data), data.size() - 1);
  va_end(vl);
}

void
logfile::write_error (const char *fmt, ...) {
  assert(fmt);

  va_list vl;
  va_start(vl, fmt);
  std::string data;
  logger::format_log(KNGIN_LOG_LEVEL::KNGIN_LOG_LEVEL_ERROR, data, fmt, vl);
  g_logger.post_log(KNGIN_LOG_LEVEL::KNGIN_LOG_LEVEL_ERROR, *this, std::move(data), data.size() - 1);
  va_end(vl);
}

void
logfile::write_warning (const char *fmt, ...) {
  assert(fmt);

  va_list vl;
  va_start(vl, fmt);
  std::string data;
  logger::format_log(KNGIN_LOG_LEVEL::KNGIN_LOG_LEVEL_WARNING, data, fmt, vl);
  g_logger.post_log(KNGIN_LOG_LEVEL::KNGIN_LOG_LEVEL_WARNING, *this, std::move(data), data.size() - 1);
  va_end(vl);
}

void
logfile::write_info (const char *fmt, ...) {
  assert(fmt);

  va_list vl;
  va_start(vl, fmt);
  std::string data;
  logger::format_log(KNGIN_LOG_LEVEL::KNGIN_LOG_LEVEL_INFO, data, fmt, vl);
  g_logger.post_log(KNGIN_LOG_LEVEL::KNGIN_LOG_LEVEL_INFO, *this, std::move(data), data.size() - 1);
  va_end(vl);
}

void
logfile::write_debug (const char *fmt, ...) {
  assert(fmt);

  va_list vl;
  va_start(vl, fmt);
  std::string data;
  logger::format_log(KNGIN_LOG_LEVEL::KNGIN_LOG_LEVEL_DEBUG, data, fmt, vl);
  g_logger.post_log(KNGIN_LOG_LEVEL::KNGIN_LOG_LEVEL_DEBUG, *this, std::move(data), data.size() - 1);
  va_end(vl);
}

KNGIN_NAMESPACE_K_END
