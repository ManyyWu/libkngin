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

event_loop::pimpl::pimpl ()
    try
    : m_thr(std::make_shared<thread::pimpl>(nullptr)),
      m_ptid(thread::ptid()),
      m_epoller(),
      m_waker(nullptr),
      m_looping(true),
      m_stop(false),
      m_taskq(),
      m_taskq_mutex(),
      m_stop_barrier(std::make_shared<barrier>(2)),
      m_events(KNGIN_RESERVED_EPOLLELR_EVENT)
{
    //log_debug("loop in thread \"%s\" started", m_thr->name());
} catch (...) {
    log_fatal("event_loop::pimpl::pimpl() error");
    throw;
}

event_loop::pimpl::pimpl (thread &_thr)
    try
    : m_thr(_thr.weak_self()),
      m_ptid(_thr.ptid()), // unused
      m_epoller(),
      m_waker(nullptr),
      m_looping(true),
      m_stop(false),
      m_taskq(),
      m_taskq_mutex(),
      m_stop_barrier(std::make_shared<barrier>(2)),
      m_events(KNGIN_RESERVED_EPOLLELR_EVENT)
{
    arg_check(!m_thr.expired());
    //log_debug("loop in thread \"%s\" started", m_thr->name());
} catch (...) {
    m_looping = false;
    log_fatal("event_loop::pimpl::pimpl() error");
    throw;
}

event_loop::pimpl::~pimpl () KNGIN_NOEXCP
{
    if (m_looping)
        ignore_excp(stop());
}

void
event_loop::pimpl::run (started_handler &&_start_handler,
                        stopped_handler &&_stop_handler)
{
    m_looping = true;
    auto _loop = self();
    auto _thr  = m_thr.lock();

    try {
        m_waker = std::make_shared<event>([] () {});
        register_event(m_waker);
        if (_start_handler) {
            log_excp_error(
                _start_handler(),
                "start_handler() error"
            );
        }

        while (!m_stop) {
            // wait for events
            uint32_t _size = m_epoller.wait(m_events, KNGIN_EPOLLER_TIMEOUT);
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
                return (_ptr1->m_type > _ptr1->m_type or
                        (_ptr1->m_type == _ptr1->m_type and
                         _ptr1->m_priority > _ptr1->m_priority));
            }); // end of operator <
*/
            // process events
            epoller::process_events(m_events, _size);

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
        if (m_waker and m_waker->registed())
            remove_event(*m_waker);
        m_waker->close();
        m_waker.reset();
        if (_stop_handler) {
            log_excp_error(
                _stop_handler(),
                "stop_handler() error"
            );
        }
        m_looping = false;
        if (!m_stop_barrier->destroyed())
            m_stop_barrier->destroy();
        log_fatal("caught an exception in event_loop of thread \"%s\"",
                  _thr ? _thr->name() : "");
        throw;
    }

    if (m_waker and m_waker->registed())
        remove_event(*m_waker);
    m_waker->close();
    m_waker.reset();
    if (_stop_handler) {
        log_excp_error(
            _stop_handler(),
            "ignore_excp() error"
        );
    }
    m_looping = false;
    auto _temp_ptr = m_stop_barrier;
    if (!_temp_ptr->destroyed() and _temp_ptr->wait())
        _temp_ptr->destroy();
}

void
event_loop::pimpl::stop ()
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
        if (!m_stop_barrier->destroyed())
        m_stop_barrier->destroy();
    }
}

bool
event_loop::pimpl::in_loop_thread () const KNGIN_NOEXCP
{
    auto _thr = m_thr.lock();
    if (_thr)
        return _thr->equal_to(thread::ptid());
    else
        return thread::equal(m_ptid, thread::ptid());
}

void
event_loop::pimpl::register_event (epoller_event_ptr _e)
{
    assert(m_looping);
    m_epoller.register_event(_e);
    if (!in_loop_thread())
        wakeup();
}

void
event_loop::pimpl::remove_event (epoller_event &_e)
{
    assert(m_looping);
    m_epoller.remove_event(_e);
    if (!in_loop_thread())
        wakeup();
}

void
event_loop::pimpl::update_event (epoller_event &_e)
{
    assert(m_looping);
    m_epoller.modify_event(_e);
    if (!in_loop_thread())
        wakeup();
}

bool
event_loop::pimpl::registed (epoller_event &_e)
{
    assert(m_looping);
    m_epoller.registed(_e);
}

void
event_loop::pimpl::run_in_loop (event_loop::task &&_fn)
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
event_loop::pimpl::wakeup ()
{
    if (m_waker and m_looping and m_waker->registed())
        m_waker->notify(); // blocked
}

KNGIN_NAMESPACE_K_END