#include "kngin/core/base/system_error.h"
#include <iostream>

using namespace std;

int
main () {
  try {
    throw k::system_error("EAGAIN", ERRNO(EAGAIN));
  } catch (std::exception &e) {
    cerr << "catch exception: " << e.what() << endl;
  }

#if defined(KNGIN_SYSTEM_WIN32)
  try {
    throw k::system_error("WSAEWOULDBLOCK", WSAEWOULDBLOCK);
  } catch (std::exception &e) {
    cerr << "catch exception: " << e.what() << endl;
  }
#endif

  return 0;
}
