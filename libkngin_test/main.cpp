#include <iostream>

using namespace std;

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
    try {
//        cerr << "**************************************************\n";
//        cerr << "* log_test                                       *\n";
//        cerr << "**************************************************\n";
//        log_test();
//
//        cerr << "**************************************************\n";
//        cerr << "* thread_test                                    *\n";
//        cerr << "**************************************************\n";
//        thread_test();
//
//        cerr << "**************************************************\n";
//        cerr << "* mutex_test                                     *\n";
//        cerr << "**************************************************\n";
//        mutex_test();
//
//        cerr << "**************************************************\n";
//        cerr << "* rwlock_test                                    *\n";
//        cerr << "**************************************************\n";
//        rwlock_test();
//
//        cerr << "**************************************************\n";
//        cerr << "* cond_test                                      *\n";
//        cerr << "**************************************************\n";
//        cond_test();
//
//        cerr << "**************************************************\n";
//        cerr << "* deque_test                                     *\n";
//        cerr << "**************************************************\n";
//        sync_deque_test();
//
//        cerr << "**************************************************\n";
//        cerr << "* queue_test                                     *\n";
//        cerr << "**************************************************\n";
//        sync_queue_test();
//
//        cerr << "**************************************************\n";
//        cerr << "* work_thread_test                               *\n";
//        cerr << "**************************************************\n";
//        work_thread_test();
//
//        cerr << "**************************************************\n";
//        cerr << "* threadpool_test                                *\n";
//        cerr << "**************************************************\n";
//        threadpool_test();

        cerr << "**************************************************\n";
        cerr << "* test                                           *\n";
        cerr << "**************************************************\n";
        test();

    } catch (...) {
        cout << "crash!\n";
    }

#ifdef _WIN32
    getchar();
#endif
    return 0;
}

////////////////////// test //////////////////////

#include <atomic>
#include <typeinfo>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <string>
#include <memory>
#include "../libkngin/core/logfile.h"
#include "../libkngin/core/thread.h"
#include "../libkngin/core/lock.h"
#include "../libkngin/core/memory.h"

using namespace k;

void
test ()
{
    uint8_uptr p = make_shared<uint8_t[]>(100);
    shared_ptr<uint8_t> p1 = make_shared<uint8_t>(10);
    cerr << sizeof(uint8_uptr) << ',' << sizeof(p1) << endl;
}

////////////////////// test //////////////////////
