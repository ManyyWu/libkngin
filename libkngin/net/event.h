#ifndef _EVENT_H_
#define _EVENT_H_

#include <memory>
#include <functional>
#include "core/define.h"
#include "net/epoller_event.h"

KNGIN_NAMESPACE_K_BEGIN

class event_loop;
class event_loop_pimpl;
class event : public noncopyable {
public:
    typedef std::shared_ptr<event_loop_pimpl> event_loop_pimpl_ptr;

    typedef std::function<void (void)>        event_handler;

private:
    class pimpl
        : public epoller_event,
          public std::enable_shared_from_this<event::pimpl> {
    public:
        pimpl    () = delete;

        pimpl    (event_loop_pimpl_ptr _loop,
                  event_handler &&_event_handler);

        virtual
        ~pimpl   () KNGIN_NOEXP;

    public:
        void
        notify   ();

    public:
        event_loop_pimpl_ptr &
        loop     () KNGIN_NOEXP
        { return m_loop; }

        std::shared_ptr<event::pimpl>
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
    };

public:
    typedef event::pimpl                 event_pimpl;

    typedef std::shared_ptr<event_pimpl> event_pimpl_ptr;

public:
    event  () = delete;

    event  (event_loop_pimpl_ptr _loop, event_handler &&_event_handler);

    event  (event_loop &_loop, event_handler &&_event_handler);

    virtual
    ~event () KNGIN_NOEXP {}

public:
    void
    notify ()
    { m_pimpl->notify(); }

public:
    event_pimpl_ptr
    pimpl  ()
    { return m_pimpl; }

private:
    event_pimpl_ptr m_pimpl;

private:
    friend class epoller;
};

KNGIN_NAMESPACE_K_END

#endif /* _EVENT_H_ */