#include "kngin/core/define.h"
#if !defined(KNGIN_SYSTEM_WIN32)

#include "kngin/core/base/noncopyable.h"
#include "kngin/core/base/system_error.h"
#include "kngin/core/base/buffer.h"

#define FD_INVALID(fd) ((fd) < 0)
#define FD_VALID(fd)   ((fd) >= 0)
#define INVALID_FD     (-1)

KNGIN_NAMESPACE_K_DETAIL_BEGIN

typedef int fd_type;

size_t
read (fd_type fd, in_buffer &buf);

size_t
read (fd_type fd, in_buffer &buf, error_code &ec) noexcept;

size_t
write (fd_type fd, out_buffer &buf);

size_t
write (fd_type fd, out_buffer &buf, error_code &ec) noexcept;

size_t
readn (fd_type fd, in_buffer &buf);

size_t
readn (fd_type fd, in_buffer &buf, error_code &ec) noexcept;

size_t
writen (fd_type fd, out_buffer &buf);

size_t
writen (fd_type fd, out_buffer &buf, error_code &ec) noexcept;

size_t
readable (fd_type fd);

size_t
readable (error_code &ec) noexcept;

error_code
read_error (fd_type fd) noexcept;

void
close (fd_type fd);

void
close (fd_type fd, error_code &ec) noexcept;

fd_type
dup (fd_type fd);

fd_type
dup (fd_type fd, error_code &ec) noexcept;

void
set_nonblock (fd_type fd, bool on);

void
set_nonblock (fd_type fd, bool on, error_code &ec) noexcept;

void
set_closeexec (fd_type fd, bool on);

void
set_closeexec (fd_type fd, bool on, error_code &ec) noexcept;

bool
nonblock (fd_type fd);

bool
nonblock (fd_type fd, error_code &ec) noexcept;

KNGIN_NAMESPACE_K_DETAIL_END

#endif /* !defined(KNGIN_SYSTEM_WIN32) */
