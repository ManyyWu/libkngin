#include <cstdio>
#include <ctime>
#include <atomic>
#include <mutex>
#include "thread.h"
#include "lock.h"

using namespace k;

static rwlock           g_rwlock;
static mutex            g_mutex;

#define print_err(_fmt, ...) do {        \
    g_mutex.lock();                     \
    ::fprintf(stderr, _fmt, ##__VA_ARGS__);\
    g_mutex.unlock();                   \
} while (false)

static int
process1 (void *_args)
{
    for (int i = 0; i < 10; i++) {
        g_rwlock.wrlock();
        print_err("process1: wrlock\n");
        g_rwlock.unlock();
        print_err("process1: unlock\n");
    }
    return 0;
}

static int
process2 (void *_args)
{
    for (int i = 0; i < 10; i++) {
        g_rwlock.rdlock();
        print_err("----process2: rdlock\n");
        g_rwlock.unlock();
        print_err("----process2: unlock\n");
    }
    return 0;
}

static int
process3 (void *_args)
{
    for (int i = 0; i < 10; i++) {
        g_rwlock.rdlock();
        print_err("--------process3: rdlock\n");
        g_rwlock.unlock();
        print_err("--------process3: unlock\n");
    }
    return 0;
}

void
rwlock_test ()
{
    thread t1(process1, NULL);
    thread t2(process2, NULL);
    thread t3(process3, NULL);
    t1.run();
    t2.run();
    t3.run();
    t1.join(NULL);
    t2.join(NULL);
    t3.join(NULL);

}
