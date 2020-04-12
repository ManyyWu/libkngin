#ifndef KNGIN_SYSTEM_ERROR_H
#define KNGIN_SYSTEM_ERROR_H

#include "kngin/core/base/error_code.h"
#include "kngin/core/base/string.h"
#include <exception>

#define throw_system_error(what, ec) throw k::system_error((what), (ec))

namespace k {

class system_error : public std::exception {
public:
  system_error () = delete;

  explicit
  system_error (const char *what, error_code ec)
   : what_(format_string("[k::system_error] %s - %s:%" PRId64,
                         what, ec.message().c_str(), ec.value())),
     ec_(ec) {
  }

  explicit
  system_error (const std::string &what, error_code ec)
   : what_(format_string("[k::system_error] %s:%" PRId64,
                         ec.message().c_str(), ec.value())),
     ec_(ec) {
  }

  system_error (const k::system_error &e)
   : what_(e.what_),
     ec_(e.ec_) {
  }

  virtual
  ~system_error () = default;

  k::system_error &
  operator = (const k::system_error &e) = delete;

  virtual
  const char *
  what () const noexcept {
    return what_.c_str();
  }

  virtual
  const error_code
  code () const noexcept {
    return ec_;
  }

private:
  std::string what_;

  error_code  ec_;
};

} /* namespace k */

#endif /* KNGIN_SYSTEM_ERROR_H */