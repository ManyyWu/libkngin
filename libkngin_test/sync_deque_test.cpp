#include <cstdio>
#include "sync_deque.h"
#include "thread.h"

using namespace k;
using namespace std;

static int
producer (void *_args)
{
    sync_deque<string> *_q = (sync_deque<string> *)_args;
    for (int i = 0; i < 100; i++) {
        _q->lock();
        char _buf[20];
        ::sprintf(_buf, "%d", i);
        while (_q->full())
            _q->wait();
        string *_str = NULL;
        knew(_str, string, (_buf));
        kassert(_str);
        kassert(!_q->full() && _q->push_front(&_str));
        ::fprintf(stderr, "-----producer put, len: %ld\n",
                _q->size());
        _q->unlock();
        _q->broadcast();
    }
    _q->lock();
    string *_str = NULL;
    knew(_str, string, (""));
    kassert(_str);
    _q->push_front(&_str);
    _q->unlock();
    _q->broadcast();
    return 0;
}

static int
comsumer (void *_args)
{
    sync_deque<string> *_q = (sync_deque<string> *)_args;
    bool _done = false;
    while (!_done) {
        _q->lock();
        while (_q->empty())
            _q->wait();
        string *_s = _q->back();
        kassert(_s);
        _q->pop_back();
        ::fprintf(stderr, "comsumer get \"%s\", len: %ld\n",
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
sync_deque_test ()
{
    sync_deque<string> _q;
    k::thread t0(std::bind(producer, &_q));
    k::thread t1(std::bind(producer, &_q));
    k::thread t2(std::bind(producer, &_q));
    k::thread t3(std::bind(comsumer, &_q));
    k::thread t4(std::bind(comsumer, &_q));
    k::thread t5(std::bind(comsumer, &_q));
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
