#ifndef KNGIN_DESCRIPTOR_H
#define KNGIN_DESCRIPTOR_H

#include "kngin/core/define.h"
#if defined(KNGIN_HAS_DESCRIPTOR)

#include "kngin/core/base/noncopyable.h"
#include "kngin/core/base/system_error.h"
#include "kngin/core/base/buffer.h"

#define FD_INVALID(fd) ((fd) < 0)
#define FD_VALID(fd)   ((fd) >= 0)
#define INVALID_FD     (-1)

KNGIN_NAMESPACE_K_DETAIL_BEGIN

namespace descriptor {

size_t
read (int fd, in_buffer &buf);

size_t
read (int fd, in_buffer &buf, error_code &ec) noexcept;

size_t
write (int fd, out_buffer &buf);

size_t
write (int fd, out_buffer &buf, error_code &ec) noexcept;

size_t
readable (int fd);

size_t
readable (int fd, error_code &ec) noexcept;

error_code
read_error (int fd) noexcept;

void
close (int &fd);

void
close (int &fd, error_code &ec) noexcept;

int
dup (int fd);

int
dup (int fd, error_code &ec) noexcept;

void
set_nonblock (int fd, bool on);

void
set_nonblock (int fd, bool on, error_code &ec) noexcept;

void
set_closeexec (int fd, bool on);

void
set_closeexec (int fd, bool on, error_code &ec) noexcept;

bool
nonblock (int fd);

bool
nonblock (int fd, error_code &ec) noexcept;

bool
closeexec (int fd);

bool
closeexec (int fd, error_code &ec) noexcept;

};

KNGIN_NAMESPACE_K_DETAIL_END

#endif /* !defined(KNGIN_SYSTEM_WIN32) */

#endif /* KNGIN_DESCRIPTOR_H */
