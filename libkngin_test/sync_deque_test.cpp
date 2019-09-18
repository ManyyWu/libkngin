#include <cstdio>
#include "../libkngin/core/sync_deque.h"
#include "../libkngin/core/thread.h"

using namespace k;
using namespace std;

static int
producer (void *_args)
{
    sync_deque<string> *_q = (sync_deque<string> *)_args;
    for (int i = 0; i < 1000000; i++) {
        
    }
    return 0;
}

static int
comsumer (void *_args)
{
    sync_deque<string> *_q = (sync_deque<string> *)_args;
    while (true) {
    }
    return 0;
}

void
sync_deque_test ()
{
    sync_deque<string> *_q = sync_deque<string>::create(1000, true);
    thread t1(producer, _q);
    thread t2(comsumer, _q);
    t1.run();
    t2.run();
    t1.join(NULL);
    t2.join(NULL);

}