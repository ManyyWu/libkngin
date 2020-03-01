#include "core/base/define.h"
#ifndef KNGIN_FLAG_HAVE_EPOLLER_EVENT

#include "core/base/common.h"
#include "core/base/exception.h"
#include "core/event/iocp_event.h"

#define KNGIN_FILENAME "libkngin/core/event/iocp_event.cpp"

KNGIN_NAMESPACE_K_BEGIN

iocp_event::iocp_event (HANDLE _handle, EVENT_TYPE _type,
                        uint8_t _priority /* = UINT8_MAX*/) noexcept
    : m_flags(IOCP_EVENT_HUP | IOCP_EVENT_ERR),
      m_handle(_handle),
      m_registed(false),
      m_type(_type),
      m_priority(_priority),
      m_index()
{
}

iocp_event::iocp_event (iocp_event &&_e) noexcept
    : m_flags(_e.m_flags),
      m_handle(_e.m_handle),
      m_registed(false),
      m_type(_e.m_type),
      m_priority(_e.m_priority),
      m_index()
{
    assert(!_e.m_registed);
    _e.m_handle = INVALID_HANDLE_VALUE;
}

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_FLAG_HAVE_EPOLLER_EVENT */
