#include "define.h"
#include "io_thread.h"
#include "common.h"

#ifdef __FILENAME__
#undef __FILENAME__
#endif
#define __FILENAME__ "libkngin/net/work_thread.cpp"

__NAMESPACE_BEGIN

io_thread::io_thread (const char *_name,
                      loop_started_cb &&_started_cb,
                      loop_stopped_cb &&_stopped_cb)
    try
    : thread(_name), m_loop(new event_loop(this)),
      m_started_cb(std::move(_started_cb)),
      m_stopped_cb(std::move(_stopped_cb))
{
} catch (...) {
    delete m_loop;
    throw;
}

io_thread::~io_thread ()
{
    if (this->running()) {
        this->cancel();
        this->join(nullptr);
    }
}

__NAMESPACE_END
