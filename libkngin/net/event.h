#ifndef KNGIN_EVENT_H
#define KNGIN_EVENT_H

#include <memory>
#include <functional>
#include "core/define.h"
#include "net/epoller_event.h"

KNGIN_NAMESPACE_K_BEGIN

class event_loop;
class event_loop_pimpl;
class event
    : public epoller_event,
      public std::enable_shared_from_this<event> {
public:
    typedef std::shared_ptr<event_loop_pimpl> event_loop_pimpl_ptr;

    typedef std::function<void (void)>        event_handler;

    typedef std::shared_ptr<event>            event_ptr;

public:
    event    () = delete;

    event    (event_loop_pimpl_ptr _loop,
              event_handler &&_event_handler);

    virtual
    ~event   () KNGIN_NOEXCP;

public:
    void
    notify   ();

public:
    event_loop_pimpl_ptr &
    loop     () KNGIN_NOEXCP
    { return m_loop; }

    event_ptr
    self     ()
    { return shared_from_this(); }

private:
    virtual void
    on_read  ();

    virtual void
    on_error ();

private:
    event_loop_pimpl_ptr m_loop;

    event_handler        m_event_handler;

private:
    friend class epoller;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_EVENT_H */