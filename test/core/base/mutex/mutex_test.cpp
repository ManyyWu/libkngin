#include "kngin/core/base/mutex.h"
#include "kngin/core/base/thread.h"
#include <iostream>
#include <thread>

using namespace std;

int
main () {
  k::mutex m;
  int v = 0;

  k::thread t([&] (void *) -> int {
    for (int i = 0; i < 10000000; ++i) {
      {
        ++v;
      }
    }
    return 0;
  });

  k::thread t1([&] (void *) -> int {
    for (int i = 0; i < 10000000; ++i) {
      {
        ++v;
      }
    }
    return 0;
  });

  t.join();
  t1.join();
  cout << v << endl;

  v = 0;
  k::thread t2([&] (void *) -> int {
    for (int i = 0; i < 10000000; ++i) {
      {
        k::mutex::scoped_lock lock(m);
        ++v;
      }
    }
    return 0;
  });

  k::thread t3([&] (void *) -> int {
    for (int i = 0; i < 10000000; ++i) {
      {
        k::mutex::scoped_lock lock(m);
        ++v;
      }
    }
    return 0;
  });

  t2.join();
  t3.join();
  cout << v << endl;

  return 0;
}
