#ifndef KNGIN_HANDLE_H
#define KNGIN_HANDLE_H

#include "kngin/core/define.h"
#if !defined(KNGIN_SYSTEM_WIN32)

#include "kngin/core/base/noncopyable.h"
#include "kngin/core/base/system_error.h"
#include "kngin/core/base/buffer.h"
#include "detail/core/base/win_utils.h"

KNGIN_NAMESPACE_K_DETAIL_BEGIN

class descriptor {
public:
  static
  size_t
  read (HANDLE handle, in_buffer &buf);

  static
  size_t
  read (HANDLE handle, in_buffer &buf, error_code &ec) noexcept;

  static
  size_t
  write (HANDLE handle, out_buffer &buf);

  static
  size_t
  write (HANDLE handle, out_buffer &buf, error_code &ec) noexcept;

  static
  size_t
  readn (HANDLE handle, in_buffer &buf);

  static
  size_t
  readn (HANDLE handle, in_buffer &buf, error_code &ec) noexcept;

  static
  size_t
  writen (HANDLE handle, out_buffer &buf);

  static
  size_t
  writen (HANDLE handle, out_buffer &buf, error_code &ec) noexcept;

  static
  size_t
  readable (HANDLE handle);

  static
  size_t
  readable (HANDLE handle, error_code &ec) noexcept;

  static
  error_code
  read_error (HANDLE handle) noexcept;

  static
  void
  close (int &fd);

  static
  void
  close (int &fd, error_code &ec) noexcept;

  static
  void
  set_nonblock (HANDLE handle, bool on);

  static
  void
  set_nonblock (HANDLE handle, bool on, error_code &ec) noexcept;

  static
  void
  set_closeexec (HANDLE handle, bool on);

  static
  void
  set_closeexec (HANDLE handle, bool on, error_code &ec) noexcept;

  static
  bool
  nonblock (HANDLE handle);

  static
  bool
  nonblock (HANDLE handle, error_code &ec) noexcept;
};

KNGIN_NAMESPACE_K_DETAIL_END

#endif /* !defined(KNGIN_SYSTEM_WIN32) */

#endif /* KNGIN_HANDLE_H */
