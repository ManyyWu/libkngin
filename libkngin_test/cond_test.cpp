#include <cstdio>
#include <unistd.h>
#include "../libkngin/core/thread.h"
#include "../libkngin/core/lock.h"

using namespace k;

static mutex *g_mutex = NULL;
static cond * g_cond = NULL;

static unsigned int
process1 (void *_args)
{
    g_mutex->lock();
    g_cond->wait();
    fputs("process1\n", stderr);
    fflush(stderr);
    g_mutex->unlock();
    g_mutex->lock();
    g_cond->wait();
    fputs("process1\n", stderr);
    fflush(stderr);
    g_mutex->unlock();
    return 0;
}

static unsigned int
process2 (void *_args)
{
    usleep(2000000);
    g_mutex->lock();
    fputs("process2\n", stderr);
    fflush(stderr);
    g_mutex->unlock();
    g_cond->signal();
    usleep(2000000);
    g_mutex->lock();
    fputs("process2\n", stderr);
    fflush(stderr);
    g_mutex->unlock();
    g_cond->signal();
    return 0;
}

extern void
cond_test ()
{
    g_mutex = mutex::create();
    g_cond = cond::create(g_mutex);
    thread t1(process1, NULL);
    thread t2(process2, NULL);
    t1.run();
    t2.run();
    t1.join(NULL);
    t2.join(NULL);
    g_cond->release();
    g_mutex->release();
}