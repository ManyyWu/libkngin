#include <cstdio>
#include "../libkngin/core/sync_queue.h"
#include "../libkngin/core/thread.h"

using namespace k;
using namespace std;

static int
producer (void *_args)
{
    sync_queue<string> *_q = (sync_queue<string> *)_args;
    for (int i = 0; i < 100000; i++) {
        _q->lock();
        char _buf[20];
        sprintf(_buf, "%d", i);
        while (_q->full())
            _q->wait();
        assert(!_q->full() && _q->push(new_nothrow(string(_buf))));
        fprintf(stderr, "-----producer[%ld] put, len: %ld\n",
                thread::self(),
                _q->size());
        fflush(stderr);
        _q->unlock();
        _q->broadcast();
    }
    _q->lock();
    _q->push(new_nothrow(string("")));
    _q->unlock();
    _q->broadcast();
    return 0;
}

static int
comsumer (void *_args)
{
    sync_queue<string> *_q = (sync_queue<string> *)_args;
    while (true) {
        _q->lock();
        while (_q->empty())
            _q->wait();
        string *_s = _q->pop();
        fprintf(stderr, "comsumer[%ld] get \"%s\", len: %ld\n",
                thread::self(),
                _s->c_str(), _q->size());
        fflush(stderr);
        safe_release(_s);
        _q->unlock();
        _q->broadcast();
        if (*_s == "")
            break; 
    }
    return 0;
}

void
sync_queue_test ()
{
    sync_queue<string> *_q = sync_queue<string>::create(100000, true);
    thread t0(producer, _q);
    thread t1(producer, _q);
    thread t2(producer, _q);
    thread t3(comsumer, _q);
    thread t4(comsumer, _q);
    thread t5(comsumer, _q);
    t0.run();
    t1.run();
    t2.run();
    t3.run();
    t4.run();
    t5.run();
    t0.join(NULL);
    t1.join(NULL);
    t2.join(NULL);
    t3.join(NULL);
    t4.join(NULL);
    t5.join(NULL);
}