#include <iostream>
#include "kngin/core/base/exception.h"

using namespace std;

int
main () {
  try {
    throw_exception("test");
  } catch (std::exception &e) {
    cerr << "catch exception: " << e.what() << endl;
    auto &ex = (k::exception &)e;
    cerr << ex.dump() << endl;
  }

  return 0;
}
