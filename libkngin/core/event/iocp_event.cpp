#include "core/base/define.h"
#ifndef KNGIN_FLAG_HAVE_EPOLLER_EVENT

#include "core/base/common.h"
#include "core/base/exception.h"
#include "core/event/iocp_event.h"

#define KNGIN_FILENAME "libkngin/core/event/iocp_event.cpp"

KNGIN_NAMESPACE_K_BEGIN

iocp_event::iocp_event (HANDLE handle, EVENT_TYPE type,
                        uint8_t priority /* = UINT8_MAX*/) noexcept
    : m_flags(IOCP_EVENT_HUP | IOCP_EVENT_ERR),
      m_handle(handle),
      m_registed(false),
      m_type(type),
      m_priority(priority),
      m_index()
{
}

iocp_event::iocp_event (iocp_event &&e) noexcept
    : m_flags(e.m_flags),
      m_handle(e.m_handle),
      m_registed(false),
      m_type(e.m_type),
      m_priority(e.m_priority),
      m_index()
{
    assert(!e.m_registed);
    e.m_handle = INVALID_HANDLE_VALUE;
}

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_FLAG_HAVE_EPOLLER_EVENT */
