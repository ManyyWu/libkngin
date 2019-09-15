#include <cstdio>
#include <unistd.h>
#include "../libkngin/core/thread.h"
#include "../libkngin/core/lock.h"

using namespace k;

class cond_test_thread : public thread {
public:
    cond_test_thread (pthr_fn _pfn)
    : thread(_pfn, this)
    {
    }

    virtual
    ~cond_test_thread ()
    {
    }

public:
    static unsigned int
    process1 (void *_args)
    {
        assert(_args);
        char *_buf = (char *)_args;

        m_mutex->lock();
        m_cond->wait();
        fputs("process1\n", stderr);
        fflush(stderr);
        m_mutex->unlock();
        m_mutex->lock();
        m_cond->wait();
        fputs("process1\n", stderr);
        fflush(stderr);
        m_mutex->unlock();
        return 0;
    }

    static unsigned int
    process2 (void *_args)
    {
        assert(_args);
        char *_buf = (char *)_args;

        sleep(1000);
        m_mutex->lock();
        fputs("process2\n", stderr);
        fflush(stderr);
        m_mutex->unlock();
        m_cond->signal();
        sleep(1000);
        m_mutex->lock();
        fputs("process2\n", stderr);
        fflush(stderr);
        m_mutex->unlock();
        m_cond->signal();
        return 0;
    }

private:
    static mutex *m_mutex;
    static cond * m_cond;
};

mutex *cond_test_thread::m_mutex = mutex::create();
cond *cond_test_thread::m_cond = cond::create(m_mutex);

extern void
cond_test ()
{
    unsigned int ret;

    cond_test_thread thr1(cond_test_thread::process1);
    cond_test_thread thr2(cond_test_thread::process2);
    thr1.run();
    thr2.run();

    thr1.join(&ret);
    fprintf(stderr, "join: %d\n", ret);
    fflush(stderr);
    thr2.join(&ret);
    fprintf(stderr, "join: %d\n", ret);
    fflush(stderr);
}