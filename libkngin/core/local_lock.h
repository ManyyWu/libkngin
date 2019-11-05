#ifndef _LOCAL_LOCK_
#define _LOCAL_LOCK_

#include "define.h"
#include "lock.h"

__NAMESPACE_BEGIN

class local_lock {
public:
    explicit
    local_lock  (mutex &_m) : m_mutex(_m) { m_mutex.lock(); }

    ~local_lock ()                        { m_mutex.unlock(); }

protected:
    mutex &m_mutex;
};

__NAMESPACE_END

#endif /* _LOCAL_LOCK_ */