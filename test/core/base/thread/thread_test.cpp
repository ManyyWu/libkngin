#include "kngin/core/base/log.h"
#include "kngin/core/base/thread.h"

using namespace std;

int
main () {
  k::thread t([] () -> int {
    throw_exception("");
    return 0;
  }, "thread_test");
  t.join();

  return 0;
}
