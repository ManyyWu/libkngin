#ifdef _WIN32
#else
#include <unistd.h>
#include <sys/eventfd.h>
#endif
#include <cstring>
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
      m_looping(false),
      m_stop(false),
      m_taskq(),
      m_taskq_mutex(),
      m_stop_barrier(std::make_shared<barrier>(2)),
      m_events(RESERVED_EPOLLELR_EVENT)
#warning "TODO: RESERVED_EPOLLELR_EVENT param"
{
    //log_debug("loop in thread \"%s\" started", m_thr->name());
} catch (...) {
    log_fatal("event_loop_pimpl::event_loop_pimpl() error");
    throw;
}

event_loop_pimpl::event_loop_pimpl (thread &_thr)
    try
    : m_thr(_thr.pimpl()),
      m_ptid(), // unused
      m_epoller(),
      m_waker(nullptr),
      m_looping(false),
      m_stop(false),
      m_taskq(),
      m_taskq_mutex(),
      m_stop_barrier(std::make_shared<barrier>(2)),
      m_events(RESERVED_EPOLLELR_EVENT)
#warning "TODO: RESERVED_EPOLLELR_EVENT param"
{
    if (nullptr == m_thr)
        throw k::exception("invalid argument");
    //log_debug("loop in thread \"%s\" started", m_thr->name());
} catch (...) {
    log_fatal("event_loop_pimpl::event_loop_pimpl() error");
    throw;
}

event_loop_pimpl::~event_loop_pimpl () KNGIN_NOEXCP
{
    if (m_looping)
        ignore_excp(stop());

    log_debug("loop in thread \"%s\" closed",
              m_thr ? m_thr->name() : "");
}

void
event_loop_pimpl::run (started_handler &&_start_handler,
                       stopped_handler &&_stop_handler)
{
    assert(!m_looping);
    check_thread();
    m_looping = true;

    try {
        m_waker = std::make_shared<event>(self(), [] () {});
        register_event(m_waker);
        if (_start_handler)
            ignore_excp(_start_handler());

        while (!m_stop) {
            // wait for events
            uint32_t _size = m_epoller.wait(m_events, EPOLLER_TIMEOUT);
            if (m_stop)
                break;
            //log_warning("the epoller in thread \"%s\" is awaken with %" PRIu64 " events",
            //            m_thr->name(), _size);

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
            std::deque<task> _fnq;
            if (!m_taskq.empty())
            {
                local_lock _lock(m_taskq_mutex);
                _fnq.swap(m_taskq);
            }
            for (auto _iter : _fnq)
                _iter();
            //log_warning("the epoller in thread \"%s\" handled %" PRIu64 " task",
            //            m_thr->name(), _fnq.size());
        }
    } catch (...) {
        if (_stop_handler)
            ignore_excp(_stop_handler());
        remove_event(m_waker);
        m_looping = false;
        log_fatal("caught an exception in event_loop of thread \"%s\"",
                  m_thr ? m_thr->name() : "");
        throw;
    }

    if (_stop_handler)
        ignore_excp(_stop_handler());
    remove_event(m_waker);
    m_looping = false;
    std::shared_ptr<barrier> _temp_ptr = m_stop_barrier;
    if (_temp_ptr->wait())
        _temp_ptr->destroy();
}

void
event_loop_pimpl::stop ()
{
    if (!m_looping)
        return;

    m_stop = true;
    if (!in_loop_thread())
        wakeup();
    std::shared_ptr<barrier> _temp_ptr = m_stop_barrier;
    if (_temp_ptr->wait())
        _temp_ptr->destroy();
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
event_loop_pimpl::registed (epoller_event_ptr _e)
{
    assert(m_looping);
    m_epoller.registed(_e->fd());
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
event_loop_pimpl::remove_event (epoller_event_ptr _e)
{
    assert(m_looping);
    m_epoller.remove_event(_e);
    if (!in_loop_thread())
        wakeup();
}

void
event_loop_pimpl::update_event (epoller_event_ptr _e)
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
        m_taskq.push_back(std::move(_fn));
    }
    if (!in_loop_thread())
        wakeup();
}

void
event_loop_pimpl::wakeup ()
{
    if (!m_waker || !m_looping)
        return;
    //log_debug("wakeup event_loop in thread \"%s\"",
    //          m_thr ? m_thr->name() : "");

    m_waker->notify(); // blocked
}

KNGIN_NAMESPACE_K_END