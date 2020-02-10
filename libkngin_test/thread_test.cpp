#include <cstdio>
#include "../libkngin/core/base/thread.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin_test/thread_test.cpp"

using namespace k;

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

extern void
thread_test ()
{
    thread thr1("");
    thread thr2("");
    thr1.run(process1);
    thr2.run(process2);
    ::fprintf(stderr, "join: %d\n", thr1.join());
    ::fprintf(stderr, "join: %d\n", thr2.join());
}
