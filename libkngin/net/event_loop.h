#ifndef _EVENT_LOOP_H_
#define _EVENT_LOOP_H_

#include "net/event_loop_pimpl.h"

#ifndef NDEBUG
#define EPOLLER_TIMEOUT 10000
#else
#define EPOLLER_TIMEOUT 3000
#endif

KNGIN_NAMESPACE_K_BEGIN

class event_loop : public noncopyable {
public:
    typedef event_loop_pimpl::started_handler      started_handler;

    typedef event_loop_pimpl::stopped_handler      stopped_handler;

    typedef event_loop_pimpl::task                 task;

public:
    event_loop     () KNGIN_NOEXP = delete;

    explicit
    event_loop     (thread &_thr) KNGIN_EXP
        : m_pimpl(std::make_shared<event_loop_pimpl>(_thr)) {}

    ~event_loop    () KNGIN_NOEXP = default;

public:
    void
    loop           (started_handler &&_start_cb, stopped_handler &&_stop_cb) KNGIN_EXP
    { m_pimpl->loop(std::move(_start_cb), std::move(_stop_cb)); }

    void
    stop           () KNGIN_EXP
    { m_pimpl->stop(); }

    bool
    looping        () KNGIN_NOEXP
    { return m_pimpl->looping(); }

public:
    void
    add_event      (epoller_event &_e) KNGIN_EXP
    { m_pimpl->add_event(_e); }

    void
    remove_event   (epoller_event &_e) KNGIN_EXP
    { m_pimpl->remove_event(_e); }

    void
    update_event   (epoller_event &_e) KNGIN_EXP
    { m_pimpl->update_event(_e); }

    void
    run_in_loop    (task &&_fn) KNGIN_EXP
    { m_pimpl->run_in_loop(std::move(_fn)); }

public:
    void
    check_thread   () const KNGIN_NOEXP
    { m_pimpl->check_thread(); }

    bool
    in_loop_thread () const KNGIN_NOEXP
    { m_pimpl->in_loop_thread(); }

public:
    event_loop_pimpl_ptr
    pimpl          ()
    { return m_pimpl; }

protected:
    event_loop_pimpl_ptr m_pimpl;
};

KNGIN_NAMESPACE_K_END

#endif /* _EVENT_LOOP_H_ */