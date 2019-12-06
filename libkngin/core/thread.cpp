#ifdef _WIN32
#include "pthreads_win32/include/pthread.h"
#else
#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>
#endif
#include "core/system_error.h"
#include "core/common.h"
#include "core/exception.h"
#include "core/thread.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin/core/thread.cpp"

KNGIN_NAMESPACE_K_BEGIN

thread::pimpl::pimpl ()
    try
    : m_name(""),
#ifdef _WIN32
      m_thr({nullptr, 0}),
#else
      m_thr(0),
#endif
      m_joined(false)
{
} catch (...) {
    log_fatal("thread::pimpl::pimpl() error");
    throw;
}

thread::pimpl::pimpl (const char *_name)
    try
    : m_name(_name ? _name : ""),
#ifdef _WIN32
      m_thr({nullptr, 0}),
#else
      m_thr(0),
#endif
      m_joined(false)
{
} catch (...) {
    log_fatal("thread::pimpl::pimpl() error");
    throw;
}

thread::pimpl::~pimpl () KNGIN_NOEXP
{
    if (m_joined)
        return;
    std::error_code _ec = int2ec(::pthread_detach(m_thr));
    if (_ec)
        log_fatal("::pthread_detach() error, name = \"%s\", %s",
                   m_name.c_str(), system_error_str(_ec).c_str());
    else
        log_info("thread \"%s\" has detached", m_name.c_str());
}

void
thread::pimpl::run (thr_fn &&_fn, crash_handler &&_crash_handler /* = nullptr */)
{
    arg_check(_fn);

    std::error_code _ec = int2ec(::pthread_create(
                                     &m_thr, nullptr,
                                     thread::pimpl::start,
                                     new thread::pimpl::thread_data(
                                         m_name, 
                                         std::move(_fn),
                                         std::move(_crash_handler)
                                         )
                                     )
                                 );
    if (_ec) {
        log_fatal("::pthread_create() error, name = \"%s\", %s",
                  m_name.c_str(), system_error_str(_ec).c_str());
        throw k::exception("::pthread_create() error");
    }
    log_info("thread \"%s\" is running", m_name.c_str());
}

int
thread::pimpl::join ()
{
    if (equal_to(ptid()))
        throw k::exception("thread::pimpl::join() error, try to join self");

    thread_err_code _code;
    std::error_code _ec = int2ec(::pthread_join(m_thr, &_code.ptr));
    m_joined = true;
    if (_ec) {
        log_fatal("::pthread_join(), name = \"%s\", %s",
                  m_name.c_str(), system_error_str(_ec).c_str());
        throw k::exception("::pthread_join() error");
    }
    log_info("thread \"%s\" has joined with code: %d", m_name.c_str(), _code.code);
    return _code.code;
}

void
thread::pimpl::cancel ()
{
    if (equal_to(ptid()))
        throw k::exception("thread::pimpl::cancel() error, try to cancel self");

    std::error_code _ec = int2ec(::pthread_cancel(m_thr));
    if (_ec) {
        log_fatal("::pthread_cancel(), name = \"%s\", %s",
                  m_name.c_str(), system_error_str(_ec).c_str());
        throw k::exception("::pthread_cancel() error");
    }
    log_info("thread \"%s\" canceled", m_name.c_str());
}

bool
thread::pimpl::joined () const KNGIN_NOEXP
{
    return m_joined;
}

pthread_t
thread::pimpl::get_interface () const KNGIN_NOEXP
{
    return m_thr;
}

const char *
thread::pimpl::name () const KNGIN_NOEXP
{
    return m_name.c_str();
}

bool
thread::pimpl::equal_to (pthread_t _t) KNGIN_NOEXP
{
    return ::pthread_equal(_t, m_thr);
}

void *
thread::pimpl::start (void *_args) KNGIN_NOEXP
{
    assert(_args);
    thread_err_code _code;
    bool _crash = true;

    auto _data = static_cast<thread_data *>(_args);
    pthread_cleanup_push(thread::pimpl::cleanup, _args);
    try {
        if (_data->fn)
            _code.code = _data->fn();
        _crash = false;
    } catch (const k::exception &_e) {
        log_fatal("caught an exception in thread \"%s\", %s",
                  _data->name.c_str(), _e.what());
        log_dump(_e.dump().c_str());
    } catch (const std::exception &_e) {
        log_fatal("caught an exception in thread \"%s\", %s",
                  _data->name.c_str(), _e.what());
    } catch (...) {
        log_fatal("caught an undefined exception in thread \"%s\"",
                  _data->name.c_str());
    }
    if (_crash) {
        assert(0);
        if (_crash_handler)
            _crash_handler(thread::ptid());
    }
    pthread_cleanup_pop(1);
    return _code.ptr;
}

void
thread::pimpl::cleanup (void *_args) KNGIN_NOEXP
{
    auto _data = static_cast<thread_data *>(_args);
    safe_release(_data);
}

/*
void
thread::pimpl::process ()
{
    // thread *_p = (thread *)_args;
    // pthread_cleanup_push()
    // pthread_setcancelstate()
    // pthread_setcanceltype()
    // pthread testcancel()
    // pthread_cleanup_pop()
    return 0;
}
*/

uint64_t
thread::tid () KNGIN_NOEXP
{
#ifdef _WIN32
    return ::GetCurrentThreadId();
#else
    return ::syscall(SYS_gettid);
#endif
}

pthread_t
thread::ptid () KNGIN_NOEXP
{
    return ::pthread_self();
}

bool
thread::equal (pthread_t _thr1, pthread_t _thr2) KNGIN_NOEXP
{
    return ::pthread_equal(_thr1, _thr2);
}

void
thread::sleep (timestamp _ms) KNGIN_NOEXP
{
#ifdef _WIN32
    ::Sleep((DWORD)_ms.value_uint());
#else
    ::usleep(_ms.value_uint() * 1000);
#endif
}

KNGIN_NAMESPACE_K_END
