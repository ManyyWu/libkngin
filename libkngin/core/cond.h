#ifndef _COND_H_
#define _COND_H_

#ifdef _WIN32
#include "pthreads_win32/include/pthread.h"
#else
#include <pthread.h>
#endif
#include "core/define.h"
#include "core/timestamp.h"
#include "core/mutex.h"

KNGIN_NAMESPACE_K_BEGIN

class cond {
public:
    explicit
    cond           (mutex *_mutex);

    ~cond          () KNGIN_NOEXP;

public:
    void
    wait           ();

    bool
    timedwait      (timestamp _ms);

    void
    signal         ();

    void
    broadcast      ();

private:
    pthread_cond_t m_cond;

    mutex *        m_mutex;
};

KNGIN_NAMESPACE_K_END

#endif /* _COND_H_ */