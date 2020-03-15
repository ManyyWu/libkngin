#include <iostream>
#include "kngin/core/base/exception.h"
#include "kngin/core/base/common.h"

using namespace std;

int
main () {
  TRY()
    try {
    throw_exception("test");
  } catch (const std::exception &e) {
    cerr << "catch exception: " << e.what() << endl;
    auto &ex = (k::exception &)e;
    cerr << ex.dump() << endl;
    throw;
  }
  IGNORE("main()");

  return 0;
}
