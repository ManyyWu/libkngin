#ifndef KNGIN_RWLOCK_H
#define KNGIN_RWLOCK_H

#ifdef _WIN32
#include "pthreads_win32/include/pthread.h"
#else
#include <pthread.h>
#endif
#include "core/define.h"
#include "core/timestamp.h"
#include "core/noncopyable.h"

KNGIN_NAMESPACE_K_BEGIN

class rwlock : public noncopyable {
public:
    rwlock        ();

    ~rwlock       () KNGIN_NOEXCP;

public:
    void
    rdlock        ();

    void
    wrlock        ();

    bool
    tryrdlock     ();

    bool
    trywrlock     ();

    bool
    timedrdlock   (timestamp _ms);

    bool
    timedwrlock   (timestamp _ms);

    void
    unlock        ();

private:
    pthread_rwlock_t m_rwlock;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_RWLOCK_H */