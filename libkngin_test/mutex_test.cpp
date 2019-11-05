#include <cstdio>
#include <ctime>
#include <atomic>
#include <mutex>
#include "define.h"
#include "thread.h"
#include "common.h"
#include "lock.h"

#ifdef __FILENAME__
#undef __FILENAME__
#endif
#define __FILENAME__ "libkngin_test/mutex_test.cpp"

using namespace k;

#define THR_NUM 10

static int              g_num1(0);
static int              g_num2(0);
static std::atomic<int> g_num3(0);
static mutex            g_mutex;
static std::mutex       g_std_mutex;

static int
process_mutex ()
{
    for (int i = 0; i < 1000000; i++) {
        g_mutex.lock();
        g_num1 += 1;
        g_mutex.unlock();
    }
    return 0;
}

static int
process_std_mutex ()
{
    for (int i = 0; i < 1000000; i++) {
        g_std_mutex.lock();
        g_num2 += 1;
        g_std_mutex.unlock();
    }
    return 0;
}

static int
process_atomic ()
{
    for (int i = 0; i < 1000000; i++) {
        g_num3 += 1;
    }
    return 0;
}

void
mutex_test ()
{
    ::fprintf(stderr, "waiting...\n");
    /* lock */
    thread * thrs[THR_NUM];
    timespec ts1;
    ::timespec_get(&ts1, TIME_UTC);

    for (int i = 0; i < THR_NUM; ++i) {
        knew(thrs[i], thread, (""));
        thrs[i]->run(process_mutex);
    }

    for (int i = 0; i < THR_NUM; ++i) {
        thrs[i]->join(nullptr);
        kdelete(thrs[i]);
    }

    timespec ts2;
    ::timespec_get(&ts2, TIME_UTC);
    ::fprintf(stderr, "--- k::mutex ---\n time use: %lfms, result = %d\n",
           (ts2.tv_sec - ts1.tv_sec ) * 1000 + (ts2.tv_nsec - ts1.tv_nsec) / 1000000.0,
           g_num1);

    /* std::mutex */
    ::timespec_get(&ts1, TIME_UTC);

    for (int i = 0; i < THR_NUM; ++i) {
        knew(thrs[i], thread, (""));
        thrs[i]->run(process_std_mutex);
    }

    for (int i = 0; i < THR_NUM; ++i) {
        thrs[i]->join(nullptr);
        kdelete(thrs[i]);
    }

    ::timespec_get(&ts2, TIME_UTC);
    ::fprintf(stderr, "--- std::mutex ---\n time use: %lfms, result = %d\n",
           (ts2.tv_sec - ts1.tv_sec) * 1000 +  (ts2.tv_nsec - ts1.tv_nsec) / 1000000.0,
           g_num2);

    /* atomic */
    ::timespec_get(&ts1, TIME_UTC);

    for (int i = 0; i < THR_NUM; ++i) {
        knew(thrs[i], thread, (""));
        thrs[i]->run(process_atomic);
    }

    for (int i = 0; i < THR_NUM; ++i) {
        thrs[i]->join(nullptr);
        kdelete(thrs[i]);
    }

    ::timespec_get(&ts2, TIME_UTC);
    ::fprintf(stderr, "--- atomic ---\n time use: %lfms, result = %d\n",
           (ts2.tv_sec - ts1.tv_sec) * 1000 + (ts2.tv_nsec - ts1.tv_nsec) / 1000000.0, g_num3.load());
}