#ifdef _WIN32
#include "pthread.h"
#else
#include <pthread.h>
#endif
#include "core/base/common.h"
#include "core/base/timestamp.h"
#include "core/base/mutex.h"
#include "core/base/exception.h"
#include "core/base/system_error.h"

#define KNGIN_FILENAME "libkngin/core/base/mutex.cpp"

KNGIN_NAMESPACE_K_BEGIN

mutex::mutex ()
    : mutex_(PTHREAD_MUTEX_INITIALIZER)
{
}

mutex::~mutex ()
{
    ignore_excp(
        auto ec = ::pthread_mutex_destroy(&mutex_);
        if (ec)
            log_fatal("::pthread_mutex_destroy() error, %s",
                      system_error_str(CERR(ec)).c_str());
    );
}

void
mutex::lock ()
{
    auto ec = ::pthread_mutex_lock(&mutex_);
    if (ec) {
        log_fatal("::pthread_mutex_lock() error, %s",
                  system_error_str(CERR(ec)).c_str());
        throw k::exception("::pthread_mutex_lock() error");
    }
}

bool
mutex::trylock ()
{
    auto ec = ::pthread_mutex_trylock(&mutex_);
    if (EBUSY == ec)
        return false;
    if (ec) {
        log_fatal("::pthread_mutex_trylock() error, %s",
                  system_error_str(CERR(ec)).c_str());
        throw k::exception("::pthread_mutex_trylock() error");
    }
    return true;
}

bool
mutex::timedlock (timestamp ms)
{
    timespec ts;
    ::timespec_get(&ts, TIME_UTC);
    timestamp time = ts;
    (time += ms).to_timespec(ts);
    auto ec = ::pthread_mutex_timedlock(&mutex_, &ts);
    if (ETIMEDOUT == ec)
        return false;
    if (ec) {
        log_fatal("::pthread_mutex_timedlock() error, %s",
                  system_error_str(CERR(ec)).c_str());
        throw k::exception("::pthread_mutex_timedlock() error");
    }
    return true;
}

void
mutex::unlock ()
{
    auto ec = ::pthread_mutex_unlock(&mutex_);
    if (ec) {
        log_fatal("::pthread_mutex_unlock() error, %s",
                  system_error_str(CERR(ec)).c_str());
        throw k::exception("::pthread_mutex_unlock() error");
    }
}

KNGIN_NAMESPACE_K_END
