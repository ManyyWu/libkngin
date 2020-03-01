#ifndef KNGIN_RWLOCK_H
#define KNGIN_RWLOCK_H

#ifdef _WIN32
#include "pthread.h"
#else
#include <pthread.h>
#endif
#include "core/base/define.h"
#include "core/base/timestamp.h"
#include "core/base/noncopyable.h"

KNGIN_NAMESPACE_K_BEGIN

class rwlock : public noncopyable {
public:
    rwlock        ();

    ~rwlock       () noexcept;

public:
    void
    rdlock        ();

    void
    wrlock        ();

    bool
    tryrdlock     ();

    bool
    trywrlock     ();

    bool
    timedrdlock   (timestamp ms);

    bool
    timedwrlock   (timestamp ms);

    void
    unlock        ();

private:
    pthread_rwlock_t rwlock_;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_RWLOCK_H */