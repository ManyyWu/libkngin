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
        check(_str);
        check(!_q->full() && _q->push_front(&_str));
        ::fprintf(stderr, "-----producer put, len: %ld\n",
                _q->size());
        _q->unlock();
        _q->broadcast();
    }
    _q->lock();
    string *_str = NULL;
    knew(_str, string, (""));
    check(_str);
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
        check(_s);
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
    t0.join(NULL);
    t1.join(NULL);
    t2.join(NULL);
    t3.join(NULL);
    t4.join(NULL);
    t5.join(NULL);
}
