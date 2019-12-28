#ifdef _WIN32
#include "pthreads_win32/include/pthread.h"
#else
#include <pthread.h>
#endif
#include "core/common.h"
#include "core/timestamp.h"
#include "core/mutex.h"
#include "core/cond.h"
#include "core/exception.h"
#include "core/system_error.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin/core/cond.cpp"

KNGIN_NAMESPACE_K_BEGIN

cond::cond (mutex *_mutex)
    : m_cond(PTHREAD_COND_INITIALIZER),
      m_mutex(_mutex)
{
}

cond::~cond () KNGIN_NOEXCP
{
    ignore_excp(
        auto _ec = int2ec(::pthread_cond_destroy(&m_cond));
        if (_ec)
            log_fatal("::pthread_cond_destroy() error, %s",
                      system_error_str(_ec).c_str());
    );
}

void
cond::wait ()
{
    auto _ec = int2ec(::pthread_cond_wait(&m_cond, &m_mutex->m_mutex));
    if (_ec) {
        log_fatal("::pthread_cond_wait() error, %s",
                  system_error_str(_ec).c_str());
        throw k::exception("::pthread_cond_wait() error");
    }
}

bool
cond::timedwait (timestamp _ms)
{
    timespec _ts;
    ::timespec_get(&_ts, TIME_UTC);
    timestamp _time = _ts;
    (_time += _ms).to_timespec(_ts);
    auto _ec = int2ec(::pthread_cond_timedwait(&m_cond, &m_mutex->m_mutex, &_ts));
    if (std::errc::timed_out == _ec)
        return false;
    if (_ec) {
        log_fatal("::pthread_cond_timedwait() error, %s",
                  system_error_str(_ec).c_str());
        throw k::exception("::pthread_cond_timedwait() error");
    }
    return true;
}

void
cond::signal ()
{
    auto _ec = int2ec(::pthread_cond_signal(&m_cond));
    if (_ec) {
        log_fatal("::pthread_cond_signal() error, %s",
                  system_error_str(_ec).c_str());
        throw k::exception("::pthread_cond_signal() error");
    }
}

void
cond::broadcast ()
{
    auto _ec = int2ec(::pthread_cond_broadcast(&m_cond));
    if (_ec) {
        log_fatal("::pthread_cond_broadcast() error, %s",
                  system_error_str(_ec).c_str());
        throw k::exception("::pthread_cond_broadcast() error");
    }
}

KNGIN_NAMESPACE_K_END
