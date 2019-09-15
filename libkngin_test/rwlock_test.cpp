#include <cstdio>
#include <unistd.h>
#include <time.h>
#include <atomic>
#include <mutex>
#include "../libkngin/core/thread.h"
#include "../libkngin/core/lock.h"

using namespace k;

static rwlock *         g_rwlock = NULL;

static unsigned int
process1 (void *_args)
{
    for (int i = 0; i < 10; i++) {
        g_rwlock->wrlock();
        fprintf(stderr, "process1: wrlock\n");
        fflush(stderr);
        usleep(1000000);
        g_rwlock->unlock();
        fprintf(stderr, "process1: unlock\n");
        fflush(stderr);
    }
}

static unsigned int
process2 (void *_args)
{
    for (int i = 0; i < 10; i++) {
        g_rwlock->rdlock();
        fprintf(stderr, "----process2: rdlock\n");
        fflush(stderr);
        usleep(1000000);
        g_rwlock->unlock();
        fprintf(stderr, "----process2: unlock\n");
        fflush(stderr);
    }
}

static unsigned int
process3 (void *_args)
{
    for (int i = 0; i < 10; i++) {
        g_rwlock->rdlock();
        fprintf(stderr, "--------process3: rdlock\n");
        fflush(stderr);
        usleep(1000000);
        g_rwlock->unlock();
        fprintf(stderr, "--------process3: unlock\n");
        fflush(stderr);
    }
}

void
rwlock_test ()
{
    g_rwlock = rwlock::create();
    thread t1(process1, NULL);
    thread t2(process2, NULL);
    thread t3(process3, NULL);
    t1.run();
    t2.run();
    t3.run();
    t1.join(NULL);
    t2.join(NULL);
    t3.join(NULL);
    g_rwlock->release();

}
