#include <iostream>
#include "define.h"
#include "io_threadpool.h"

#ifdef __FILENAME__
#undef __FILENAME__
#endif
#define __FILENAME__ "libkngin_test/io_threadpoll_test.cpp"

using namespace k;

static void
func ()
{
    log_info("test");
}

extern void
io_threadpool_test ()
{
    io_threadpool _pool(10);
    _pool.start();

    for (int i = 0; i < 100; ++i)
        _pool.add_task(func);

    thread::sleep(5000);
}
