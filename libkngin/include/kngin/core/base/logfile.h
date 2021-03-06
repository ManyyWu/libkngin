#ifndef KNGIN_LOGFILE_H
#define KNGIN_LOGFILE_H

#include "kngin/core/base/callback.h"
#include <string>
#include <memory>

enum {
  log_mode_file     = 0x1,
  log_mode_stderr   = 0x2,
  log_mode_callback = 0x4,
  log_mode_all      = 0x7,
};

namespace k {

enum class log_level {
  log_level_fatal = 0,
  log_level_error,
  log_level_warning,
  log_level_info,
  log_level_debug,
  log_level_max
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
    return (mode_ &= mode & log_mode_all);
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

} /* namespace k */

#endif /* KNGIN_LOGFILE_H */