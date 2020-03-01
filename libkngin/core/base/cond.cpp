#ifdef WIN32
#include "pthread.h"
#else
#include <pthread.h>
#endif
#include "core/base/common.h"
#include "core/base/timestamp.h"
#include "core/base/mutex.h"
#include "core/base/cond.h"
#include "core/base/exception.h"
#include "core/base/system_error.h"

#define KNGIN_FILENAME "libkngin/core/base/cond.cpp"

KNGIN_NAMESPACE_K_BEGIN

cond::cond (mutex *mutex)
    : cond_(PTHREAD_COND_INITIALIZER),
      mutex_(mutex)
{
}

cond::~cond () noexcept
{
    ignore_excp(
        auto ec = ::pthread_cond_destroy(&cond_);
        if (ec)
            log_fatal("::pthread_cond_destroy() error, %s",
                      system_error_str(CERR(ec)).c_str());
    );
}

void
cond::wait ()
{
    auto ec = ::pthread_cond_wait(&cond_, &mutex_->mutex_);
    if (ec) {
        log_fatal("::pthread_cond_wait() error, %s",
                  system_error_str(CERR(ec)).c_str());
        throw k::exception("::pthread_cond_wait() error");
    }
}

bool
cond::timedwait (timestamp ms)
{
    timespec ts;
    ::timespec_get(&ts, TIME_UTC);
    timestamp time = ts;
    (time += ms).to_timespec(ts);
    auto ec = ::pthread_cond_timedwait(&cond_, &mutex_->mutex_, &ts);
    if (ETIMEDOUT == ec)
        return false;
    if (ec) {
        log_fatal("::pthread_cond_timedwait() error, %s",
                  system_error_str(CERR(ec)).c_str());
        throw k::exception("::pthread_cond_timedwait() error");
    }
    return true;
}

void
cond::signal ()
{
    auto ec = ::pthread_cond_signal(&cond_);
    if (ec) {
        log_fatal("::pthread_cond_signal() error, %s",
                  system_error_str(CERR(ec)).c_str());
        throw k::exception("::pthread_cond_signal() error");
    }
}

void
cond::broadcast ()
{
    auto ec = ::pthread_cond_broadcast(&cond_);
    if (ec) {
        log_fatal("::pthread_cond_broadcast() error, %s",
                  system_error_str(CERR(ec)).c_str());
        throw k::exception("::pthread_cond_broadcast() error");
    }
}

KNGIN_NAMESPACE_K_END
