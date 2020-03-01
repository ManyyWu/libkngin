#ifndef _LOCAL_LOCK_
#define _LOCAL_LOCK_

#include "core/base/define.h"
#include "core/base/lock.h"
#include "core/base/noncopyable.h"

KNGIN_NAMESPACE_K_BEGIN

class local_lock : public noncopyable {
public:
    explicit
    local_lock  (mutex &m)
        : mutex_(m) { mutex_.lock(); }

    ~local_lock ()
    { mutex_.unlock(); }

protected:
    mutex &mutex_;
};

KNGIN_NAMESPACE_K_END

#endif /* _LOCAL_LOCK_ */