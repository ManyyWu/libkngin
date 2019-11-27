#ifndef _MUTEX_H_
#define _MUTEX_H_

#ifdef _WIN32
#include "pthreads_win32/include/pthread.h"
#else
#include <pthread.h>
#endif
#include "core/define.h"
#include "core/timestamp.h"

KNGIN_NAMESPACE_K_BEGIN

class mutex {
public:
    mutex         ();

    ~mutex        ();

public:
    void
    lock          ();

    void
    unlock        ();

    bool
    trylock       ();

    bool
    timedlock     (timestamp _ms);

    pthread_mutex_t *
    get_interface ();

protected:
    pthread_mutex_t m_mutex;

protected:
    friend class cond;
};

KNGIN_NAMESPACE_K_END

#endif /* _MUTEX_H_ */
