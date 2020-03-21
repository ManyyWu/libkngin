#include "kngin/core/base/memory.h"
#include "kngin/core/base/semaphore.h"
#include "kngin/core/base/thread.h"
#include <iostream>

using namespace std;

int
main () {
  k::semaphore sem(3);

  k::thread *t[10];
  for (int i = 0; i < 10; ++i) {
    t[i] = new k::thread([&, i]() -> int {
      sem.wait();
      cerr << i;
      k::thread::sleep(3000);
      sem.post();
      return 0;
    });
  }

  for (int i = 0; i < 10; ++i) {
    t[i]->join();
    k::safe_release(t[i]);
  }

  return 0;
}
