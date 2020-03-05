#include "kngin/core/base/logfile.h"
#include "kngin/core/base/logger.h"
#include <cstdarg>
#include <cassert>

KNGIN_NAMESPACE_K_BEGIN

void
log_file::fatal (const char *fmt, ...) {
  assert(fmt);

  va_list vl;
  va_start(vl, fmt);
  std::string data;
  logger::format_log(data, fmt, vl);
  log.write_log(KNGIN_LOG_LEVEL_FATAL, *this, std::move(data), data.size());
  va_end(vl);
}

void
log_file::error (const char *fmt, ...) {
  assert(fmt);

  va_list vl;
  va_start(vl, fmt);
  std::string data;
  logger::format_log(data, fmt, vl);
  log.write_log(KNGIN_LOG_LEVEL_ERROR, *this, std::move(data), data.size());
  va_end(vl);
}

void
log_file::warning (const char *fmt, ...) {
  assert(fmt);

  va_list vl;
  va_start(vl, fmt);
  std::string data;
  logger::format_log(data, fmt, vl);
  log.write_log(KNGIN_LOG_LEVEL_WARNING, *this, std::move(data), data.size());
  va_end(vl);
}

void
log_file::info (const char *fmt, ...) {
  assert(fmt);

  va_list vl;
  va_start(vl, fmt);
  std::string data;
  logger::format_log(data, fmt, vl);
  log.write_log(KNGIN_LOG_LEVEL_INFO, *this, std::move(data), data.size());
  va_end(vl);
}

void
log_file::debug (const char *fmt, ...) {
  assert(fmt);

  va_list vl;
  va_start(vl, fmt);
  std::string data;
  logger::format_log(data, fmt, vl);
  log.write_log(KNGIN_LOG_LEVEL_DEBUG, *this, std::move(data), data.size());
  va_end(vl);
}

KNGIN_NAMESPACE_K_END
