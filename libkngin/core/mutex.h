#ifndef _MUTEX_H_
#define _MUTEX_H_

#ifdef _WIN32
#include "pthread.h"
#else
#include <pthread.h>
#endif
#include "timestamp.h"
#include "define.h"
#include "noncopyable.h"

__NAMESPACE_BEGIN

class mutex : noncopyable {
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
    timedlock     (time_t _ms);

    pthread_mutex_t *
    get_interface ();

protected:
    pthread_mutex_t m_mutex;

protected:
    friend class cond;
};

__NAMESPACE_END

#endif /* _MUTEX_H_ */
