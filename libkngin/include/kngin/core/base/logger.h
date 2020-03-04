#ifndef KNGIN_LOGGER_H
#define KNGIN_LOGGER_H

#include "kngin/core/define.h"
#include <atomic>
#include <deque>

KNGIN_NAMESPACE_K_BEGIN

enum KNGIN_LOG_MODE {
  KNGIN_LOG_MODE_FILE = 0,
  KNGIN_LOG_MODE_STDERR,
  KNGIN_LOG_MODE_BOTH
};

enum KNGIN_LOG_LEVEL {
  KNGIN_LOG_LEVEL_FATAL = 0,
  KNGIN_LOG_LEVEL_ERROR,
  KNGIN_LOG_LEVEL_WARNING,
  KNGIN_LOG_LEVEL_INFO,
  KNGIN_LOG_LEVEL_DEBUG,
  KNGIN_LOG_LEVEL_MAX
};

#endif /* defined(KNGIN_SYSTEM_WIN32) */

class mutex;
class thread;
class logger : noncopyable {
  friend logger& log ();
private:
  logger ();

  ~logger ();

public:
  void
  fatal (const char *file, KNGIN_LOG_LEVEL level, const char *fmt, ...);

  void
  error (const char *file, KNGIN_LOG_LEVEL level, const char *fmt, ...);

  void
  warning (const char *file, KNGIN_LOG_LEVEL level, const char *fmt, ...);

  void
  info (const char *file, KNGIN_LOG_LEVEL level, const char *fmt, ...);

  void
  debug (const char *file, KNGIN_LOG_LEVEL level, const char *fmt, ...);

  static
  void
  set_mode (KNGIN_LOG_MODE mode) noexcept {
    logger::mode_ = mode;
  }

  static
  void
  disable_info () noexcept {
    logger::enable_info_ = false;
  }

  static
  void
  disable_debug () noexcept {
    logger::enable_debug_ = false;
  }

  static
  void
  enble_info () noexcept {
    logger::enable_info_ = true;
  }

  static
  void
  enble_debug () noexcept {
    logger::enable_debug_ = true;
  }

private:
  typedef size_t size_type;

  void
  init ();

  void
  post_data (std::string &data, size_type size);

private:
  static KNGIN_LOG_MODE mode_;

  static bool enable_info_;

  static bool enable_debug_;

#if defined(KNGIN_USE_ASYNC_LOGGER)
  struct async_log_data {
    std::string data;
  };

  typedef std::deque<async_log_data> async_log_dataq;

  static thread *write_thr;

  static mutex *mutex_;

  static cond *cond_;

  static std::atomic_bool stop_;

  static async_log_dataq log_dataq_;
#endif /* defined(KNGIN_USE_ASYNC_LOGGER) */
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_LOGGER_H */