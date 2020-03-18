#include "kngin/core/base/rmutex.h"
#include "kngin/core/base/thread.h"
#include <iostream>
#include <functional>

using namespace std;

int
main () {
  k::rmutex m;
  int i = 10;

  std::function<void ()> func = [&] () {
    if (--i >= 0) {
      m.lock();
      cerr << "lock" << endl;
      func();
      m.unlock();
      cerr << "unlock" << endl;
    }
  };

  k::thread t([&] () -> int {
    func();
    return 0;
  });
  t.join();

  return 0;
}
