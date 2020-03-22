#include "kngin/core/base/log.h"
#include "kngin/core/base/thread.h"

using namespace std;

int
main () {
  k::thread::thread_opt opt = {1};

  k::thread t([] () -> int {
    //throw_exception("test");
    return 0;
  }, "thread_test", &opt);
  t.join();

  return 0;
}
