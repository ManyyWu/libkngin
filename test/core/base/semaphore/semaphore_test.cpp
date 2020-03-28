#include "kngin/core/base/memory.h"
#include "kngin/core/base/semaphore.h"
#include "kngin/core/base/thread.h"
#include <cstdio>

int
main () {
  k::semaphore sem(10);

  k::thread *t[50];
  for (int i = 0; i < 50; ++i) {
    t[i] = new k::thread([&, i]() -> int {
      k::semaphore::scoped_sem s(sem);
      fprintf(stderr, "%d ", i);
      k::thread::sleep(1000);
      return 0;
    });
  }

  for (int i = 0; i < 50; ++i) {
    t[i]->join();
    k::safe_release(t[i]);
  }

  return 0;
}
