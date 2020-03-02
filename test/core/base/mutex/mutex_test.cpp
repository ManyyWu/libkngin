#include "kngin/core/base/mutex.h"
#include <iostream>

using namespace std;

int
main () {
  k::mutex m;
  k::mutex::scope_lock lock(m);

  return 0;
}
