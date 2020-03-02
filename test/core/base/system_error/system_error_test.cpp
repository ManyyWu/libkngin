#include "kngin/core/base/system_error.h"
#include <iostream>

using namespace std;

int
main () {
  try {
    throw k::mutex_test("EAGAIN", CERR(EAGAIN));
  } catch (std::exception &e) {
    cerr << "catch exception: " << e.what() << endl;
  }

#if defined(KNGIN_SYSTEM_WIN32)
  try {
    throw k::mutex_test("WSAEWOULDBLOCK", WSAEWOULDBLOCK);
  } catch (std::exception &e) {
    cerr << "catch exception: " << e.what() << endl;
  }
#endif

  return 0;
}
