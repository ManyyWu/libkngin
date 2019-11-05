#include <cstdio>
#include "thread.h"
#include "lock.h"

#ifdef __FILENAME__
#undef __FILENAME__
#endif
#define __FILENAME__ "libkngin_test/thread_test.cpp"

using namespace k;

class mythread : public thread {
public:
    mythread ()
        : thread("")
    {
    }

    virtual
    ~mythread ()
    {
    }

public:
    static int
    process1 ()
    {
        ::fprintf(stderr, "process1\n");

        return 0;
    }

    static int
    process2 ()
    {
        ::fprintf(stderr, "process1\n");

        return 0;
    }
};

extern void
thread_test ()
{
    int ret;

    thread t1("");
    t1.run(nullptr);
    t1.join(&ret);
    ::fprintf(stderr, "join: %d\n", ret);

    thread t2("");
    t2.run(nullptr);
    t2.join(&ret);
    ::fprintf(stderr, "join: %d\n", ret);

    mythread thr1;
    mythread thr2;
    thr1.run(mythread::process1);
    thr2.run(mythread::process2);

    thr1.join(&ret);
    ::fprintf(stderr, "join: %d\n", ret);
    thr2.join(&ret);
    ::fprintf(stderr, "join: %d\n", ret);
}
