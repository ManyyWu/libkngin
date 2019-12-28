#ifdef _WIN32
#else
#include <unistd.h>
#include <sys/eventfd.h>
#endif
#include <cstring>
#include <algorithm>
#include "core/common.h"
#include "core/system_error.h"
#include "net/event_loop.h"
#include "net/event.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin/net/event_loop.cpp"

KNGIN_NAMESPACE_K_BEGIN

event_loop_pimpl::event_loop_pimpl ()
    try
    : m_thr(nullptr),
      m_ptid(thread::ptid()),
      m_epoller(),
      m_waker(nullptr),
      m_looping(true),
      m_stop(false),
      m_taskq(),
      m_taskq_mutex(),
      m_stop_barrier(std::make_shared<barrier>(2)),
      m_events(RESERVED_EPOLLELR_EVENT)
#warning "TODO: RESERVED_EPOLLELR_EVENT param"
{
    //log_debug("loop in thread \"%s\" started", m_thr->name());
} catch (...) {
    m_looping = false;
    log_fatal("event_loop_pimpl::event_loop_pimpl() error");
    throw;
}

event_loop_pimpl::event_loop_pimpl (thread &_thr)
    try
    : m_thr(_thr.pimpl()),
      m_ptid(), // unused
      m_epoller(),
      m_waker(nullptr),
      m_looping(true),
      m_stop(false),
      m_taskq(),
      m_taskq_mutex(),
      m_stop_barrier(std::make_shared<barrier>(2)),
      m_events(RESERVED_EPOLLELR_EVENT)
#warning "TODO: RESERVED_EPOLLELR_EVENT param"
{
    if (!m_thr)
        throw k::exception("invalid argument");
    //log_debug("loop in thread \"%s\" started", m_thr->name());
} catch (...) {
    m_looping = false;
    log_fatal("event_loop_pimpl::event_loop_pimpl() error");
    throw;
}

event_loop_pimpl::~event_loop_pimpl () KNGIN_NOEXCP
{
    if (m_looping)
        ignore_excp(stop());

    log_debug("loop in thread \"%s\" closed", m_thr ? m_thr->name() : "");
}

void
event_loop_pimpl::run (started_handler &&_start_handler,
                       stopped_handler &&_stop_handler)
{
    //assert(!m_looping);
    check_thread();
    m_looping = true;

    try {
        auto _loop = self();
        m_waker = std::make_shared<event>(_loop, [] () {});
        register_event(m_waker);
        if (_start_handler) {
            log_excp_error(
                _start_handler(),
                "start_handler() error"
            );
        }

        while (!m_stop) {
            // wait for events
            uint32_t _size = m_epoller.wait(m_events, EPOLLER_TIMEOUT);
            if (m_stop)
                break;

            //log_warning("the epoller in thread \"%s\" is awaken with %" PRIu64 " events",
            //            m_thr->name(), _size);
/*
            // sort the events by priority and type(timer > event > file)
            std::sort(m_events.begin(), m_events.begin() + _size,
                [] (struct ::epoll_event &_e1, struct ::epoll_event &_e2) -> bool {
                epoller_event *_ptr1 = static_cast<epoller_event *>(_e1.data.ptr);
                epoller_event *_ptr2 = static_cast<epoller_event *>(_e2.data.ptr);
                return (_ptr1->m_type > _ptr1->m_type ||
                        (_ptr1->m_type == _ptr1->m_type &&
                         _ptr1->m_priority > _ptr1->m_priority));
            }); // end of operator <
*/
            // process events
            for (uint32_t _i = 0; _i < _size; _i++) {
                auto *_ptr = static_cast<epoller_event *>(m_events[_i].data.ptr);
                assert(_ptr);
                log_excp_error(
                    epoller_event::on_events(_ptr, m_events[_i].events),
                    "epoller_event::on_events() error"
                );
            }

            // process queued events
            std::queue<task> _fnq;
            {
                local_lock _lock(m_taskq_mutex);
                if (!m_taskq.empty())
                    _fnq.swap(m_taskq);
            }
            while (!_fnq.empty()) {
                _fnq.front()();
                _fnq.pop();
            }
            //log_warning("the epoller in thread \"%s\" handled %" PRIu64 " task",
            //            m_thr->name(), _fnq.size());
        }
    } catch (...) {
        if (m_waker && m_waker->registed())
            remove_event(*m_waker);
        m_waker.reset();
        if (_stop_handler) {
            log_excp_error(
                _stop_handler(),
                "stop_handler() error"
            );
        }
        m_looping = false;
        log_fatal("caught an exception in event_loop of thread \"%s\"",
                  m_thr ? m_thr->name() : "");
        throw;
    }

    if (m_waker && m_waker->registed())
        remove_event(*m_waker);
    m_waker.reset();
    if (_stop_handler) {
        log_excp_error(
            _stop_handler(),
            "ignore_excp() error"
        );
    }
    m_looping = false;
    std::shared_ptr<barrier> _temp_ptr = m_stop_barrier;
    if (!_temp_ptr->destroyed() && _temp_ptr->wait())
        _temp_ptr->destroy();
}

void
event_loop_pimpl::stop ()
{
    if (!m_looping)
        return;

    m_stop = true;
    if (!in_loop_thread()) {
        wakeup();
        std::shared_ptr<barrier> _temp_ptr = m_stop_barrier;
        if (_temp_ptr->wait())
            _temp_ptr->destroy();
    } else {
        m_stop_barrier->destroy();
    }
}

bool
event_loop_pimpl::looping () KNGIN_NOEXCP
{
    return m_looping;
}

void
event_loop_pimpl::check_thread () const KNGIN_NOEXCP
{
    if (m_thr)
        assert(m_thr->equal_to(thread::ptid()));
}

bool
event_loop_pimpl::in_loop_thread () const KNGIN_NOEXCP
{
    if (m_thr)
        return m_thr->equal_to(thread::ptid());
    else
        return (thread::equal(m_ptid, thread::ptid()));
}

bool
event_loop_pimpl::registed (epoller_event &_e)
{
    assert(m_looping);
    m_epoller.registed(_e);
}

void
event_loop_pimpl::register_event (epoller_event_ptr _e)
{
    assert(m_looping);
    m_epoller.register_event(_e);
    if (!in_loop_thread())
        wakeup();
}

void
event_loop_pimpl::remove_event (epoller_event &_e)
{
    assert(m_looping);
    m_epoller.remove_event(_e);
    if (!in_loop_thread())
        wakeup();
}

void
event_loop_pimpl::update_event (epoller_event &_e)
{
    assert(m_looping);
    m_epoller.modify_event(_e);
    if (!in_loop_thread())
        wakeup();
}

void
event_loop_pimpl::run_in_loop (event_loop::task &&_fn)
{
    assert(m_looping);
    if (_fn)
    {
        local_lock _lock(m_taskq_mutex);
        m_taskq.push(std::move(_fn));
    }
    if (!in_loop_thread())
        wakeup();
}

void
event_loop_pimpl::wakeup ()
{
    if (!m_waker || !m_looping || !m_waker->registed())
        return;
    //log_debug("wakeup event_loop in thread \"%s\"",
    //          m_thr ? m_thr->name() : "");

    m_waker->notify(); // blocked
}

KNGIN_NAMESPACE_K_END