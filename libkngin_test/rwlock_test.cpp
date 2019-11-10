#include <cstdio>
#include <ctime>
#include <atomic>
#include <mutex>
#include "thread.h"
#include "lock.h"

#ifdef __FILENAME__
#undef __FILENAME__
#endif
#define __FILENAME__ "libkngin_test/rwlock_test.cpp"

using namespace k;

static rwlock           g_rwlock;
static mutex            g_mutex;

#define print_err(_fmt, ...) do {        \
    g_mutex.lock();                     \
    ::fprintf(stderr, _fmt, ##__VA_ARGS__);\
    g_mutex.unlock();                   \
} while (false)

static int
process1 ()
{
    for (int i = 0; i < 10; ++i) {
        g_rwlock.wrlock();
        print_err("process1: wrlock\n");
        g_rwlock.unlock();
        print_err("process1: unlock\n");
    }
    return 0;
}

static int
process2 ()
{
    for (int i = 0; i < 10; ++i) {
        g_rwlock.rdlock();
        print_err("----process2: rdlock\n");
        g_rwlock.unlock();
        print_err("----process2: unlock\n");
    }
    return 0;
}

static int
process3 ()
{
    for (int i = 0; i < 10; ++i) {
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
    thread t1("");
    thread t2("");
    thread t3("");
    t1.run(process1);
    t2.run(process2);
    t3.run(process3);
    t1.join(nullptr);
    t2.join(nullptr);
    t3.join(nullptr);

}
