#ifndef KNGIN_EXCEPTION_H
#define KNGIN_EXCEPTION_H

#include <string>
#include <exception>
#include "kngin/core/define.h"
#include "kngin/core/base/noncopyable.h"

KNGIN_NAMESPACE_K_BEGIN

class exception : public std::exception {
public:
  exception ()
    : what_(), dump_str_() {
    dump_stack();
  }

  explicit
  exception (const char *what)
    : what_(std::string("[k::exception] ") + what), dump_str_() {
    dump_stack();
  }

  explicit
  exception (const std::string &what)
    : what_(std::string("[k::exception] ") + what), dump_str_() {
    dump_stack();
  }

  exception (const k::exception &e)
    : what_(e.what_), dump_str_(e.dump_str_) {
  }

  virtual
  ~exception () = default;

  virtual const char *
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
  void
  dump_stack ();

private:
  const std::string what_;

  std::string dump_str_;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_EXCEPTION_H */