#include <cstdio>
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
    static int
    process1 (void *_args)
    {
        mythread *_p = (mythread *)_args;
        fprintf(stderr, "process1\n");

        return 0;
    }

    static int
    process2 (void *_args)
    {
        mythread *_p = (mythread *)_args;
        fprintf(stderr, "process1\n");

        return 0;
    }
};

extern void
thread_test ()
{
    int ret;

    thread t1(thread::process, NULL);
    t1.run();
    t1.join(&ret);
    fprintf(stderr, "join: %d\n", ret);

    thread t2(thread::process, NULL);
    t2.run();
    t2.join(&ret);
    fprintf(stderr, "join: %d\n", ret);

    mythread thr1(mythread::process1);
    mythread thr2(mythread::process2);
    thr1.run();
    thr2.run();

    thr1.join(&ret);
    fprintf(stderr, "join: %d\n", ret);
    thr2.join(&ret);
    fprintf(stderr, "join: %d\n", ret);
}
