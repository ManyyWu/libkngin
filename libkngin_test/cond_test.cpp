#include <cstdio>
#include "thread.h"
#include "lock.h"

using namespace k;

static mutex  g_mutex;
static cond   g_cond(&g_mutex);

static int
process1 (void *_args)
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
process2 (void *_args)
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
    t1.join(NULL);
    t2.join(NULL);
}