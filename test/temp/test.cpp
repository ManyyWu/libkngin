#include "libkngin.h"
#include <iostream>

using namespace std;

int
main () {
  {
    auto func = [] () {
    };
    k::timestamp begin(k::timestamp::monotonic());
    for (int i = 0; i < 10000000; ++i)
      func();
    cerr << (k::timestamp::monotonic() - begin).value() << endl;
  }

  {
    class c {
    public:
      void
      fun () {
      }
    };
    c a;
    k::timestamp begin(k::timestamp::monotonic());
    for (int i = 0; i < 10000000; ++i)
      a.fun();
    cerr << (k::timestamp::monotonic() - begin).value() << endl;
  }

  {
    class a {
    public:
      a (std::function<void (void)> &&f) {
        f_ = std::move(f);
      }

      void
      func () {
        f_();
      }

    private:
      std::function<void (void)> f_;
    };
    class b {
    public:
      b () : a_(std::bind(&b::func, this)) {
      }

      void
      func () {

      }

      a &
      query_a () {
        return a_;
      }

    private:
      a a_;
    };
    b b_obj;
    auto a_obj = b_obj.query_a();
    k::timestamp begin(k::timestamp::monotonic());
    for (int i = 0; i < 10000000; ++i)
      a_obj.func();
    cerr << (k::timestamp::monotonic() - begin).value() << endl;
  }

  {
    class a {
    public:
      a (std::function<void (void)> &&f) {
        f_ = std::move(f);
      }

      void
      func () {
        f_();
      }

    private:
      std::function<void (void)> f_;
    };
    class b {
    public:
      b (): a_([this] () {
        this->func();
      }) {
      }

      void
      func () {

      }

      a &
      query_a () {
        return a_;
      }

    private:
      a a_;
    };
    b b_obj;
    auto a_obj = b_obj.query_a();
    k::timestamp begin(k::timestamp::monotonic());
    for (int i = 0; i < 10000000; ++i)
      a_obj.func();
    cerr << (k::timestamp::monotonic() - begin).value() << endl;
  }

  {
    class c {
    public:
      void
      fun () {
      }
    };
    c a;
    auto func = bind(&c::fun, &a);
    k::timestamp begin(k::timestamp::monotonic());
    for (int i = 0; i < 10000000; ++i)
      func();
    cerr << (k::timestamp::monotonic() - begin).value() << endl;
  }

  {
    class c {
    public:
      void
      fun () {
      }
    };
    c a;
    auto func1 = [&a] () {
      a.fun();
    };
    auto func = func1;
    k::timestamp begin(k::timestamp::monotonic());
    for (int i = 0; i < 10000000; ++i)
      func();
    cerr << (k::timestamp::monotonic() - begin).value() << endl;
  }

  {
    class c {
    public:
      void
      fun () {
      }
    };
    c a;
    auto func1 = [&a] () {
      a.fun();
    };
    auto func = bind(func1);
    k::timestamp begin(k::timestamp::monotonic());
    for (int i = 0; i < 10000000; ++i)
      func();
    cerr << (k::timestamp::monotonic() - begin).value() << endl;
  }

  return 0;
}
