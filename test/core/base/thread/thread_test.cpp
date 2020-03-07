#include "kngin/core/base/thread.h"

using namespace std;

int
main () {
  k::thread t([] (void *) -> int {
    throw_exception("");
    return 0;
  }, nullptr, "thread_test");

  return 0;
}
