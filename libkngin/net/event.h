#ifndef KNGIN_EVENT_H
#define KNGIN_EVENT_H

#include <memory>
#include <functional>
#include "core/define.h"
#include "net/epoller_event.h"

KNGIN_NAMESPACE_K_BEGIN

class event
    : public epoller_event,
      public std::enable_shared_from_this<event> {
public:
    typedef std::function<void (void)> event_handler;

    typedef std::shared_ptr<event>     event_ptr;

    typedef std::weak_ptr<event>       event_weak_ptr;

public:
    event    () = delete;

    event    (event_handler &&_event_handler);

    virtual
    ~event   () KNGIN_NOEXCP;

public:
    void
    notify   ();

    void
    close    ();

public:
    event_ptr
    self     ()
    { return shared_from_this(); }

private:
    virtual void
    on_read  ();

    virtual void
    on_error ();

private:
    event_handler m_event_handler;

private:
    friend class epoller;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_EVENT_H */