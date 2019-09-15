#include <cstdio>
#include <unistd.h>
#include "../libkngin/core/thread.h"
#include "../libkngin/core/lock.h"

using namespace k;

class mythread : public thread {
public:
    mythread (pthr_fn _pfn)
        : thread(_pfn, this)
    {
    }

    virtual
    ~mythread ()
    {
    }

public:
    static unsigned int
    process1 (void *_args)
    {
        mythread *_p = (mythread *)_args;
        fprintf(stderr, "process1[%ld]\n", _p->get_interface());
        fflush(stderr);
        _p->sleep(1000);

        return 0;
    }

    static unsigned int
    process2 (void *_args)
    {
        mythread *_p = (mythread *)_args;
        fprintf(stderr, "process1[%ld]\n", _p->get_interface());
        fflush(stderr);
        _p->sleep(1000);

        return 0;
    }
};

extern void
thread_test ()
{
    unsigned int ret;

    thread t1(thread::process, &t1);
    t1.run();
    t1.join(&ret, 0);
    fprintf(stderr, "join: %d\n", ret);
    fflush(stderr);

    thread t2(thread::process, &t1);
    t2.run();
    t2.join(&ret, 0);
    fprintf(stderr, "join: %d\n", ret);
    fflush(stderr);

    mythread thr1(mythread::process1);
    mythread thr2(mythread::process2);
    thr1.run();
    thr2.run();

    thr1.join(&ret);
    fprintf(stderr, "join: %d\n", ret);
    fflush(stderr);
    thr2.join(&ret);
    fprintf(stderr, "join: %d\n", ret);
    fflush(stderr);
}
