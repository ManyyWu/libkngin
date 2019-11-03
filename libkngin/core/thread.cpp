#ifdef _WIN32
#include "pthread.h"
#include "windows.h"
#else
#include <pthread.h>
#include <unistd.h>
#endif
#include "define.h"
#include "error.h"
#include "exception.h"
#include "thread.h"
#include "logfile.h"

#ifdef __FILENAME__
#undef __FILENAME__
#endif
#define __FILENAME__ "libkngin/core/thread.cpp"

__NAMESPACE_BEGIN

#ifdef _WIN32
thread::thread (const char *_name /* = "" */)
    : m_name(_name ? _name : ""), m_thr(pthread_t{nullptr, 0}),
      m_retptr(nullptr), m_running(false), m_fn(nullptr)
#else
thread::thread (const char *_name)
    : m_name(_name ? _name : ""), m_thr(0), m_retptr(nullptr),
      m_running(false), m_fn(nullptr)
#endif
{
    check(_name);
}

thread::~thread ()
{
    int _ret = 0;
    
#ifdef _WIN32
    if (m_thr.p) {
#else
    if (m_thr) {
#endif
        _ret = ::pthread_detach(m_thr);
        if_not (!_ret)
            log_fatal("::pthread_detach(), name = \"%s\", return %d - %s", m_name.c_str(), _ret, strerror(_ret));
        else
            log_info("thread \"%s\" detached", m_name.c_str());
    }
}

bool
thread::run (thr_fn &&_fn)
{
#ifdef _WIN32
    check_r0(!m_thr.p);
#else
    check_r0(!m_thr);
#endif

    m_fn = std::move(_fn);
    int _ret = ::pthread_create(&m_thr, nullptr, thread::start, this);
    if_not (!_ret)
        log_fatal("::pthread_create(), name = \"%s\", return %d - %s", m_name.c_str(), _ret, strerror(_ret));
    log_info("thread \"%s\" running", m_name.c_str());
    return !_ret;
}

bool
thread::join (int *_err_code)
{
#ifdef _WIN32
    check_r0(m_thr.p);
#else
    check_r0(m_thr);
#endif
    check_r0(!::pthread_equal(m_thr, pthread_self()));

    int _ret = 0;
    _ret = ::pthread_join(m_thr, &m_retptr);
    if_not (!_ret) {
        log_fatal("::pthread_join(), name = \"%s\"return %d - %s", m_name.c_str(), _ret, strerror(_ret));
        return false;
    }
    if (_err_code)
        *_err_code = (int)(long long)m_retptr;
#ifdef _WIN32
    m_thr = pthread_t{nullptr, 0};
#else
    m_thr = 0;
#endif
    log_info("thread \"%s\" joined with code: %d", m_name.c_str(), (int)(long long)m_retptr);
    return true;
}

bool
thread::cancel ()
{
#ifdef _WIN32
    check_r0(m_thr.p);
#else
    check_r0(m_thr);
#endif
    check_r0(!pthread_equal(m_thr, pthread_self()));

    int _ret = 0;
    _ret = ::pthread_cancel(m_thr);
    if_not (!_ret) {
        log_fatal("::pthread_cancel(), name = \"%s\"return %d - %s", m_name.c_str(), _ret, strerror(_ret));
        return false;
    }
    log_info("thread \"%s\" cancel", m_name.c_str());
    return true;
}

bool
thread::running () const
{
    return m_running;
}

pthread_t
thread::get_interface () const
{
    return m_thr;
}

int
thread::get_err_code  () const
{
    return (int)(long long)m_retptr;
}

uint64_t
thread::get_tid ()
{
#ifdef _WIN32
    return ::GetCurrentThreadId();
#else
    return ::getpid();
#endif
}

const char *
thread::name () const
{
    return m_name.c_str();
}

void
thread::sleep (timestamp _ms)
{
#ifdef _WIN32
    ::Sleep((DWORD)_ms.value_uint());
#else
    ::usleep(_ms.value_uint() * 1000);
#endif
}

void
thread::exit (int _err_code)
{
    ::pthread_exit((void *)(long long)_err_code);
}

pthread_t
thread::self ()
{
    return ::pthread_self();
}

void
thread::set_err_code (int _err_code)
{
    m_retptr = (void *)(long long)_err_code;
}

bool
thread::equal_to (pthread_t _t)
{
    return ::pthread_equal(_t, m_thr);
}

void *
thread::start (void *_args)
{
    check_r0(_args);

    thread *_p = (thread *)_args;
    _p->m_running = true;

    try {
        pthread_cleanup_push(thread::cleanup, _args);

        _p->set_err_code(_p->m_fn(_p->m_args));

        pthread_cleanup_pop(1);
    } catch (const k::exception &e){
        log_fatal("caught an k::exception in thread \"%s\": %s\n%s",
                  _p->name(), e.what().c_str(), e.dump().c_str());
    } catch (const std::exception &e) {
        log_fatal("caught an std::exception in thread \"%s\": %s", _p->name(), e.what());
    } catch (...) {
        log_fatal("caught an undefined exception in thread \"%s\"", _p->name());
    }

    return _p->m_retptr;
}

void
thread::cleanup (void *_args)
{
    check_r(_args);

    thread *_p = (thread *)_args;
    _p->m_running = false;
}

int
thread::process (void *_args)
{
    // thread *_p = (thread *)_args;
    // pthread_cleanup_push()
    // pthread_setcancelstate()
    // pthread_setcanceltype()
    // pthread testcancel()
    // pthread_cleanup_pop()
    return 0;
}

__NAMESPACE_END
