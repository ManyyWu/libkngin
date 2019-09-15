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
class rwlock_test_thread : public thread {
public:
    rwlock_test_thread (pthr_fn _pfn, void *_args)
    : thread(_pfn, _args)
    {
    }

    virtual
    ~rwlock_test_thread ()
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
            m_rwlock->rdlock();
            for (char *_i = _buf1; *_i != '\0'; ++_i)
                sprintf(&rwlock_test_thread::m_buf[_i - _buf1], "%c", *_i);
            fputs(rwlock_test_thread::m_buf, stderr);
            m_rwlock->unlock();
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
            m_rwlock->rdlock();
            for (char *_i = _buf2; *_i != '\0'; ++_i)
                sprintf(&rwlock_test_thread::m_buf[_i - _buf2], "%c", *_i);
            fputs(rwlock_test_thread::m_buf, stderr);
            m_rwlock->unlock();
        }
        return 0;
    }

private:
    static rwlock *m_rwlock;

    static char    m_buf[4096];
};

rwlock *rwlock_test_thread::m_rwlock = rwlock::create();
char    rwlock_test_thread::m_buf[4096] = {0};

extern void
rwlock_test ()
{
    unsigned int ret;

    rwlock_test_thread thr1(rwlock_test_thread::process1, _buf1);
    rwlock_test_thread thr2(rwlock_test_thread::process2, _buf2);
    thr1.run();
    thr2.run();

    thr1.join(&ret);
    fprintf(stderr, "join: %d\n", ret);
    fflush(stderr);
    thr2.join(&ret);
    fprintf(stderr, "join: %d\n", ret);
    fflush(stderr);
}