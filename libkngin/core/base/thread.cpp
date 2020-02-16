#ifdef _WIN32
#include "pthread.h"
#else
#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>
#endif
#include "core/base/common.h"
#include "core/base/system_error.h"
#include "core/base/exception.h"
#include "core/base/thread.h"

#define KNGIN_FILENAME "libkngin/core/base/thread.cpp"

KNGIN_NAMESPACE_K_BEGIN

thread::thread ()
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
    log_fatal("thread() error");
    throw;
}

thread::thread (const char *_name)
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
    log_fatal("thread() error");
    throw;
}

thread::~thread () KNGIN_NOEXCP
{
#ifdef _WIN32
    if (m_joined or !m_thr.p or !m_thr.x)
#else
    if (m_joined or !m_thr)
#endif
        return;
    auto _ec = int2ec(::pthread_detach(m_thr));
    if (_ec)
        log_fatal("::pthread_detach() error, name = \"%s\", %s",
                   m_name.c_str(), system_error_str(_ec).c_str());
    else
        log_info("thread \"%s\" has detached", m_name.c_str());
}

void
thread::run (thr_fn &&_fn, crash_handler &&_crash_handler /* = nullptr */)
{
    assert(_fn);

    auto _ec = int2ec(::pthread_create(
                          &m_thr, nullptr,
                          thread::start,
                          new thread::thread_data(
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
}

int
thread::join ()
{
    assert(!equal_to(ptid()));

    thread_err_code _code;
    auto _ec = int2ec(::pthread_join(m_thr, &_code.ptr));
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
thread::cancel ()
{
    assert(!equal_to(ptid()));

    auto _ec = int2ec(::pthread_cancel(m_thr));
    if (_ec) {
        log_fatal("::pthread_cancel(), name = \"%s\", %s",
                  m_name.c_str(), system_error_str(_ec).c_str());
        throw k::exception("::pthread_cancel() error");
    }
    log_info("thread \"%s\" cancelled", m_name.c_str());
}

void *
thread::start (void *_args) KNGIN_NOEXCP
{
    assert(_args);
    thread_err_code _code;
    bool _crash = true;

    auto _data = static_cast<thread_data *>(_args);
    assert(_data);
    pthread_cleanup_push(thread::cleanup, _args);
    try {
        log_info("thread \"%s\" is running, tid = %" PRIu64, _data->name.c_str(), thread::tid());
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
        //assert(0);
        if (_data->handler)
            _data->handler(thread::ptid());
    }
    pthread_cleanup_pop(1);
    return _code.ptr;
}

void
thread::cleanup (void *_args) KNGIN_NOEXCP
{
    assert(_args);
    auto _data = static_cast<thread_data *>(_args);
    safe_release(_data);
}

uint64_t
thread::tid () KNGIN_NOEXCP
{
#ifdef _WIN32
    return ::GetCurrentThreadId();
#else
    return ::syscall(SYS_gettid);
#endif
}

pthread_t
thread::ptid () KNGIN_NOEXCP
{
    return ::pthread_self();
}

void
thread::sleep (timestamp _ms) KNGIN_NOEXCP
{
#ifdef _WIN32
    ::Sleep(static_cast<DWORD>(_ms.value_uint()));
#else
    ::usleep(_ms.value_uint() * 1000);
#endif
}

bool
thread::equal_to (pthread_t _t) KNGIN_NOEXCP
{
    return ::pthread_equal(_t, m_thr);
}

bool
thread::equal (pthread_t _thr1, pthread_t _thr2) KNGIN_NOEXCP
{
    return ::pthread_equal(_thr1, _thr2);
}

void
thread::exit (int _err_code) KNGIN_NOEXCP
{
    ::pthread_exit(thread::thread_err_code(_err_code).ptr);
}

KNGIN_NAMESPACE_K_END
