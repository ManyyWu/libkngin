#include "kngin/core/define.h"
#if defined(KNGIN_NOT_SYSTEM_WIN32)

#include "kngin/core/base/system_error.h"
#include "detail/core/base/descriptor.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

namespace k::detail {

size_t
descriptor::read (handle_t h, in_buffer &buf) {
  assert(buf.size() ? buf.writeable() : true);
  assert(HANDLE_VALID(h));
  auto size = ::read(h, buf.begin(), buf.writeable());
  if (size < 0)
    throw_system_error("::read() error", last_error());
  buf += size;
  return size;
}

size_t
descriptor::read (handle_t h, in_buffer &buf, error_code &ec) noexcept {
  assert(buf.size() ? buf.writeable() : true);
//  assert(HANDLE_VALID(h));
  auto size = ::read(h, buf.begin(), buf.writeable());
  if (size < 0) {
    ec = last_error();
    return 0;
  } else {
    ec = error_code();
  }
  buf += size;
  return size;
}

size_t
descriptor::write (handle_t h, out_buffer buf) {
  assert(buf.size() ? !buf.eof() : true);
  assert(HANDLE_VALID(h));
  auto size = ::write(h, buf.begin(), buf.size());
  if (size < 0)
    throw_system_error("::write() error", last_error());
  return size;
}

size_t
descriptor::write (handle_t h, out_buffer buf, error_code &ec) noexcept {
  assert(buf.size() ? !buf.eof() : true);
  assert(HANDLE_VALID(h));
  auto size = ::write(h, buf.begin(), buf.size());
  if (size < 0) {
    ec = last_error();
    return 0;
  } else {
    ec = error_code();
  }
  return size;
}

size_t
descriptor::readable (handle_t h) {
  assert(HANDLE_VALID(h));
  size_t bytes = 0;
  if (::ioctl(h, FIONREAD, &bytes) < 0)
    throw_system_error("::ioctl(FIONREAD) error", last_error());
  return bytes;
}

size_t
descriptor::readable (handle_t h, error_code &ec) noexcept {
  assert(HANDLE_VALID(h));
  size_t bytes;
  ec = (::ioctl(h, FIONREAD, &bytes) < 0) ? last_error() : error_code();
  return bytes;
}

error_code
descriptor::read_error (handle_t h) noexcept {
  assert(HANDLE_VALID(h));
  auto size = ::read(h, nullptr, 0);
  if (size < 0)
    return last_error();
  return error_code();
}

void
descriptor::close (handle_t &h) {
  assert(HANDLE_VALID(h));
  if (::close(h) < 0)
    throw_system_error("::close() error", last_error());
  h = INVALID_HANDLE;
}

void
descriptor::close (handle_t &h, error_code &ec) noexcept {
  assert(HANDLE_VALID(h));
  ec = (::close(h) < 0) ? last_error() : error_code();
  h = INVALID_HANDLE;
}

int
descriptor::dup (handle_t h) {
  assert(HANDLE_VALID(h));
  auto new_fd = ::dup(h);
  if (new_fd < 0)
    throw_system_error("::dup() error", last_error());
  return new_fd;
}

int
descriptor::dup (handle_t h, error_code &ec) noexcept {
  assert(HANDLE_VALID(h));
  auto new_fd = ::dup(h);
  if (new_fd < 0) {
    ec = last_error();
    return INVALID_HANDLE;
  }
  ec = error_code();
  return new_fd;
}

void
descriptor::set_nonblock (handle_t h, bool on) {
  assert(HANDLE_VALID(h));
  auto flags = ::fcntl(h, F_GETFL);
  flags = on ? flags | O_NONBLOCK : flags & ~O_NONBLOCK;
  if (::fcntl(h, F_SETFL, flags) < 0)
    throw_system_error("::fcntl() set O_NONBLOCK flag failed", last_error());
}

void
descriptor::set_nonblock (handle_t h, bool on, error_code &ec) noexcept {
  assert(HANDLE_VALID(h));
  auto flags = ::fcntl(h, F_GETFL);
  flags = on ? flags | O_NONBLOCK : flags & ~O_NONBLOCK;
  ec = (::fcntl(h, F_SETFL, flags) < 0) ? last_error() : error_code();
}

void
descriptor::set_closeexec (handle_t h, bool on) {
  assert(HANDLE_VALID(h));
  auto flags = ::fcntl(h, F_GETFD);
  flags = on ? flags | FD_CLOEXEC : flags & ~FD_CLOEXEC;
  if (::fcntl(h, F_SETFD, flags) < 0)
    throw_system_error("::fcntl() set FD_CLOEXEC flag failed", last_error());
}

void
descriptor::set_closeexec (handle_t h, bool on, error_code &ec) noexcept {
  assert(HANDLE_VALID(h));
  auto flags = ::fcntl(h, F_GETFD);
  flags = on ? flags | FD_CLOEXEC : flags & ~FD_CLOEXEC;
  ec = (::fcntl(h, F_SETFD, flags) < 0) ? last_error() : error_code();
}

bool
descriptor::nonblock (handle_t h) {
  assert(HANDLE_VALID(h));
  auto flags = ::fcntl(h, F_GETFL);
  if (flags < 0)
    throw_system_error("::fcntl() get O_NONBLOCK flag failed", last_error());
  return (flags & O_NONBLOCK);
}

bool
descriptor::nonblock (handle_t h, error_code &ec) noexcept {
  assert(HANDLE_VALID(h));
  auto flags = ::fcntl(h, F_GETFL);
  ec = (flags < 0) ? last_error() : error_code();
  return (flags & O_NONBLOCK);
}

bool
descriptor::closeexec (handle_t h) {
  assert(HANDLE_VALID(h));
  auto flags = ::fcntl(h, F_GETFD);
  if (flags < 0)
    throw_system_error("::fcntl() get FD_CLOEXEC flag failed", last_error());
  return (flags & FD_CLOEXEC);
}

bool
descriptor::closeexec (handle_t h, error_code &ec) noexcept {
  assert(HANDLE_VALID(h));
  auto flags = ::fcntl(h, F_GETFD);
  ec = (flags < 0) ? last_error() : error_code();
  return (flags & FD_CLOEXEC);
}

} /* namespace k::detail */

#endif /* defined(KNGIN_NOT_SYSTEM_WIN32) */
