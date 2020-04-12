#include "kngin/core/base/logfile.h"
#include "kngin/core/base/logger.h"
#include <cstdarg>
#include <cassert>

namespace k {

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
  logger::format_log(log_level::log_level_fatal, data, fmt, vl);
  g_logger.post_log(log_level::log_level_fatal, *this, std::move(data), data.size() - 1);
  va_end(vl);
}

void
logfile::write_error (const char *fmt, ...) {
  assert(fmt);

  va_list vl;
  va_start(vl, fmt);
  std::string data;
  logger::format_log(log_level::log_level_error, data, fmt, vl);
  g_logger.post_log(log_level::log_level_error, *this, std::move(data), data.size() - 1);
  va_end(vl);
}

void
logfile::write_warning (const char *fmt, ...) {
  assert(fmt);

  va_list vl;
  va_start(vl, fmt);
  std::string data;
  logger::format_log(log_level::log_level_warning, data, fmt, vl);
  g_logger.post_log(log_level::log_level_warning, *this, std::move(data), data.size() - 1);
  va_end(vl);
}

void
logfile::write_info (const char *fmt, ...) {
  assert(fmt);

  va_list vl;
  va_start(vl, fmt);
  std::string data;
  logger::format_log(log_level::log_level_info, data, fmt, vl);
  g_logger.post_log(log_level::log_level_info, *this, std::move(data), data.size() - 1);
  va_end(vl);
}

void
logfile::write_debug (const char *fmt, ...) {
  assert(fmt);

  va_list vl;
  va_start(vl, fmt);
  std::string data;
  logger::format_log(log_level::log_level_debug, data, fmt, vl);
  g_logger.post_log(log_level::log_level_debug, *this, std::move(data), data.size() - 1);
  va_end(vl);
}

} /* namespace k */
