#ifndef KNGIN_BARRIER_H
#define KNGIN_BARRIER_H

#ifdef _WIN32
#include "pthreads_win32/include/pthread.h"
#else
#include <pthread.h>
#endif
#include <atomic>
#include "core/define.h"
#include "core/timestamp.h"
#include "core/noncopyable.h"
#include "core/mutex.h"

KNGIN_NAMESPACE_K_BEGIN

class barrier : public noncopyable {
public:
    explicit
    barrier        (int _count);

    ~barrier       () KNGIN_NOEXCP;

public:
    void
    reinit         (int _count);

    bool
    wait           ();

    void
    destroy        ();

    bool
    destroyed      () const KNGIN_NOEXCP
    { return !m_inited; }

private:
    pthread_barrier_t m_barrier;

    std::atomic<bool> m_inited;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_BARRIER_H */