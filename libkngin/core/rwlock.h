#ifndef _RWLOCK_H_
#define _RWLOCK_H_

#ifdef _WIN32
#include "pthread.h"
#else
#include <pthread.h>
#endif
#include "define.h"
#include "thread.h"
#include "noncopyable.h"

__NAMESPACE_BEGIN

class rwlock : noncopyable {
public:
    rwlock        () = delete;

    rwlock        (pthread_rwlock_t *_rwlock_intr);

protected:
    ~rwlock       ();

public:
    static rwlock *
    create        ();

    void
    release       ();

public:
    bool
    rdlock        ();

    bool
    wrlock        ();

    bool
    tryrdlock     ();

    bool
    trywrlock     ();

    bool
    timedrdlock   (time_t _ms);

    bool
    timedwrlock   (time_t _ms);

    bool
    rdunlock      ();

    bool
    wrunlock      ();

public:
    const pthread_rwlock_t *
    get_interface () const;

protected:
    pthread_rwlock_t *m_rwlock;
};

__NAMESPACE_END

#endif /* _RWLOCK_H_ */