#ifndef KNGIN_LOGFILE_H
#define KNGIN_LOGFILE_H

#include "kngin/core/define.h"
#include "kngin/core/base/callback.h"
#include <string>

#define KNGIN_LOG_MODE_FILE     0x1
#define KNGIN_LOG_MODE_STDERR   0x2
#define KNGIN_LOG_MODE_CALLBACK 0x4
#define KNGIN_LOG_MODE_ALL      0x7

KNGIN_NAMESPACE_K_BEGIN

enum KNGIN_LOG_LEVEL {
  KNGIN_LOG_LEVEL_FATAL = 0,
  KNGIN_LOG_LEVEL_ERROR,
  KNGIN_LOG_LEVEL_WARNING,
  KNGIN_LOG_LEVEL_INFO,
  KNGIN_LOG_LEVEL_DEBUG,
  KNGIN_LOG_LEVEL_MAX
};

class log_file {
  friend class logger;
public:
  log_file (const char *file, int mode, log_callback &&cb)
    : file_(file),
      mode_(mode),
      cb_(std::move(cb)),
      enable_info_(true),
      enable_debug_(true) {
    }

  void
  fatal (const char *fmt, ...);

  void
  error (const char *fmt, ...);

  void
  warning (const char *fmt, ...);

  void
  info (const char *fmt, ...);

  void
  debug (const char *fmt, ...);

  int
  set_mode (int mode) noexcept {
    mode_ &= mode & KNGIN_LOG_MODE_ALL;
  }

  void
  set_cb (log_callback cb) noexcept {
    cb_ = cb;
  }

  void
  disable_info () noexcept {
    enable_info_ = false;
  }

  void
  disable_debug () noexcept {
    enable_debug_ = false;
  }

  void
  enble_info () noexcept {
    enable_info_ = true;
  }

  void
  enble_debug () noexcept {
    enable_debug_ = true;
  }

private:
  std::string file_;

  int mode_;

  log_callback cb_;

  bool enable_info_;

  bool enable_debug_;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_LOGFILE_H */