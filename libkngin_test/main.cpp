#include <iostream>
#include <exception>
#include "../libkngin/core/common.h"
#include "../libkngin/core/exception.h"
#include "../libkngin/core/system_error.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin_test/main.cpp"

using namespace std;

extern void
test ();

extern void
log_test ();

extern void
buffer_test ();

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

extern void
sockopts_test ();

extern void
socket_test ();

extern void
tcp_connection_test ();

extern void
io_threadpool_test ();

void
event_loop_test ();

extern void
tcp_server_test ();

extern void
simple_ftp_server_test ();

void
test ();

int main()
{
    try {
        cerr << "********************* test *****************************\n";
        test();
        cerr << "********************************************************\n";

//        cerr << "********************* log_test *************************\n";
//        log_test();
//        cerr << "********************************************************\n";
//
//        cerr << "********************* buffer_test **********************\n";
//        buffer_test();
//        cerr << "********************************************************\n";
//
//        cerr << "********************* thread_test **********************\n";
//        thread_test();
//        cerr << "********************************************************\n";
//
//        cerr << "********************* mutex_test ***********************\n";
//        mutex_test();
//        cerr << "********************************************************\n";
//
//        cerr << "********************* rwlock_test **********************\n";
//        rwlock_test();
//        cerr << "********************************************************\n";
//
//        cerr << "********************* cond_test ************************\n";
//        cond_test();
//        cerr << "********************************************************\n";
//
//        cerr << "********************* deque_test ***********************\n";
//        //sync_deque_test();
//        cerr << "********************************************************\n";
//
//        cerr << "********************* queue_test ***********************\n";
//        //sync_queue_test();
//        cerr << "********************************************************\n";
//
//        cerr << "********************* work_thread_test *****************\n";
//        //work_thread_test();
//        cerr << "********************************************************\n";
//
//        cerr << "********************* threadpool_test ******************\n";
//        //threadpool_test();
//        cerr << "********************************************************\n";
//
//        cerr << "********************* sock_opts_test *******************\n";
//        sockopts_test();
//        cerr << "********************************************************\n";
//
//        cerr << "********************* socket_test test *****************\n";
//        socket_test();
//        cerr << "********************************************************\n";
//
//        cerr << "********************* io_threadpool_test ***************\n";
//        io_threadpool_test ();
//        cerr << "********************************************************\n";
//
//        cerr << "********************* event_loop_test ******************\n";
//        event_loop_test ();
//        cerr << "********************************************************\n";

        cerr << "********************* tcp_server_test ******************\n";
        tcp_server_test ();
        cerr << "********************************************************\n";
    } catch (const k::exception &_e) {
        log_fatal("caught an exception %s", _e.what());
        log_dump(_e.dump().c_str());
    } catch (const std::exception &_e) {
        log_fatal("caught an exception %s", _e.what());
    } catch (...) {
        log_fatal("caught an undefined exception");
    }
#ifdef _WIN32
getchar();
#endif
return 0;
}
