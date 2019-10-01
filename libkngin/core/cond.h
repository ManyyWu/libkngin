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
    cond           (mutex *_m);

    ~cond          ();

public:
    void
    wait           ();

    bool
    timedwait      (time_t _ms);

    void
    signal         ();

    void
    broadcast      ();

    pthread_cond_t *
    get_interface  ();

protected:
    pthread_cond_t m_cond;

    mutex *        m_mutex;
};

__NAMESPACE_END

#endif /* _COND_H_ */