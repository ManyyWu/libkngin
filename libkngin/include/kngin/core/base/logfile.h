#ifndef KNGIN_LOGFILE_H
#define KNGIN_LOGFILE_H

#include "kngin/core/base/callback.h"
#include <string>
#include <memory>

#define KNGIN_LOG_MODE_FILE     0x1
#define KNGIN_LOG_MODE_STDERR   0x2
#define KNGIN_LOG_MODE_CALLBACK 0x4
#define KNGIN_LOG_MODE_ALL      0x7

KNGIN_NAMESPACE_K_BEGIN

enum class KNGIN_LOG_LEVEL {
  KNGIN_LOG_LEVEL_FATAL = 0,
  KNGIN_LOG_LEVEL_ERROR,
  KNGIN_LOG_LEVEL_WARNING,
  KNGIN_LOG_LEVEL_INFO,
  KNGIN_LOG_LEVEL_DEBUG,
  KNGIN_LOG_LEVEL_MAX
};

class logfile
  : public std::enable_shared_from_this<logfile> {
  friend class logger;
private:
  logfile (const char *file, int mode, log_callback &&cb);

public:
  ~logfile () = default;

  void
  write_fatal (const char *fmt, ...);

  void
  write_error (const char *fmt, ...);

  void
  write_warning (const char *fmt, ...);

  void
  write_info (const char *fmt, ...);

  void
  write_debug (const char *fmt, ...);

  int
  set_mode (int mode) noexcept {
    return (mode_ &= mode & KNGIN_LOG_MODE_ALL);
  }

  int
  mode () const noexcept {
    return mode_;
  }

  void
  set_cb (log_callback &&cb) noexcept {
    cb_ = std::move(cb);
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

  const char *
  filename () const noexcept {
    return file_.c_str();
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