#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include <cstring>
#include <memory>
#include "common.h"
#include "timestamp.h"
#include "system_error.h"
#include "exception.h"
#include "thread.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin/core/thread.cpp"

KNGIN_NAMESPACE_K_BEGIN

thread::thread (const char *_name)
    : m_name(_name ? _name : ""),
#ifdef _WIN32
      m_thr({nullptr, 0}),
#else
      m_thr(0),
#endif
      m_joined(false)
{
}

thread::~thread ()
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

bool
thread::run (thr_fn &&_fn)
{
    int _ret = ::pthread_create(&m_thr, nullptr, thread::start,
                                new thread_data(m_name, std::move(_fn)));
    if (_ret)
        log_fatal("::pthread_create(), name = \"%s\", return %d - %s",
                  m_name.c_str(), _ret, strerror(_ret));
    else
        log_info("thread \"%s\" is running", m_name.c_str());
    return !_ret;
}

bool
thread::join (int *_err_code)
{
    check(!equal_to(self()));

    thread_err_code _code;
    int _ret = ::pthread_join(m_thr, &_code.ptr);
    if (_ret) {
        log_fatal("::pthread_join(), name = \"%s\"return %d - %s",
                  m_name.c_str(), _ret, strerror(_ret));
        m_joined = true;
        return false;
    }
    if (_err_code)
        *_err_code = _code.code;
    m_joined = true;
    log_info("thread \"%s\" has joined with code: %d", m_name.c_str(), _code.code);
    return true;
}

bool
thread::cancel ()
{
    check(!equal_to(self()));

    int _ret = ::pthread_cancel(m_thr);
    if (_ret)
        log_fatal("::pthread_cancel(), name = \"%s\"return %d - %s",
                  m_name.c_str(), _ret, strerror(_ret));
    else
        log_info("thread \"%s\" cancel", m_name.c_str());
    return !_ret;
}

void *
thread::start (void *_args)
{
    check(_args);
    pthread_cleanup_push(thread::cleanup, _args);
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
thread::cleanup (void *_args)
{
    thread_data * _data = (thread_data *)_args;
    safe_release(_data);
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

KNGIN_NAMESPACE_K_END
