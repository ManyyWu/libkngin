#ifndef KNGIN_EVENT_H
#define KNGIN_EVENT_H

#include "core/base/define.h"
#ifdef KNGIN_FLAG_HAVE_EVENTFD

#include <memory>
#include <functional>
#include "core/event/epoller_event.h"

KNGIN_NAMESPACE_K_BEGIN

class event_loop;
class event
    : public epoller_event,
      public std::enable_shared_from_this<event> {
    friend class event_loop;

public:
    typedef std::function<void (void)> event_handler;

    typedef std::shared_ptr<event>     event_ptr;

    typedef std::weak_ptr<event>       event_weak_ptr;

public:
    event     () = delete;

    event     (event_handler &&event_handler);

    virtual
    ~event    () noexcept;

public:
    void
    notify    ();

    void
    close     ();

public:
    event_ptr
    self      ()
    { return shared_from_this(); }

private:
    virtual void
    on_events (event_loop &loop, uint32_t flags);

    void
    on_read   ();

private:
    event_handler event_handler_;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_EVENT_H */

#endif /* KNGIN_FLAG_HAVE_EVENTFD */
