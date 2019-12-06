#include <iostream>
#include "../libkngin/core/logfile.h"
#include "../libkngin/net/io_threadpool.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin_test/io_threadpoll_test.cpp"

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
    _pool.start(nullptr);

    for (int i = 0; i < 100; ++i)
        _pool.add_task(func);

    thread::sleep(5000);
}
