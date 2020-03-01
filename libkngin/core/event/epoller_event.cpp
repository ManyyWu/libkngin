#include "core/base/define.h"
#ifdef KNGIN_FLAG_HAVE_EPOLLER_EVENT

#include "core/base/common.h"
#include "core/base/exception.h"
#include "core/event/epoller_event.h"

#define KNGIN_FILENAME "libkngin/core/event/epoller_event.cpp"

KNGIN_NAMESPACE_K_BEGIN

epoller_event::epoller_event (epollfd fd, EVENT_TYPE type,
                              uint8_t priority /* = UINT8_MAX*/) noexcept
    : filefd(fd),
      m_flags(EPOLLHUP | EPOLLERR),
      m_event({0, nullptr}),
      m_registed(false),
      m_type(type),
      m_priority(priority),
      m_index()
{
}

epoller_event::epoller_event (epoller_event &&e) noexcept
    : filefd(std::move(e)),
      m_flags(e.m_flags),
      m_event({0, nullptr}),
      m_registed(false),
      m_type(e.m_type),
      m_priority(e.m_priority),
      m_index()
{
    assert(!e.m_registed);
    e.m_event = {0, nullptr};
}

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_FLAG_HAVE_EPOLLER_EVENT */
