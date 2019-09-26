#ifndef _COND_H_
#define _COND_H_

#ifdef _WIN32
#include "pthread.h"
#else
#include <pthread.h>
#endif
#include "time.h"
#include "error.h"
#include "define.h"
#include "logfile.h"
#include "common.h"
#include "thread.h"
#include "mutex.h"
#include "noncopyable.h"

__NAMESPACE_BEGIN

class cond : noncopyable {
public:
    cond          () = delete;

    cond          (cond &&_cond);

protected:
    cond          (mutex *_mutex, pthread_cond_t *_cond_intr);

protected:
    ~cond         ();

public:
    static cond *
    create        (mutex *_mutex);

    void
    release       ();

public:
    bool
    wait          ();

    bool
    timedwait     (time_t _ms);

    bool
    signal        ();

    bool
    broadcast     ();

public:
    pthread_cond_t *
    get_interface () const;

protected:
    pthread_cond_t *m_cond;

    mutex *         m_mutex;
};

__NAMESPACE_END

#endif /* _COND_H_ */