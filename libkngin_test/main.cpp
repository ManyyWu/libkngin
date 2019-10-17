#include <iostream>

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

void
test ();

int main()
{
    try {
        cerr << "********************* log_test *************************\n";
        log_test();
        cerr << "********************************************************\n";

        cerr << "********************* buffer_test **********************\n";
        buffer_test();
        cerr << "********************************************************\n";

        cerr << "********************* thread_test **********************\n";
        thread_test();
        cerr << "********************************************************\n";

        cerr << "********************* mutex_test ***********************\n";
        mutex_test();
        cerr << "********************************************************\n";

        cerr << "********************* rwlock_test **********************\n";
        rwlock_test();
        cerr << "********************************************************\n";

        cerr << "********************* cond_test ************************\n";
        cond_test();
        cerr << "********************************************************\n";

        cerr << "********************* deque_test ***********************\n";
        sync_deque_test();
        cerr << "********************************************************\n";

        cerr << "********************* queue_test ***********************\n";
        sync_queue_test();
        cerr << "********************************************************\n";

        cerr << "********************* work_thread_test *****************\n";
        work_thread_test();
        cerr << "********************************************************\n";

        cerr << "********************* threadpool_test ******************\n";
        threadpool_test();
        cerr << "********************************************************\n";

        cerr << "************************* test *****************************\n";
      test();
        cerr << "************************************************************\n";

    } catch (std::exception &e) {
        cerr << e.what() << endl;
    } catch (...) {
        cout << "crash!\n";
    }

#ifdef _WIN32
    getchar();
#endif
    return 0;
}

// 去除knew kdelete
// raii
// 构造函数throw
// 加explicit
// class () delete
// 云除 copyable noncopyable
// 返回值
// inline
// 重新考虑kassert if_not throw
// 检查日志
// timestamp
