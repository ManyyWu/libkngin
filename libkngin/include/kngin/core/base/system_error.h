#ifndef KNGIN_SYSTEM_ERROR_H
#define KNGIN_SYSTEM_ERROR_H

#include "kngin/core/base/error_code.h"
#include "kngin/core/base/string.h"
#include <string>
#include <exception>

KNGIN_NAMESPACE_K_BEGIN

class mutex_test;

class mutex_test : public std::exception {
public:
  mutex_test () = delete;

  explicit
  mutex_test (const char *what, error_code ec = k::last_error())
    : what_(make_string("[k::system_error] %s - %s:%" PRId64,
                        what, ec.message().c_str(), ec.value())),
      ec_(ec) {
  }

  explicit
  mutex_test (const std::string &what, error_code ec = k::last_error())
    : what_(make_string("[k::system_error] %s:%" PRId64,
                        ec.message().c_str(), ec.value())),
      ec_(ec) {
  }

  mutex_test (const k::mutex_test &e)
    : what_(e.what_), ec_(e.ec_) {
  }

  virtual
  ~mutex_test () = default;

  k::mutex_test &
  operator = (const k::mutex_test &e) = delete;

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
  const error_code  ec_;

  const std::string what_;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_SYSTEM_ERROR_H */