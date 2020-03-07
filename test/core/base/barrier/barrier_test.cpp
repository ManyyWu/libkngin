#include "kngin/core/base/barrier.h"
#include "kngin/core/base/thread.h"
#include <iostream>

using namespace std;

int
main () {
  k::barrier b(4);

  k::thread t([&b] (void *) -> int {
    if (b.wait()) {
      b.destroy();
      cerr << "t destoryed barrier" << endl;
    }
    return 0;
  });
  k::thread t1([&b] (void *) -> int {
    if (b.wait()) {
      b.destroy();
      cerr << "t1 destoryed barrier" << endl;
    }
    return 0;
  });
  k::thread t2([&b] (void *) -> int {
    if (b.wait()) {
      b.destroy();
      cerr << "t2 destoryed barrier" << endl;
    }
    return 0;
  });

  if (b.wait()) {
    b.destroy();
    cerr << "main thread destoryed barrier" << endl;
  }

  t.join();
  t1.join();
  t2.join();

  return 0;
}
