#ifndef KNGIN_DESCRIPTOR_H
#define KNGIN_DESCRIPTOR_H

#include "kngin/core/define.h"
#if defined(KNGIN_HAS_DESCRIPTOR)

#include "kngin/core/base/noncopyable.h"
#include "kngin/core/base/system_error.h"
#include "kngin/core/base/buffer.h"

namespace k::detail {

namespace descriptor {

size_t
read (handle_t h, in_buffer &buf);

size_t
read (handle_t h, in_buffer &buf, error_code &ec) noexcept;

size_t
write (handle_t h, out_buffer buf);

size_t
write (handle_t h, out_buffer buf, error_code &ec) noexcept;

size_t
readable (handle_t h);

size_t
readable (handle_t h, error_code &ec) noexcept;

error_code
read_error (handle_t h) noexcept;

void
close (handle_t &h);

void
close (handle_t &h, error_code &ec) noexcept;

int
dup (handle_t h);

int
dup (handle_t h, error_code &ec) noexcept;

void
set_nonblock (handle_t h, bool on);

void
set_nonblock (handle_t h, bool on, error_code &ec) noexcept;

void
set_closeexec (handle_t h, bool on);

void
set_closeexec (handle_t h, bool on, error_code &ec) noexcept;

bool
nonblock (handle_t h);

bool
nonblock (handle_t h, error_code &ec) noexcept;

bool
closeexec (handle_t h);

bool
closeexec (handle_t h, error_code &ec) noexcept;

} /* namespace descriptor */

} /* namespace k::detail */

#endif /* defined(KNGIN_NOT_SYSTEM_WIN32) */

#endif /* KNGIN_DESCRIPTOR_H */
