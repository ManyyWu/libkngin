#ifdef _WIN32
#include "pthread.h"
#include "windows.h"
#else
#include <pthread.h>
#include <unistd.h>
#endif
#include "define.h"
#include "common.h"
#include "error.h"
#include "timestamp.h"
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
    : m_name(_name ? _name : ""), m_thr(pthread_t{nullptr, 0}), m_tid(thread::tid())
      m_err_code(nullptr), m_running(false), m_joined(false), m_fn(nullptr)
#else
thread::thread (const char *_name)
    : m_name(_name ? _name : ""), m_thr(0), m_tid(thread::tid()),
      m_err_code(), m_running(false), m_joined(false), m_fn(nullptr)
#endif
{
    check(_name);
}

thread::~thread ()
{
    int _ret = 0;
    
    if (!m_joined) {
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
    check_r0(!equal_to(self()));

    int _ret = ::pthread_join(m_thr, &m_err_code.ptr);
    if_not (!_ret) {
        log_fatal("::pthread_join(), name = \"%s\"return %d - %s", m_name.c_str(), _ret, strerror(_ret));
        return false;
    }
    if (_err_code)
        *_err_code = m_err_code.code;
    m_joined = true;
#ifdef _WIN32
    m_thr = pthread_t{nullptr, 0};
#else
    m_thr = 0;
#endif
    log_info("thread \"%s\" has joined with code: %d", m_name.c_str(), m_err_code.code);
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
    check_r0(!equal_to(self()));

    int _ret = 0;
    _ret = ::pthread_cancel(m_thr);
    if_not (!_ret) {
        log_fatal("::pthread_cancel(), name = \"%s\"return %d - %s", m_name.c_str(), _ret, strerror(_ret));
        return false;
    }
    log_info("thread \"%s\" cancel", m_name.c_str());
    return true;
}

void *
thread::start (void *_args)
{
    check_r0(_args);

    thread *_p = (thread *)_args;
    _p->m_running = true;

    try {
        pthread_cleanup_push(thread::cleanup, _args);
        if (_p->m_fn)
            _p->set_err_code(_p->m_fn());
        pthread_cleanup_pop(1);
    } catch (const k::exception &e){
        log_fatal("caught an k::exception in thread \"%s\": %s\n%s",
                  _p->name(), e.what().c_str(), e.dump().c_str());
    } catch (const std::exception &e) {
        log_fatal("caught an std::exception in thread \"%s\": %s", _p->name(), e.what());
    } catch (...) {
        log_fatal("caught an undefined exception in thread \"%s\"", _p->name());
    }

    return _p->m_err_code.ptr;
}

void
thread::cleanup (void *_args)
{
    check_r(_args);

    thread *_p = (thread *)_args;
    _p->m_running = false;
}

//int
//thread::process ()
//{
//    // thread *_p = (thread *)_args;
//    // pthread_cleanup_push()
//    // pthread_setcancelstate()
//    // pthread_setcanceltype()
//    // pthread testcancel()
//    // pthread_cleanup_pop()
//    return 0;
//}

__NAMESPACE_END
