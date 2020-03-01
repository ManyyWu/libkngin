#ifdef _WIN32
#include "pthread.h"
#else
#include <pthread.h>
#endif
#include "core/base/common.h"
#include "core/base/timestamp.h"
#include "core/base/barrier.h"
#include "core/base/exception.h"
#include "core/base/system_error.h"

#define KNGIN_FILENAME "libkngin/core/base/barrier.cpp"

KNGIN_NAMESPACE_K_BEGIN

barrier::barrier (int count)
    try
    : inited_(true)
{
    auto ec = ::pthread_barrier_init(&barrier_, nullptr, count);
    if (ec) {
        log_fatal("::pthread_barrier_init() error, %s",
                  system_error_str(CERR(ec)).c_str());
        throw k::exception("::pthread_barrier_init() error");
    }
} catch (...) {
    log_fatal("barrier::barrier() error");
    throw;
}

barrier::~barrier ()
{
    if (inited_) {
        ignore_excp(destroy());
        log_warning("undestroyed barrier");
    }
}

void
barrier::reinit (int count)
{
    assert(!inited_);
    auto ec = ::pthread_barrier_init(&barrier_, nullptr, count);
    if (ec) {
        log_fatal("::pthread_barrier_init() error, %s",
                  system_error_str(CERR(ec)).c_str());
        throw k::exception("::pthread_barrier_init() error");
    }
    inited_ = true;
}

bool
barrier::wait ()
{
    assert(inited_);
    auto ret = ::pthread_barrier_wait(&barrier_);
    if (0 == ret)
        return false;
    if (PTHREAD_BARRIER_SERIAL_THREAD == ret)
        return true;
    if (ret) {
        log_fatal("::pthread_barrier_wait() error, %s",
                  system_error_str(CERR(ret)).c_str());
        throw k::exception("::pthread_barrier_wait() error");
    }
    return true;
}

void
barrier::destroy ()
{
    assert(inited_);
    inited_ = false;
    auto ec = ::pthread_barrier_destroy(&barrier_);
    if (ec) {
        log_fatal("::pthread_barrier_destroy() error, %s",
                  system_error_str(CERR(ec)).c_str());
        throw k::exception("::pthread_barrier_destroy() error");
    }
}

KNGIN_NAMESPACE_K_END
