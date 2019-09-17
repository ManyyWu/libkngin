#include <cstdio>
#include <ctime>
#include <atomic>
#include <mutex>
#include "../libkngin/core/thread.h"
#include "../libkngin/core/lock.h"

using namespace k;

static rwlock *         g_rwlock = NULL;
static mutex  *         g_mutex = NULL;

#define print_err(_fmt, ...) do {        \
    g_mutex->lock();                     \
    fprintf(stderr, _fmt, ##__VA_ARGS__);\
    fflush(stderr);                      \
    g_mutex->unlock();                   \
} while (false)

static int
process1 (void *_args)
{
    for (int i = 0; i < 10; i++) {
        g_rwlock->wrlock();
        print_err("process1: wrlock\n");
        g_rwlock->wrunlock();
        print_err("process1: unlock\n");
    }
    return 0;
}

static int
process2 (void *_args)
{
    for (int i = 0; i < 10; i++) {
        g_rwlock->rdlock();
        print_err("----process2: rdlock\n");
        g_rwlock->rdunlock();
        print_err("----process2: unlock\n");
    }
    return 0;
}

static int
process3 (void *_args)
{
    for (int i = 0; i < 10; i++) {
        g_rwlock->rdlock();
        print_err("--------process3: rdlock\n");
        g_rwlock->rdunlock();
        print_err("--------process3: unlock\n");
    }
    return 0;
}

void
rwlock_test ()
{
    g_rwlock = rwlock::create();
    g_mutex = mutex::create();
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
    g_mutex->release();

}
