#include "kngin/core/base/system_error.h"
#if defined(KNGIN_SYSTEM_WIN32)
#include <Windows.h>
#endif /* defined(KNGIN_SYSTEM_WIN32) */
#include <iostream>

using namespace std;

int
main () {
  try {
    throw_system_error("EAGAIN", ERRNO(EAGAIN));
  } catch (const std::exception &e) {
    cerr << "catch exception: " << e.what() << endl;
  }

#if defined(KNGIN_SYSTEM_WIN32)
  try {
    throw_system_error("WSAEWOULDBLOCK", WSAEWOULDBLOCK);
  } catch (const std::exception &e) {
    cerr << "catch exception: " << e.what() << endl;
  }
#endif

  return 0;
}
