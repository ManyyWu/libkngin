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
//    fprintf(stderr, "**************************************************\n");
//    fprintf(stderr, "* log_test                                       *\n");
//    fprintf(stderr, "**************************************************\n");
//    log_test();
//
//    fprintf(stderr, "**************************************************\n");
//    fprintf(stderr, "* thread_test                                    *\n");
//    fprintf(stderr, "**************************************************\n");
//    thread_test();
//
//    fprintf(stderr, "**************************************************\n");
//    fprintf(stderr, "* mutex_test                                     *\n");
//    fprintf(stderr, "**************************************************\n");
//    mutex_test();
//
//    fprintf(stderr, "**************************************************\n");
//    fprintf(stderr, "* rwlock_test                                    *\n");
//    fprintf(stderr, "**************************************************\n");
//    rwlock_test();
//
//    fprintf(stderr, "**************************************************\n");
//    fprintf(stderr, "* cond_test                                      *\n");
//    fprintf(stderr, "**************************************************\n");
//    cond_test();
//
//    fprintf(stderr, "**************************************************\n");
//    fprintf(stderr, "* deque_test                                     *\n");
//    fprintf(stderr, "**************************************************\n");
//    sync_deque_test();
//
//    fprintf(stderr, "**************************************************\n");
//    fprintf(stderr, "* queue_test                                     *\n");
//    fprintf(stderr, "**************************************************\n");
//    sync_queue_test();
//
//    fprintf(stderr, "**************************************************\n");
//    fprintf(stderr, "* work_thread_test                               *\n");
//    fprintf(stderr, "**************************************************\n");
//    work_thread_test();

    fprintf(stderr, "**************************************************\n");
    fprintf(stderr, "* threadpool_test                                *\n");
    fprintf(stderr, "**************************************************\n");
    threadpool_test();

// others
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
#include "../libkngin/core/logfile.h"
#include "../libkngin/core/thread.h"
#include "../libkngin/core/lock.h"
#include <typeinfo>

using namespace k;

//template <typename __T>
//__T *
//new_debug (__T &&_c, size_t _n = 1)
//{
//
//
//    if (1 == _n) {
//        _mem = new(std::nothrow) __T(_c);
//        memory_debug("serial: %16lx, new(std::nothrow) %s, size: %ldByte",
//                     typeid(__T), sizeof(__T) * _n, ++_serial);
//    } else {
//        _mem = new(std::nothrow) __T[_n](_c);
//        memory_debug("serial: %16ld, new(std::nothrow) %s[%d], size: %ldByte",
//                     typeid(__T), _n, sizeof(__T) * _n, ++_serial);
//    }
//    assert(_mem);
//    return _mem;
//}

static std::atomic<size_t> __serial(0);
void *__addr = NULL;

#define new_debug(__t, __e, __n) \
            ((__addr = new(std::nothrow) __t[__n](__e)),                           \
             memory_debug("serial: %16ld, new(std::nothrow) %s[%d], size: %ldByte",\
                          #__t, (__n), sizeof((__t)) * (__n), ++__serial),         \
             __addr)
void
test ()
{

    if ((__addr = new(std::nothrow) int(10),                                    \
             logger()[k::__LOG_FILE_SERVER]->log_assert("%s,%lld, %lld, %s, %lld",
             __FUNCTION__, __FILE__, __LINE__,                   \
             "int", (1), sizeof(int) * (1), ++__serial),         \
             __addr);
    //new_debug(thread, thread::process, NULL, 10);
// 队列，**
// search comments
};

////////////////////// test //////////////////////
