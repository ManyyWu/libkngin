#ifndef KNGIN_ERROR_CODE_H
#define KNGIN_ERROR_CODE_H

#include "kngin/core/define.h"
#include <cerrno>
#include <string>
#include <cassert>

namespace k {

#if defined(KNGIN_SYSTEM_WIN32)
typedef int64_t error_type;
#define ERRNO(code) (assert((code) > 0), -(code))
#else
typedef int error_type;
#define ERRNO(code) (code)
#endif /* defined(KNGIN_SYSTEM_WIN32) */

error_type
last_error ();

void
set_last_error (error_type ec);

class error_code {
public:
  error_code () noexcept
   : code_(0) {
  }
  error_code (error_type code) noexcept
   : code_(code) {
  }
  error_code (const error_code &code) noexcept
   : code_(code.code_) {
  }
  error_code (const error_code &&code) noexcept
   : code_(code.code_) {
  }

  ~error_code () = default;

  std::string
  message () const {
    return get_error_str(code_);
  }

  error_type
  value () const noexcept {
    return code_;
  }

  bool
  is_errno () {
    return (code_ < 0);
  }

  explicit
  operator bool () const noexcept {
    return code_;
  }
  explicit
  operator error_type () const noexcept {
    return code_;
  }

  error_code &
  operator = (error_type code) noexcept {
    code_ = code;
    return *this;
  }
  error_code &
  operator = (const error_code &code) noexcept {
    code_ = code.code_;
    return *this;
  }
  error_code &
  operator = (const error_code &&code) noexcept {
    code_ = code.code_;
    return *this;
  }

  bool
  operator == (const error_type code) const noexcept {
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
  operator != (const error_type code) const noexcept {
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
  operator == (int code1, const error_code &code2) noexcept;
  friend
  bool
  operator == (int code1, const error_code &&code2) noexcept;
  friend
  bool
  operator != (int code1, const error_code &code2) noexcept;
  friend
  bool
  operator != (int code1, const error_code &&code2) noexcept;

private:
  static
  std::string
  get_error_str (error_type code);

public:
  static const error_code eof;

private:
  error_type code_;
};

inline
bool
operator == (int code1, const error_code &code2) noexcept {
  return (code1 == code2.code_);
}

inline
bool
operator == (int code1, const error_code &&code2) noexcept {
  return (code1 == code2.code_);
}

inline
bool
operator != (int code1, const error_code &code2) noexcept {
  return (code1 != code2.code_);
}

inline
bool
operator != (int code1, const error_code &&code2) noexcept {
  return (code1 != code2.code_);
}

} /* namespace k */

#endif /* KNGIN_ERROR_CODE_H */
