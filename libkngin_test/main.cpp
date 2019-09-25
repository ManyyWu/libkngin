#include <cstdio>

extern void
log_test ();

extern void
thread_test ();

extern void
mutex_test ();

extern void
rwlock_test ();

extern void
cond_test ();

extern void
sync_deque_test ();

extern void
sync_queue_test ();

extern void
work_thread_test ();

extern void
threadpool_test ();

void
test ();

int main()
{
    fprintf(stderr, "**************************************************\n");
    fprintf(stderr, "* log_test                                       *\n");
    fprintf(stderr, "**************************************************\n");
    log_test();

    fprintf(stderr, "**************************************************\n");
    fprintf(stderr, "* thread_test                                    *\n");
    fprintf(stderr, "**************************************************\n");
    thread_test();

    fprintf(stderr, "**************************************************\n");
    fprintf(stderr, "* mutex_test                                     *\n");
    fprintf(stderr, "**************************************************\n");
    mutex_test();

    fprintf(stderr, "**************************************************\n");
    fprintf(stderr, "* rwlock_test                                    *\n");
    fprintf(stderr, "**************************************************\n");
    rwlock_test();

    fprintf(stderr, "**************************************************\n");
    fprintf(stderr, "* cond_test                                      *\n");
    fprintf(stderr, "**************************************************\n");
    cond_test();

    fprintf(stderr, "**************************************************\n");
    fprintf(stderr, "* deque_test                                     *\n");
    fprintf(stderr, "**************************************************\n");
    sync_deque_test();

    fprintf(stderr, "**************************************************\n");
    fprintf(stderr, "* queue_test                                     *\n");
    fprintf(stderr, "**************************************************\n");
    sync_queue_test();

    fprintf(stderr, "**************************************************\n");
    fprintf(stderr, "* work_thread_test                               *\n");
    fprintf(stderr, "**************************************************\n");
    work_thread_test();

    fprintf(stderr, "**************************************************\n");
    fprintf(stderr, "* threadpool_test                                *\n");
    fprintf(stderr, "**************************************************\n");
    threadpool_test();

    fprintf(stderr, "**************************************************\n");
    fprintf(stderr, "* test                                           *\n");
    fprintf(stderr, "**************************************************\n");
    test();

#ifdef _WIN32
    getchar();
#endif
    return 0;
}

////////////////////// test //////////////////////

#include <atomic>
#include <typeinfo>
#include <mutex>
#include "../libkngin/core/logfile.h"
#include "../libkngin/core/thread.h"
#include "../libkngin/core/lock.h"

using namespace k;

void
test ()
{
// 队列，**
// search comments
};

////////////////////// test //////////////////////
