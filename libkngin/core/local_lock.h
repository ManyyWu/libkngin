#ifndef _LOCAL_LOCK_
#define _LOCAL_LOCK_

#include "define.h"
#include "mutex.h"

__NAMESPACE_BEGIN

class local_lock {
public:
    local_lock  (mutex &_m);

    ~local_lock ();

protected:
    mutex &m_mutex;
};

__NAMESPACE_END

#endif /* _LOCAL_LOCK_ */