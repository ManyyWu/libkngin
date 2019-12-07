#ifndef _TIMER_H_
#define _TIMER_H_

#include <memory>
#include "core/define.h"
#include "core/timestamp.h"
#include "net/filefd.h"
#include "net/epoller_event.h"
#include "net/event_loop.h"

KNGIN_NAMESPACE_K_BEGIN

class timer {
public:
    typedef event_loop::event_loop_pimpl_ptr event_loop_pimpl_ptr;

    typedef event_loop::epoller_event_ptr    epoller_event_ptr;

    typedef std::function<void (void)>       timeout_handler;

public:
    class pimpl: public epoller_event,
                 public std::enable_shared_from_this<timer::pimpl> {
    public:
        pimpl    () = delete;

        explicit
        pimpl    (const event_loop_pimpl_ptr &_loop, 
                  const timeout_handler &&_timeout_handler);

        virtual
        ~pimpl   () KNGIN_NOEXP;

    public:
        timestamp
        get_time ();

        void
        set_time (timestamp _val, timestamp _interval, bool _abs = false);

    public:
        event_loop_pimpl_ptr &
        loop     ()
        { return m_loop; }

        std::shared_ptr<timer::pimpl>
        self     ()
        { return shared_from_this(); }

    private:
        virtual void
        on_read  ();

        virtual void
        on_error  ();

    private:
        event_loop_pimpl_ptr m_loop;

        timeout_handler      m_timeout_handler;
    };

    typedef timer::pimpl                 timer_pimpl;

    typedef std::shared_ptr<timer_pimpl> timer_pimpl_ptr;

public:
    timer      () = delete;

    explicit
    timer      (const event_loop_pimpl_ptr &_loop, 
                const timeout_handler &&_timeout_handler)
        : m_pimpl(std::make_shared<timer::pimpl>(_loop, std::move(_timeout_handler))) {} 

    virtual
    ~timer     () KNGIN_NOEXP {}

public:
    timestamp
    get_time   ()
    { return m_pimpl->get_time(); }

    void
    set_time   (timestamp _val, timestamp _interval, bool _abs = false)
    { set_time(_val, _interval, _abs); }

private:
    timer_pimpl_ptr m_pimpl;
};

KNGIN_NAMESPACE_K_END
#endif /* _TIMER_H_ */
