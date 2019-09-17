#include <cstdio>
#include <time.h>
#include <atomic>
#include <mutex>
#include "../libkngin/core/thread.h"
#include "../libkngin/core/lock.h"

using namespace k;

#define THR_NUM 10

static int              g_num1(0);
static int              g_num2(0);
static std::atomic<int> g_num3(0);
static mutex *          g_mutex = NULL;
static std::mutex       g_std_mutex;

static unsigned int
process_mutex (void *_args)
{
    for (int i = 0; i < 1000000; i++) {
        g_mutex->lock();
        g_num1 += 1;
        g_mutex->unlock();
    }
    return 0;
}

static unsigned int
process_std_mutex (void *_args)
{
    for (int i = 0; i < 1000000; i++) {
        g_std_mutex.lock();
        g_num2 += 1;
        g_std_mutex.unlock();
    }
    return 0;
}

static unsigned int
process_atomic (void *_args)
{
    for (int i = 0; i < 1000000; i++) {
        g_num3 += 1;
    }
    return 0;
}

void
mutex_test ()
{
    printf("waiting...\n");
    /* lock */
    thread * thrs[THR_NUM];
    timespec ts1;
    timespec_get(&ts1, TIME_UTC);
    g_mutex = mutex::create();

    for (int i = 0; i < THR_NUM; ++i) {
        thrs[i] = new thread(process_mutex, NULL);
        thrs[i]->run();
    }

    for (int i = 0; i < THR_NUM; ++i) {
        thrs[i]->join(NULL);
        delete thrs[i];
    }
    g_mutex->release();

    timespec ts2;
    timespec_get(&ts2, TIME_UTC);
    printf("--- k::mutex ---\n time use: %lfms, result = %d\n",
           (ts2.tv_sec - ts1.tv_sec ) * 1000 + (ts2.tv_nsec - ts1.tv_nsec) / 1000000.0,
           g_num1);

    /* std::mutex */
    timespec_get(&ts1, TIME_UTC);

    for (int i = 0; i < THR_NUM; ++i) {
        thrs[i] = new thread(process_std_mutex, NULL);
        thrs[i]->run();
    }

    for (int i = 0; i < THR_NUM; ++i) {
        thrs[i]->join(NULL);
        delete thrs[i];
    }

    timespec_get(&ts2, TIME_UTC);
    printf("--- std::mutex ---\n time use: %lfms, result = %d\n",
           (ts2.tv_sec - ts1.tv_sec) * 1000 +  (ts2.tv_nsec - ts1.tv_nsec) / 1000000.0,
           g_num2);

    /* atomic */
    timespec_get(&ts1, TIME_UTC);

    for (int i = 0; i < THR_NUM; ++i) {
        thrs[i] = new thread(process_atomic, NULL);
        thrs[i]->run();
    }

    for (int i = 0; i < THR_NUM; ++i) {
        thrs[i]->join(NULL);
        delete thrs[i];
    }

    timespec_get(&ts2, TIME_UTC);
    printf("--- atomic ---\n time use: %lfms, result = %d\n",
           (ts2.tv_sec - ts1.tv_sec) * 1000 + (ts2.tv_nsec - ts1.tv_nsec) / 1000000.0,
           g_num3.load());
}