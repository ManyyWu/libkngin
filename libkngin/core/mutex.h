#ifndef _MUTEX_H_
#define _MUTEX_H_

#ifdef _WIN32
#include "pthread.h"
#else
#include <pthread.h>
#endif
#include "define.h"
#include "noncopyable.h"

__NAMESPACE_BEGIN

class mutex : noncopyable {
public:
    mutex     () = delete;

    mutex     (pthread_mutex_t *_mutex_intr);

protected:
    ~mutex    ();

public:
    static mutex *
    create    ();

    void
    release   ();

public:
    bool
    lock      ();

    bool
    trylock   ();

    bool
    timedlock (time_t _ms);

    bool
    unlock    ();

public:
    const pthread_mutex_t *
    get_interface () const;

protected:
    pthread_mutex_t *m_mutex;

protected:
    friend class     cond;
};

__NAMESPACE_END

#endif /* _MUTEX_H_ */
