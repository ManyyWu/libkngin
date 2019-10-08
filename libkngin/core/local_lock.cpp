#include "define.h"
#include "mutex.h"
#include "local_lock.h"

__NAMESPACE_BEGIN

local_lock::local_lock(mutex &_m)
    : m_mutex(_m)
{
    m_mutex.lock();
}

local_lock::~local_lock()
{
    m_mutex.unlock();
}

__NAMESPACE_END
