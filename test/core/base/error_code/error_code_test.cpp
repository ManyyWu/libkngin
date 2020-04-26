#include <iostream>
#include "kngin/core/base/error_code.h"

using namespace std;

int
main () {
#if defined(KNGIN_SYSTEM_WIN32)
  //for (int i = 4030; i <= 4095; ++i) {
  //  k::error_code ec((k::errno_type)-i);
  //  cerr << ec.value() << ": " << ec.message() << endl;
  //}
#else
  for (int i = 0; i <= 133; ++i) {
    k::error_code ec((k::errno_type)-i);
    cerr << hex << ec.value() << ": " << ec.message() << endl;
  }
  for (int i = 3000; i <= 3014; ++i) {
    k::error_code ec((k::errno_type)-i);
    cerr << hex << ec.value() << ": " << ec.message() << endl;
  }
#endif

  return 0;
}
