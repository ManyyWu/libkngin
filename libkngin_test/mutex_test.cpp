#include <cstdio>
#include <unistd.h>
#include "../libkngin/core/thread.h"
#include "../libkngin/core/lock.h"

using namespace k;

static char _buf1[] = "|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
//                      "|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
//                      "|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
                      "\n";

static char _buf2[] = "======================================================================================="
//                      "======================================================================================="
//                      "======================================================================================="
                      "\n";

class mutex_test_thread : public thread {
public:
    mutex_test_thread (pthr_fn _pfn)
    : thread(_pfn, this)
    {
    }

    virtual
    ~mutex_test_thread ()
    {
    }

public:
    static unsigned int
    process1 (void *_args)
    {
        assert(_args);
        char *_p = (char *)_args;

        int _n = 10;
        while (_n--) {
            m_mutex->lock();
            for (char *_i = _buf1; *_i != '\0'; ++_i)
                fputc(*_i, stderr);
            fflush(stderr);
            m_mutex->unlock();
            mutex_test_thread::sleep(100);
        }
        return 0;
    }

    static unsigned int
    process2 (void *_args)
    {
        assert(_args);
        char *_p = (char *)_args;

        int _n = 10;
        while (_n--) {
            m_mutex->lock();
            for (char *_i = _buf2; *_i != '\0'; ++_i)
                fputc(*_i, stderr);
            fflush(stderr);
            m_mutex->unlock();
            mutex_test_thread::sleep(100);
        }
        return 0;
    }

private:
    static mutex *m_mutex;
};

mutex *mutex_test_thread::m_mutex = mutex::create();

extern void
mutex_test ()
{
    unsigned int _ret;

    mutex_test_thread thr1(mutex_test_thread::process1);
    mutex_test_thread thr2(mutex_test_thread::process2);
    thr1.run();
    thr2.run();

    thr1.join(&_ret);
    fprintf(stderr, "join: %d\n", _ret);
    fflush(stderr);
    thr2.join(&_ret);
    fprintf(stderr, "join: %d\n", _ret);
    fflush(stderr);
}
