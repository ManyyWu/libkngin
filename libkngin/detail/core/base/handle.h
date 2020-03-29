#ifndef KNGIN_HANDLE_H
#define KNGIN_HANDLE_H

#include "kngin/core/define.h"
#if defined(KNGIN_HAS_HANDLE)

#include "kngin/core/base/noncopyable.h"
#include "kngin/core/base/system_error.h"
#include "kngin/core/base/buffer.h"
#include "detail/core/base/win_utils.h"

KNGIN_NAMESPACE_K_DETAIL_BEGIN

namespace handle {

size_t
read (HANDLE handle, in_buffer &buf);

size_t
read (HANDLE handle, in_buffer &buf, error_code &ec) noexcept;

size_t
write (HANDLE handle, out_buffer &buf);

size_t
write (HANDLE handle, out_buffer &buf, error_code &ec) noexcept;

size_t
readable (HANDLE handle);

size_t
readable (HANDLE handle, error_code &ec) noexcept;

error_code
read_error (HANDLE handle) noexcept;

void
close (int &fd);

void
close (int &fd, error_code &ec) noexcept;

void
set_nonblock (HANDLE handle, bool on);

void
set_nonblock (HANDLE handle, bool on, error_code &ec) noexcept;

void
set_closeexec (HANDLE handle, bool on);

void
set_closeexec (HANDLE handle, bool on, error_code &ec) noexcept;

bool
nonblock (HANDLE handle);

bool
nonblock (HANDLE handle, error_code &ec) noexcept;

};

KNGIN_NAMESPACE_K_DETAIL_END

#endif /* defined(KNGIN_HAS_HANDLE) */

#endif /* KNGIN_HANDLE_H */
