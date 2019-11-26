#ifdef _WIN32
#include "pthread.h"
#else
#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>
#endif
#include "common.h"
#include "exception.h"
#include "thread.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin/core/thread.cpp"

KNGIN_NAMESPACE_K_BEGIN

thread::pimpl::pimpl ()
        : m_name(""),
#ifdef _WIN32
        m_thr({nullptr, 0}),
#else
        m_thr(0),
#endif
        m_joined(false)
{
}

thread::pimpl::pimpl (const char *_name)
    : m_name(_name ? _name : ""),
#ifdef _WIN32
      m_thr({nullptr, 0}),
#else
      m_thr(0),
#endif
      m_joined(false)
{
}

thread::pimpl::~pimpl ()
{
    if (!m_joined) {
        int _ret = ::pthread_detach(m_thr);
        if (_ret)
            log_fatal("::pthread_detach(), name = \"%s\", return %d - %s",
                      m_name.c_str(), _ret, strerror(_ret));
        else
            log_info("thread \"%s\" has detached", m_name.c_str());
    }
}

void
thread::pimpl::run (thr_fn &&_fn)
{
    int _ret = ::pthread_create(&m_thr, nullptr, thread::pimpl::start,
                                new thread_data(m_name, std::move(_fn)));
    if (_ret)
        log_fatal("::pthread_create(), name = \"%s\", return %d - %s",
                  m_name.c_str(), _ret, strerror(_ret));
    else
        log_info("thread \"%s\" is running", m_name.c_str());
    return !_ret;
}

bool
thread::pimpl::join ()
{
#warning "throw or return std::error_code"
    check(!equal_to(ptid()));

    thread_err_code _code;
    int _ret = ::pthread_join(m_thr, &_code.ptr);
    if (_ret) {
        log_fatal("::pthread_join(), name = \"%s\"return %d - %s",
                  m_name.c_str(), _ret, strerror(_ret));
        m_joined = true;
        return nullptr;
    }
    int *_err_code = nullptr;
    if (_err_code)
        *_err_code = _code.code;
    m_joined = true;
    log_info("thread \"%s\" has joined with code: %d", m_name.c_str(), _code.code);
    return _err_code;
}

bool
thread::pimpl::cancel ()
{
    check(!equal_to(ptid()));

    int _ret = ::pthread_cancel(m_thr);
    if (_ret)
        log_fatal("::pthread_cancel(), name = \"%s\"return %d - %s",
                  m_name.c_str(), _ret, strerror(_ret));
    else
        log_info("thread \"%s\" cancel", m_name.c_str());
    return !_ret;
}

uint64_t
thread::pimpl::tid ()
{
#ifdef _WIN32
    return ::GetCurrentThreadId();
#else
    return ::syscall(SYS_gettid);
#endif
}

pthread_t
thread::pimpl::ptid ()
{
    return ::pthread_self();
}

void
thread::pimpl::sleep (timestamp _ms)
{
#ifdef _WIN32
    ::Sleep((DWORD)_ms.value_uint());
#else
    ::usleep(_ms.value_uint() * 1000);
#endif
}

void
thread::pimpl::exit (int _err_code)
{
    ::pthread_exit(thread_err_code(_err_code).ptr);
}

bool
thread::pimpl::equal_to (pthread_t _t)
{
    return ::pthread_equal(_t, m_thr);
}

void *
thread::pimpl::start (void *_args)
{
    check(_args);
    pthread_cleanup_push(thread::pimpl::cleanup, _args);
    thread_data * _data = (thread_data *)_args;

    try {
        if (_data->fn)
            _data->fn();
    } catch (const k::system_error &_e) {
        log_error("caught an std::system_error in thread \"%s\": \"%s:%d\"",
                  _data->name.c_str(), _e.what(), _e.code());
    } catch (const k::exception &_e){
        log_fatal("caught an k::exception in thread \"%s\": %s",
                  _data->name.c_str(), _e.what());
        log_dump(_e.dump());
    } catch (const std::bad_alloc &_e) {
        exit(1);
    } catch (const std::exception &_e) {
        log_fatal("caught an std::exception in thread \"%s\"",
                  _data->name.c_str(), _e.what());
    } catch (...) {
        log_fatal("caught an undefined exception in thread \"%s\"",
                  _data->name.c_str());
    }

    pthread_cleanup_pop(1);
    return 0;
}

void
thread::pimpl::cleanup (void *_args)
{
    thread_data * _data = (thread_data *)_args;
    safe_release(_data);
}

//int
//thread::pimpl::process ()
//{
//    // thread *_p = (thread *)_args;
//    // pthread_cleanup_push()
//    // pthread_setcancelstate()
//    // pthread_setcanceltype()
//    // pthread testcancel()
//    // pthread_cleanup_pop()
//    return 0;
//}

KNGIN_NAMESPACE_K_END
