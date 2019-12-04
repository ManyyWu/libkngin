#ifndef _RWLOCK_H_
#define _RWLOCK_H_

#ifdef _WIN32
#include "pthreads_win32/include/pthread.h"
#else
#include <pthread.h>
#endif
#include "core/define.h"
#include "core/timestamp.h"

KNGIN_NAMESPACE_K_BEGIN

class rwlock {
public:
    rwlock        ();

    ~rwlock       () KNGIN_NOEXP;

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

#endif /* _RWLOCK_H_ */