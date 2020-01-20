#ifndef KNGIN_COND_H
#define KNGIN_COND_H

#ifdef _WIN32
#include "pthreads_win32/include/pthread.h"
#else
#include <pthread.h>
#endif
#include "core/base/define.h"
#include "core/base/timestamp.h"
#include "core/base/noncopyable.h"
#include "core/base/mutex.h"

KNGIN_NAMESPACE_K_BEGIN

class cond : public noncopyable {
public:
    explicit
    cond           (mutex *_mutex);

    ~cond          () KNGIN_NOEXCP;

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

#endif /* KNGIN_COND_H */