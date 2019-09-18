#include <cstdio>
#include "../libkngin/core/sync_queue.h"
#include "../libkngin/core/thread.h"

using namespace k;
using namespace std;

static int
producer (void *_args)
{
    sync_queue<string> *_q = (sync_queue<string> *)_args;
    for (int i = 0; i < 1000000; i++) {
        _q->lock();
        char _buf[20];
        sprintf(_buf, "%d", i);
        assert(!_q->full() && _q->push(new string(_buf)));
        _q->unlock();
    }
    _q->lock();
    _q->push(new string(""));
    _q->unlock();
    return 0;
}

static int
comsumer (void *_args)
{
    sync_queue<string> *_q = (sync_queue<string> *)_args;
    while (true) {
        _q->lock();
        string *_s = _q->pop();
        assert(!(_s && _q->empty()));
        fprintf(stderr, "comsumer get \"%s\"\n", _s->c_str());
        fflush(stderr);
        delete _s;
        _q->unlock();
        if (*_s == "")
            break; 
    }
    return 0;
}

void
sync_queue_test ()
{
    sync_queue<string> *_q = sync_queue<string>::create(1000, true);
    thread t1(producer, _q);
    thread t2(comsumer, _q);
    t1.run();
    t2.run();
    t1.join(NULL);
    t2.join(NULL);

}