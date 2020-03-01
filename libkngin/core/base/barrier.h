#ifndef KNGIN_BARRIER_H
#define KNGIN_BARRIER_H

#ifdef _WIN32
#include "pthread.h"
#else
#include <pthread.h>
#endif
#include <atomic>
#include "core/base/define.h"
#include "core/base/timestamp.h"
#include "core/base/noncopyable.h"
#include "core/base/mutex.h"

KNGIN_NAMESPACE_K_BEGIN

class barrier : public noncopyable {
public:
    explicit
    barrier        (int _count);

    ~barrier       () noexcept;

public:
    void
    reinit         (int _count);

    bool
    wait           ();

    void
    destroy        ();

    bool
    destroyed      () const noexcept
    { return !m_inited; }

private:
    pthread_barrier_t m_barrier;

    std::atomic_bool  m_inited;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_BARRIER_H */