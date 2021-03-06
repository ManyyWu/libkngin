#ifndef KNGIN_EXCEPTION_H
#define KNGIN_EXCEPTION_H

#include "kngin/core/define.h"
#include "kngin/core/base/noncopyable.h"
#include <string>
#include <exception>

#define throw_exception(what) throw k::exception((what))

namespace k {

class exception : public std::exception {
public:
  exception ()
   : what_(),
     dump_str_() {
    exception::dump_stack(dump_str_);
  }

  explicit
  exception (const char *what)
   : what_(std::string("[k::exception] ") + what),
     dump_str_() {
    exception::dump_stack(dump_str_);
  }

  explicit
  exception (const std::string &what)
   : what_(std::string("[k::exception] ") + what),
     dump_str_() {
    exception::dump_stack(dump_str_);
  }

  exception (const k::exception &e)
   : what_(e.what_),
     dump_str_(e.dump_str_) {
  }

  virtual
  ~exception () = default;

  virtual
  const char *
  what () const noexcept {
    return what_.c_str();
  }

  const char *
  dump () const noexcept {
    return dump_str_.c_str();
  }

  k::exception &
  operator = (const k::exception &) = delete;

protected:
  static
  void
  dump_stack (std::string &str);

private:
  const std::string what_;

  std::string dump_str_;
};

} /* namespace k */

#endif /* KNGIN_EXCEPTION_H */