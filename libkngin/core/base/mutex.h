#ifndef KNGIN_MUTEX_H
#define KNGIN_MUTEX_H

#ifdef _WIN32
#include "pthread.h"
#else
#include <pthread.h>
#endif
#include "core/base/define.h"
#include "core/base/timestamp.h"
#include "core/base/noncopyable.h"

KNGIN_NAMESPACE_K_BEGIN

class mutex : public noncopyable {
    friend class cond;

public:
    mutex         ();

    ~mutex        () KNGIN_NOEXCP;

public:
    void
    lock          ();

    void
    unlock        ();

    bool
    trylock       ();

    bool
    timedlock     (timestamp _ms);

private:
    pthread_mutex_t m_mutex;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_MUTEX_H */
