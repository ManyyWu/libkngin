#include "kngin/core/base/mutex.h"
#include <iostream>
#include <thread>

using namespace std;

int
main () {
  k::mutex m;
  int v = 0;

  thread t([&] () {
    for (int i = 0; i < 10000000; ++i) {
      {
        ++v;
      }
    }
  });

  thread t1([&] () {
    for (int i = 0; i < 10000000; ++i) {
      {
        ++v;
      }
    }
  });

  t.join();
  t1.join();
  cout << v << endl;

  v = 0;
  thread t2([&] () {
    for (int i = 0; i < 10000000; ++i) {
      {
        k::mutex::scoped_lock lock(m);
        ++v;
      }
    }
  });

  thread t3([&] () {
    for (int i = 0; i < 10000000; ++i) {
      {
        k::mutex::scoped_lock lock(m);
        ++v;
      }
    }
  });

  t2.join();
  t3.join();
  cout << v << endl;

  return 0;
}
