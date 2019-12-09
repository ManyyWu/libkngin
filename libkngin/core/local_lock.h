#ifndef _LOCAL_LOCK_
#define _LOCAL_LOCK_

#include "core/define.h"
#include "core/lock.h"
#include "core/noncopyable.h"

KNGIN_NAMESPACE_K_BEGIN

class local_lock : public noncopyable {
public:
    explicit
    local_lock  (mutex &_m)
        : m_mutex(_m) { m_mutex.lock(); }

    ~local_lock ()
    { m_mutex.unlock(); }

protected:
    mutex &m_mutex;
};

KNGIN_NAMESPACE_K_END

#endif /* _LOCAL_LOCK_ */