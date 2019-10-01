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

    } catch (bad_alloc &e) {
        cerr << e.what() << endl;
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
#include <cstring>
#include "../libkngin/core/logfile.h"
#include "../libkngin/core/thread.h"
#include "../libkngin/core/lock.h"
#include "../libkngin/core/memory.h"
#include "../libkngin/core/buffer.h"
//#include "../libkngin/core/exception.h"

using namespace k;

void
test ()
{
    try {
        uint8_uarr p = make_uint8_uarray(100);
        buffer b1(std::move(p), 100);
        cerr << b1.size() << endl;
        for (int i = 0; i < 100; ++i)
            b1.write_uint8(i);
        std::string s;
        b1.dump(s);
        cerr << s.c_str() << endl;
        b1.reset(0);
        b1.write_bytes((uint8_t *)"0123456789", 12);
        b1.dump(s);
        cerr << s.c_str() << endl;
    } catch (logic_exception &e) {
        cerr << e.what() << endl;
    } catch (...) {
        throw;
    }
}

////////////////////// test //////////////////////
