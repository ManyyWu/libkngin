#ifndef KNGIN_DESCRIPTOR_H
#define KNGIN_DESCRIPTOR_H

#include "kngin/core/define.h"
#if !defined(KNGIN_SYSTEM_WIN32)

#include "kngin/core/base/noncopyable.h"
#include "kngin/core/base/system_error.h"
#include "kngin/core/base/buffer.h"

#define FD_INVALID(fd) ((fd) < 0)
#define FD_VALID(fd)   ((fd) >= 0)
#define INVALID_FD     (-1)

KNGIN_NAMESPACE_K_DETAIL_BEGIN

class descriptor {
public:
  static
  size_t
  read (int fd, in_buffer &buf);

  static
  size_t
  read (int fd, in_buffer &buf, error_code &ec) noexcept;

  static
  size_t
  write (int fd, out_buffer &buf);

  static
  size_t
  write (int fd, out_buffer &buf, error_code &ec) noexcept;

  static
  size_t
  readn (int fd, in_buffer &buf);

  static
  size_t
  readn (int fd, in_buffer &buf, error_code &ec) noexcept;

  static
  size_t
  writen (int fd, out_buffer &buf);

  static
  size_t
  writen (int fd, out_buffer &buf, error_code &ec) noexcept;

  static
  size_t
  readable (int fd);

  static
  size_t
  readable (int fd, error_code &ec) noexcept;

  static
  error_code
  read_error (int fd) noexcept;

  static
  void
  close (int fd);

  static
  void
  close (int fd, error_code &ec) noexcept;

  static
  int
  dup (int fd);

  static
  int
  dup (int fd, error_code &ec) noexcept;

  static
  void
  set_nonblock (int fd, bool on);

  static
  void
  set_nonblock (int fd, bool on, error_code &ec) noexcept;

  static
  void
  set_closeexec (int fd, bool on);

  static
  void
  set_closeexec (int fd, bool on, error_code &ec) noexcept;

  static
  bool
  nonblock (int fd);

  static
  bool
  nonblock (int fd, error_code &ec) noexcept;
};

KNGIN_NAMESPACE_K_DETAIL_END

#endif /* !defined(KNGIN_SYSTEM_WIN32) */

#endif /* KNGIN_DESCRIPTOR_H */
