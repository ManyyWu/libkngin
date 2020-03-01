#ifndef KNGIN_COND_H
#define KNGIN_COND_H

#ifdef _WIN32
#include "pthread.h"
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
    cond           (mutex *mutex);

    ~cond          () noexcept;

public:
    void
    wait           ();

    bool
    timedwait      (timestamp ms);

    void
    signal         ();

    void
    broadcast      ();

private:
    pthread_cond_t cond_;

    mutex *        mutex_;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_COND_H */