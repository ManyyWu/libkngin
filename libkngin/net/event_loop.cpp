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

event_loop_pimpl::event_loop_pimpl (thread &_thr)
    try
    : m_thr(_thr.pimpl()),
      m_epoller(nullptr),
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
    log_debug("loop in thread \"%s\" started", m_thr->name());
} catch (...) {
    log_fatal("event_loop_pimpl::event_loop_pimpl() error");
    throw;
}

event_loop_pimpl::~event_loop_pimpl () KNGIN_NOEXP
{
    if (m_looping)
        ignore_exp(stop());
    // FIXME: wait for loop to end

    log_debug("loop in thread \"%s\" closed", m_thr->name());
}

void
event_loop_pimpl::run (started_handler &&_start_handler,
                       stopped_handler &&_stop_handler)
{
    check(!m_looping);
    check_thread();
    m_looping = true;

    try {
        m_epoller = std::make_shared<epoller>(self());
        m_waker = std::make_shared<event>(self());
        m_waker->set_nonblock(false);
        m_waker->set_closeexec(true);
        m_waker->start(nullptr);
        if (_start_handler)
            _start_handler();

        while (!m_stop) {
            // wait for events
            uint32_t _size = m_epoller->wait(m_events, EPOLLER_TIMEOUT);
            if (m_stop)
                break;
            log_debug("the epoller in thread \"%s\" is awaken with %" PRIu64 " events",
                      m_thr->name(), _size);

            // process events
            for (uint32_t i = 0; i < _size; ++i)
                static_cast<epoller_event *>(m_events[i].data.ptr)
                    ->on_events(m_events[i].events);

            // process queued events
            std::deque<task> _fnq;
            if (!m_taskq.empty())
            {
                local_lock _lock(m_taskq_mutex);
                _fnq.swap(m_taskq);
            }
            for (auto _iter : _fnq)
                _iter();
            //log_debug("the epoller in thread \"%s\" handled %" PRIu64 " task",
            //          m_thr->name(), _fnq.size());
        }
    } catch (...) {
        if (_stop_handler)
            ignore_exp(_stop_handler());
        m_waker->stop();
        m_looping = false;
        log_fatal("caught an exception in event_loop of thread \"%s\"", m_thr->name());
        throw;
    }

    if (_stop_handler)
        ignore_exp(_stop_handler());
    m_waker->stop();
    m_looping = false;
    std::shared_ptr<barrier> _temp_ptr = m_stop_barrier;
    if (_temp_ptr->wait())
        _temp_ptr->destroy();
    log_info("event_loop in thread \"%s\" is stopped", m_thr->name());
}

void
event_loop_pimpl::stop ()
{
    if (!m_looping)
        return;
    check(m_epoller);
    check(m_waker);

    m_stop = true;
    if (!in_loop_thread())
        wakeup();
    std::shared_ptr<barrier> _temp_ptr = m_stop_barrier;
    if (_temp_ptr->wait())
        _temp_ptr->destroy();
}

bool
event_loop_pimpl::looping () KNGIN_NOEXP
{
    return m_looping;
}

void
event_loop_pimpl::check_thread () const KNGIN_NOEXP
{
    check(m_thr->equal_to(thread::ptid()));
}

bool
event_loop_pimpl::in_loop_thread () const KNGIN_NOEXP
{
    return (m_thr->equal_to(thread::ptid()));
}

void
event_loop_pimpl::add_event (epoller_event &_e)
{
    check(m_looping);
    check(m_epoller);
    check(m_waker);

    log_exp_error(
        m_epoller->register_event(&_e),
        "epoller::register_event() erorr"
    );
    if (!in_loop_thread())
        wakeup();
}

void
event_loop_pimpl::remove_event (epoller_event &_e)
{
    check(m_looping);
    check(m_epoller);
    check(m_waker);

    log_exp_error(
        m_epoller->remove_event(&_e),
        "epoller::remove_event() erorr"
    );
    if (!in_loop_thread())
        wakeup();
}

void
event_loop_pimpl::update_event (epoller_event &_e)
{
    check(m_looping);
    check(m_epoller);
    check(m_waker);

    log_exp_error(
        m_epoller->modify_event(&_e),
        "epoller::modify_event() erorr"
    );
    if (!in_loop_thread())
        wakeup();
}

void
event_loop_pimpl::run_in_loop (event_loop::task &&_fn)
{
    check(m_looping);
    check(m_epoller);
    check(m_waker);
    if (!_fn)
        return;

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
    if (!m_looping)
        return;
    if (!m_epoller)
        return;
    if (!m_waker)
        return;
    if (m_waker->stopped())
        return;
    //log_debug("wakeup event_loop in thread \"%s\"", m_thr->name());

    std::error_code _ec;
    m_waker->notify(_ec); // blocked
    if (_ec)
        log_error("event_loop_pimpl::wakeup() error, %s",
                  system_error_str(_ec).c_str());
}

KNGIN_NAMESPACE_K_END