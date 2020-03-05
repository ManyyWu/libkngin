#ifndef KNGIN_LOGGER_H
#define KNGIN_LOGGER_H

#include "kngin/core/define.h"
#include "kngin/core/base/logfile.h"
#include <atomic>
#include <deque>

KNGIN_NAMESPACE_K_BEGIN

class mutex;
class thread;
class logger : noncopyable {
  friend logger& get_logger ();
private:
  typedef size_t size_type;

  logger ();

  ~logger () noexcept;

  static
  void
  format_log (KNGIN_LOG_LEVEL level, std::string &result,
              const char *fmt, va_list vl);

  void
  write_log (KNGIN_LOG_LEVEL level, log_file &file,
             std::string &&data, size_type size);

private:
  void
  init ();

  static
  bool
  write_logfile (const char *file, KNGIN_LOG_LEVEL level,
                 const char *data, size_t len) noexcept;

  static
  void
  write_stderr (KNGIN_LOG_LEVEL level, const char *data, size_t len) noexcept;

  static
  void
  write_stderr2 (KNGIN_LOG_LEVEL level, const char *data, ...) noexcept;

  static
  const char *
  get_datetime (char datetime[], size_t size) noexcept;

  static
  const char *
  color_begin (KNGIN_LOG_LEVEL level) noexcept;

  static
  const char *
  color_end () noexcept;

private:
#if defined(KNGIN_USE_ASYNC_LOGGER)
  struct async_log_data {
    log_file &file;
    std::string data;
    size_type size;
  };

  typedef std::deque<async_log_data> async_log_dataq;

  static thread *write_thr_;

  static mutex *mutex_;

  static cond *cond_;

  static std::atomic_bool stop_;

  static async_log_dataq log_dataq_;
#endif /* defined(KNGIN_USE_ASYNC_LOGGER) */
};

extern logger &log;

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_LOGGER_H */