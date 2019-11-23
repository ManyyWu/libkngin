#ifndef _COND_H_
#define _COND_H_

#ifdef _WIN32
#include "pthread.h"
#else
#include <pthread.h>
#endif
#include "define.h"
#include "timestamp.h"
#include "mutex.h"

KNGIN_NAMESPACE_K_BEGIN

class cond {
public:
    explicit
    cond           (mutex *_mutex);

    ~cond          ();

public:
    void
    wait           ();

    bool
    timedwait      (timestamp _ms);

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

KNGIN_NAMESPACE_K_END

#endif /* _COND_H_ */