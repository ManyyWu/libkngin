#include "core/base/common.h"
#include "core/base/exception.h"
#include "core/base/system_error.h"
#include "core/event/epoller_event.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin/core/event/epoller_event.cpp"

KNGIN_NAMESPACE_K_BEGIN

epoller_event::epoller_event (epollfd _fd, EVENT_TYPE _type,
                              uint8_t _priority /* = UINT8_MAX*/) KNGIN_NOEXCP
    : filefd(_fd),
      m_flags(EPOLLHUP | EPOLLERR),
      m_event({0, nullptr}),
      m_registed(false),
      m_type(_type),
      m_priority(_priority)
{
}

epoller_event::epoller_event (epoller_event &&_e) KNGIN_NOEXCP
    : filefd(std::move(_e)),
      m_flags(_e.m_flags),
      m_event({0, nullptr}),
      m_registed(false),
      m_type(_e.m_type),
      m_priority(_e.m_priority)
{
    assert(!_e.m_registed);
    _e.m_event = {0, nullptr};
}

KNGIN_NAMESPACE_K_END
