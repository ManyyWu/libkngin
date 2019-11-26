#ifdef _WIN32
#else
#include <unistd.h>
#include <sys/eventfd.h>
#endif
#include <cstring>
#include "core/common.h"
#include "net/event_loop.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin/net/event_loop.cpp"

KNGIN_NAMESPACE_K_BEGIN

event_loop::pimpl::pimpl (thread &_thr)
    try
    : m_thr_pimpl(_thr.pimpl()),
      m_epoller(pimpl()),
      m_waker(pimpl()),
      m_looping(false),
      m_stop(false),
      m_taskq(),
      m_mutex(),
      m_events(RESERVED_EPOLLELR_EVENT)
{
    if (nullptr == m_thr_pimpl)
        throw k::exception("event_loop::pimpl::pimpl() - invalid argument");
    m_waker.set_nonblock(false);
    m_waker.set_closeexec(true);
} catch (...) {
    log_fatal("event_loop::pimpl::pimpl() error");
    throw;
}

event_loop::pimpl::~pimpl () KNGIN_NOEXP
{
    log_debug("wait for loop in thread \"%s\" to end", m_thr_pimpl->name());
    if (m_looping) {
        stop();
#warning "noexp"
        // FIXME: wait for loop to end
    }

    log_debug("loop in thread \"%s\" closed", m_thr_pimpl->name());
}

void
event_loop::pimpl::loop (started_handler &&_start_cb,
                         stopped_handler &&_stop_cb) KNGIN_EXP
{
    check_thread();
    m_looping = true;

    try {
        m_waker.start(nullptr);
        if (_start_cb)
            _start_cb();

        while (!m_stop) {
            // wait for events
            uint32_t _size = m_epoller.wait(m_events, EPOLLER_TIMEOUT);
            if (m_stop)
                break;
            //log_debug("the epoller in thread \"%s\" is awardkened with %" PRIu64 " events",
            //          m_thr_pimpl->name(), _size);

            // process events
            for (uint32_t i = 0; i < _size; ++i)
                ignore_exp(static_cast<epoller_event *>(m_events[i].data.ptr)
                           ->on_events(m_events[i].events));

            // process queued events
            std::deque<task> _fnq;
            if (!m_taskq.empty())
            {
                local_lock _lock(m_mutex);
                _fnq.swap(m_taskq);
            }
            for (auto _iter : _fnq)
                ignore_exp(_iter());
            //log_debug("the epoller in thread \"%s\" handled %" PRIu64 " task",
            //          m_thr_pimpl->name(), _fnq.size());
        }
    } catch (...) {
        if (_stop_cb)
            ignore_exp(_stop_cb());
        m_waker.stop();
        m_looping = false;
        log_fatal("caught an exception in event_loop of thread \"%s\"", m_thr_pimpl->name());
        throw;
    }

    if (_stop_cb)
        ignore_exp(_stop_cb());
    m_waker.stop();
    m_looping = false;
    log_info("event_loop in thread \"%s\" is stopped", m_thr_pimpl->name());
}

#include "core/common.h"
void
event_loop::pimpl::stop () KNGIN_EXP
{
    arg_check(m_looping);
    m_stop = true;
    if (!in_loop_thread())
        wakeup();
}

bool
event_loop::pimpl::looping () KNGIN_NOEXP
{
    return m_looping;
}

void
event_loop::pimpl::check_thread () const KNGIN_NOEXP
{
    check(m_thr_pimpl->equal_to(thread::ptid()));
}

bool
event_loop::pimpl::in_loop_thread () const KNGIN_NOEXP
{
    return (m_thr_pimpl->equal_to(thread::ptid()));
}

void
event_loop::pimpl::add_event (epoller_event &_e) KNGIN_EXP
{
    arg_check(_e.pimpl());
    check(m_looping);

    //bool _ret = m_epoller.register_event(_e);
    //if (!_ret)
    //    log_fatal("epoller::register_event() erorr");
    //if (!in_loop_thread())
    //    wakeup();
}

void
event_loop::pimpl::remove_event (epoller_event &_e) KNGIN_EXP
{
    arg_check(_e.pimpl());
    check(m_looping);

    //bool _ret = m_epoller.remove_event(_e.pimpl());
    //if (!_ret)
    //    log_fatal("epoller::remove_event() erorr");
    //if (!in_loop_thread())
    //    wakeup();
}

void
event_loop::pimpl::update_event (epoller_event &_e) KNGIN_EXP
{
    arg_check(_e.pimpl());
    check(m_looping);

    //bool _ret = m_epoller.modify_event(_e.pimpl());
    //if (!_ret)
    //    log_fatal("epoller::modify_event() erorr");
    //if (!in_loop_thread())
    //    wakeup();
}

void
event_loop::pimpl::run_in_loop (event_loop::task &&_fn) KNGIN_EXP
{
    check(m_looping);
    if (!_fn)
        return;

    {
        local_lock _lock(m_mutex);
        m_taskq.push_back(std::move(_fn));
    }
    
    if (!in_loop_thread())
        wakeup();
}

void
event_loop::pimpl::wakeup () KNGIN_EXP
{
    check(m_looping);
    //log_debug("wakeup event_loop in thread \"%s\"", m_thr_pimpl->name());
    if (m_waker.stopped())
        return;

    buffer _val(8);
    _val.write_uint64(1);
    std::error_code _ec;
    size_t _ret = m_waker.write(_val, 8, _ec); // blocked
    if (_ec)
        log_error("event_loop::pimpl::wakeup() error - %s:%d",
                  system_error_str(_ec).c_str());
    else if (_ret != sizeof(_ret))
        log_error("event_loop::pimpl::wakeup() error, write %" PRId64
                  " bytes to waker instead of 8", _ret);
}

KNGIN_NAMESPACE_K_END
