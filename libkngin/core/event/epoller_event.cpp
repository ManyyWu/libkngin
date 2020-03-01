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
      flags_(EPOLLHUP | EPOLLERR),
      event_({0, nullptr}),
      registed_(false),
      type_(type),
      priority_(priority),
      index_()
{
}

epoller_event::epoller_event (epoller_event &&e) noexcept
    : filefd(std::move(e)),
      flags_(e.flags_),
      event_({0, nullptr}),
      registed_(false),
      type_(e.type_),
      priority_(e.priority_),
      index_()
{
    assert(!e.registed_);
    e.event_ = {0, nullptr};
}

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_FLAG_HAVE_EPOLLER_EVENT */
