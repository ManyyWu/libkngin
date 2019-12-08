#ifndef _TIMER_H_
#define _TIMER_H_

#include <memory>
#include <functional>
#include "core/define.h"
#include "core/timestamp.h"
#include "net/epoller_event.h"

KNGIN_NAMESPACE_K_BEGIN

class event_loop;
class event_loop_pimpl;
class timer : public noncopyable {
public:
    typedef std::shared_ptr<event_loop_pimpl> event_loop_pimpl_ptr;

    typedef std::function<void (void)>        timeout_handler;

private:
    class pimpl
        : public epoller_event,
          public std::enable_shared_from_this<timer::pimpl> {
    public:
        pimpl    () = delete;

        pimpl    (event_loop_pimpl_ptr _loop,
                  timeout_handler &&_timeout_handler);

        virtual
        ~pimpl   () KNGIN_NOEXP;

    public:
        timestamp
        get_time ();

        void
        set_time (timestamp _val, timestamp _interval, bool _abs = false);

    public:
        event_loop_pimpl_ptr &
        loop     () KNGIN_NOEXP
        { return m_loop; }

        std::shared_ptr<timer::pimpl>
        self     ()
        { return shared_from_this(); }

    private:
        virtual void
        on_read  ();

        virtual void
        on_error ();

    private:
        event_loop_pimpl_ptr m_loop;

        timeout_handler      m_timeout_handler;
    };

    typedef timer::pimpl                 timer_pimpl;

    typedef std::shared_ptr<timer_pimpl> timer_pimpl_ptr;

public:
    timer      () = delete;

    timer      (event_loop &_loop, timeout_handler &&_timeout_handler);

    virtual
    ~timer     () KNGIN_NOEXP {}

public:
    timestamp
    get_time   ()
    { return m_pimpl->get_time(); }

    void
    set_time   (timestamp _val, timestamp _interval, bool _abs = false)
    { m_pimpl->set_time(_val, _interval, _abs); }

private:
    timer_pimpl_ptr
    pimpl      ()
    { return m_pimpl; }

private:
    timer_pimpl_ptr m_pimpl;

private:
    friend class epoller;
};

KNGIN_NAMESPACE_K_END
#endif /* _TIMER_H_ */
