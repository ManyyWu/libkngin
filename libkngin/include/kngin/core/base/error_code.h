#ifndef KNGIN_ERROR_CODE_H
#define KNGIN_ERROR_CODE_H

#include "kngin/core/define.h"
#include "kngin/core/base/error.h"
#include <cerrno>
#include <string>
#include <cassert>

namespace k {

errno_type
last_error ();

class error_code {
public:
  error_code () noexcept
   : code_(KNGIN_ESUCCESS) {
  }
  error_code (errno_type code) noexcept
   : code_(code) {
    assert(code <= 0);
  }
  error_code (const error_code &code) noexcept
   : code_(code.code_) {
    assert(code_ <= 0);
  }
  ~error_code () = default;

  const char *
  message () const noexcept {
    return get_error_str(code_);
  }
  errno_type
  value () const noexcept {
    return code_;
  }
  explicit
  operator bool () const noexcept {
    return code_;
  }
  explicit
  operator errno_type () const noexcept {
    return code_;
  }

  error_code &
  operator = (errno_type code) noexcept {
    code_ = code;
    return *this;
  }
  error_code &
  operator = (const error_code &code) noexcept {
    code_ = code.code_;
    return *this;
  }

  bool
  operator == (const errno_type code) const noexcept {
    return (code_ == code);
  }
  bool
  operator == (const error_code &code) const noexcept {
    return (code_ == code.code_);
  }
  bool
  operator == (const error_code &&code) const noexcept {
    return (code_ == code.code_);
  }
  bool
  operator != (const errno_type code) const noexcept {
    return (code_ != code);
  }
  bool
  operator != (const error_code &code) const noexcept {
    return (code_ != code.code_);
  }
  bool
  operator != (const error_code &&code) const noexcept {
    return (code_ != code.code_);
  }

  friend
  bool
  operator == (errno_type code1, const error_code &code2) noexcept;
  friend
  bool
  operator == (errno_type code1, const error_code &&code2) noexcept;
  friend
  bool
  operator != (errno_type code1, const error_code &code2) noexcept;
  friend
  bool
  operator != (errno_type code1, const error_code &&code2) noexcept;

private:
  errno_type code_;
};

inline
bool
operator == (errno_type code1, const error_code &code2) noexcept {
  return (code1 == code2.code_);
}

inline
bool
operator == (errno_type code1, const error_code &&code2) noexcept {
  return (code1 == code2.code_);
}

inline
bool
operator != (errno_type code1, const error_code &code2) noexcept {
  return (code1 != code2.code_);
}

inline
bool
operator != (errno_type code1, const error_code &&code2) noexcept {
  return (code1 != code2.code_);
}

} /* namespace k */

#endif /* KNGIN_ERROR_CODE_H */
