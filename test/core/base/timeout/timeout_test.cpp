#include "kngin/core/base/timeout.h"
#include "kngin/core/base/thread.h"
#include <iostream>

using namespace std;

int
main () {
  k::timeout t(3000, false);
  while (true) {
    k::thread::sleep(100);
    cerr << t.remaining().value() << " ";
    if (!t.timed_out())
      continue;
    else
      break;
  }

  return 0;
}
