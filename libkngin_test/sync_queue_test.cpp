#include <cstdio>
#include "sync_queue.h"
#include "thread.h"

#ifdef __FILENAME__
#undef __FILENAME__
#endif
#define __FILENAME__ "libkngin_test/sync_queue_test.cpp"

using namespace k;
using namespace std;

static int
producer (void *_args)
{
    sync_queue<string> *_q = (sync_queue<string> *)_args;
    for (int i = 0; i < 100; ++i) {
        _q->lock();
        char _buf[20];
        ::sprintf(_buf, "%d", i);
        while (_q->full())
            _q->wait();
        string *_str = nullptr;
        knew(_str, string, (_buf));
        check(_str);
        check(!_q->full() && _q->push(&_str));
        ::fprintf(stderr, "-----producer put, len: %ld\n",
                _q->size());
        _q->unlock();
        _q->broadcast();
    }
    _q->lock();
    string *_str = nullptr;
    knew(_str, string, (""));
    check(_str);
    _q->push(&_str);
    _q->unlock();
    _q->broadcast();
    return 0;
}

static int
comsumer (void *_args)
{
    sync_queue<string> *_q = (sync_queue<string> *)_args;
    bool _done = false;
    while (!_done) {
        _q->lock();
        while (_q->empty())
            _q->wait();
        string *_s = _q->pop();
        ::fprintf(stderr, "comsumer get \"%s\", len: %ldd\n",
                _s->c_str(), _q->size());
        if ("" == *_s)
            _done = true;
        kdelete(_s);
        _q->unlock();
        _q->broadcast();
    }
    return 0;
}

void
sync_queue_test ()
{
    sync_queue<string> _q;
    k::thread t0("");
    k::thread t1("");
    k::thread t2("");
    k::thread t3("");
    k::thread t4("");
    k::thread t5("");
    t0.run(std::bind(producer, &_q));
    t1.run(std::bind(producer, &_q));
    t2.run(std::bind(producer, &_q));
    t3.run(std::bind(comsumer, &_q));
    t4.run(std::bind(comsumer, &_q));
    t5.run(std::bind(comsumer, &_q));
    t0.join(nullptr);
    t1.join(nullptr);
    t2.join(nullptr);
    t3.join(nullptr);
    t4.join(nullptr);
    t5.join(nullptr);
}