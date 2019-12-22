#ifdef _WIN32
#include "pthreads_win32/include/pthread.h"
#else
#include <pthread.h>
#endif
#include "core/common.h"
#include "core/timestamp.h"
#include "core/mutex.h"
#include "core/barrier.h"
#include "core/exception.h"
#include "core/system_error.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin/core/barrier.cpp"

KNGIN_NAMESPACE_K_BEGIN

barrier::barrier (int _count)
    try
    : m_inited(true)
{
    std::error_code _ec = int2ec(pthread_barrier_init(&m_barrier, nullptr, _count));
    if (_ec) {
        log_fatal("::pthread_barrier_init() error, %s",
                  system_error_str(_ec).c_str());
        throw k::exception("::pthread_barrier_init() error");
    }
} catch (...) {
    log_fatal("barrier::barrier() error");
    throw;
}

barrier::~barrier ()
{
    if (m_inited) {
        ignore_excp(destroy());
        log_warning("undestroyed barrier");
    }
}

void
barrier::reinit (int _count)
{
    assert(!m_inited);
    std::error_code _ec = int2ec(pthread_barrier_init(&m_barrier, nullptr, _count));
    if (_ec) {
        log_fatal("::pthread_barrier_init() error, %s",
                  system_error_str(_ec).c_str());
        throw k::exception("::pthread_barrier_init() error");
    }
    m_inited = true;
}

bool
barrier::wait ()
{
    assert(m_inited);
    int _ret = ::pthread_barrier_wait(&m_barrier);
    if (0 == _ret)
        return false;
    if (PTHREAD_BARRIER_SERIAL_THREAD == _ret)
        return true;
    std::error_code _ec = int2ec(_ret);
    if (_ec) {
        log_fatal("::pthread_barrier_wait() error, %s",
                  system_error_str(_ec).c_str());
        throw k::exception("::pthread_barrier_wait() error");
    }
}

void
barrier::destroy ()
{
    assert(m_inited);
    m_inited = false;
    std::error_code _ec = int2ec(::pthread_barrier_destroy(&m_barrier));
    if (_ec) {
        log_fatal("::pthread_barrier_destroy() error, %s",
                  system_error_str(_ec).c_str());
        throw k::exception("::pthread_barrier_destroy() error");
    }
}

KNGIN_NAMESPACE_K_END
