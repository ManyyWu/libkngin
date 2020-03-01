#include "core/base/define.h"
#ifndef KNGIN_FLAG_HAVE_EPOLLER_EVENT

#include "core/base/common.h"
#include "core/base/exception.h"
#include "core/event/iocp_event.h"

#define KNGIN_FILENAME "libkngin/core/event/iocp_event.cpp"

KNGIN_NAMESPACE_K_BEGIN

iocp_event::iocp_event (HANDLE handle, EVENT_TYPE type,
                        uint8_t priority /* = UINT8_MAX*/) noexcept
    : flags_(IOCP_EVENT_HUP | IOCP_EVENT_ERR),
      handle_(handle),
      registed_(false),
      type_(type),
      priority_(priority),
      index_()
{
}

iocp_event::iocp_event (iocp_event &&e) noexcept
    : flags_(e.flags_),
      handle_(e.handle_),
      registed_(false),
      type_(e.type_),
      priority_(e.priority_),
      index_()
{
    assert(!e.registed_);
    e.handle_ = INVALID_HANDLE_VALUE;
}

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_FLAG_HAVE_EPOLLER_EVENT */
