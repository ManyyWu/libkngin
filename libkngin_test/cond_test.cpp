#include <cstdio>
#include "../libkngin/core/base/thread.h"
#include "../libkngin/core/base/lock.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin_test/cond_test.cpp"

using namespace k;

static mutex  g_mutex;
static cond   g_cond(&g_mutex);

static int
process1 ()
{
    g_mutex.lock();
    g_cond.wait();
    ::fputs("process1\n", stderr);
    g_mutex.unlock();
    g_mutex.lock();
    g_cond.wait();
    ::fputs("process1\n", stderr);
    g_mutex.unlock();
    return 0;
}

static int
process2 ()
{
    thread::sleep(100);
    g_mutex.lock();
    ::fputs("process2\n", stderr);
    g_mutex.unlock();
    g_cond.signal();
    thread::sleep(100);
    g_mutex.lock();
    ::fputs("process2\n", stderr);
    g_mutex.unlock();
    g_cond.signal();
    return 0;
}

extern void
cond_test ()
{
    thread t1("");
    thread t2("");
    t1.run(process1);
    t2.run(process2);
    t1.join();
    t2.join();
}