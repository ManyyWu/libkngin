#include "kngin/core/base/barrier.h"
#include "kngin/core/base/thread.h"
#include <cstdio>

using namespace std;

int
main () {
  k::barrier b(4);

  k::thread t([&b] () -> int {
    if (b.wait()) {
      b.destroy();
      fprintf(stderr, "t destoryed barrier\n");
    }
    return 0;
  });
  k::thread t1([&b] () -> int {
    if (b.wait()) {
      b.destroy();
      fprintf(stderr, "t1 destoryed barrier\n");
    }
    return 0;
  });
  k::thread t2([&b] () -> int {
    if (b.wait()) {
      b.destroy();
      fprintf(stderr, "t2 destoryed barrier\n");
    }
    return 0;
  });

  if (b.wait()) {
    b.destroy();
    fprintf(stderr, "main destoryed barrier\n");
  }

  t.join();
  t1.join();
  t2.join();

  return 0;
}
