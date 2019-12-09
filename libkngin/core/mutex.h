#ifndef _MUTEX_H_
#define _MUTEX_H_

#ifdef _WIN32
#include "pthreads_win32/include/pthread.h"
#else
#include <pthread.h>
#endif
#include "core/define.h"
#include "core/timestamp.h"
#include "core/noncopyable.h"

KNGIN_NAMESPACE_K_BEGIN

class mutex : public noncopyable {
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

private:
    friend class cond;
};

KNGIN_NAMESPACE_K_END

#endif /* _MUTEX_H_ */
